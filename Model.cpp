#include "Arduino.h"
#include "Model.h"
#include "Controller.h"
#include "ESP8266WiFi.h"
#include "NTPClient.h"
#include "WiFiUdp.h"
#include "FS.h"
#include "ArduinoJson.h"
#include "DHTesp.h"

#define DHTpin 3
#define NUMBER_OF_RECORDS 10

DHTesp dht;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

bool Model::autoMode = true;
int Model::interval = 3;
int Model::desiredTemperature = 30;
String Model::espSsid;
String Model::espPassword;

bool relayStatuses[2] = {false, false};
int relayPins[2]= {4, 16};
int currentRecord;
int currentTemperature;
String arrData[NUMBER_OF_RECORDS];

Controller controller;

void Model::initialization (String espSsid, String espPassword) {
  Model::espSsid = espSsid;
  Model::espPassword = espPassword;
  this->checkSPIFFS();

  if (!WiFi.status()) {
    WiFi.softAP(espSsid, espPassword);
  };

  controller.initialization();
  timeClient.begin();
  timeClient.setTimeOffset(3600 * 2);
  dht.setup(DHTpin, DHTesp::DHT11);

  pinMode(relayPins[0], OUTPUT);
  pinMode(relayPins[1], OUTPUT);

  digitalWrite(relayPins[0], LOW);
  digitalWrite(relayPins[1], LOW);
}

void Model::checkSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

void Model::dataRecording() {
  timeClient.update();

  long timeStamp = timeClient.getEpochTime();
  String statusInfo = dht.getStatusString();
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  DynamicJsonDocument doc(1024);
  doc["timeStamp"] = convertToIsoTime(timeStamp);
  doc["statusInfo"] = statusInfo;
  doc["humidity"] =  humidity;
  doc["temperature"] = temperature;
  currentTemperature = temperature;

  char jsonData[256];
  serializeJson(doc, jsonData);

  arrData[currentRecord] = jsonData;
  currentRecord++;
  if (currentRecord == NUMBER_OF_RECORDS) {
    currentRecord = 0;
  }
}

String Model::convertToIsoTime(long timeStamp) {
  time_t rawtime = timeStamp;
  struct tm * ti;
  ti = localtime(&rawtime);

  uint16_t year = ti->tm_year + 1900;
  String yearStr = String(year);

  uint8_t month = ti->tm_mon + 1;
  String monthStr = month < 10 ? "0" + String(month) : String(month);

  uint8_t day = ti->tm_mday;
  String dayStr = day < 10 ? "0" + String(day) : String(day);

  uint8_t hours = ti->tm_hour;
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  uint8_t minutes = ti->tm_min;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  uint8_t seconds = ti->tm_sec;
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

  return yearStr + "-" + monthStr + "-" + dayStr + " " +
         hoursStr + ":" + minuteStr + ":" + secondStr;
}

String Model::getData() {
  String json;
  DynamicJsonDocument doc(10240);
  JsonArray data = doc.createNestedArray("data");

  for (int i = currentRecord; i < NUMBER_OF_RECORDS; i++) {
    if (arrData[i] != NULL) {
      data.add(arrData[i]);
    }
  }

  for (int i = 0; i < currentRecord; i++) {
    if (arrData[i] != NULL) {
      data.add(arrData[i]);
    }
  }

  serializeJson(doc, json);

  return (json);
}

String Model::getLocalIP() {
  char wifiLocalIP[16];
  String json;
  DynamicJsonDocument doc(64);
  sprintf(wifiLocalIP, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  doc["wifiLocalIP"] = wifiLocalIP;
  serializeJson(doc, json);
  return json;
}

String Model::getFreeMemory() {
  String json;
  DynamicJsonDocument doc(32);
  doc["freeMemory"] = ESP.getFreeHeap();
  serializeJson(doc, json);
  return json;
}

String Model::scanWiFi() {
  String json;
  DynamicJsonDocument doc(1024);
  JsonArray networks = doc.createNestedArray("networks");
  int networksFound = WiFi.scanComplete();

  if (networksFound == -2) {
    WiFi.scanNetworks(true);
  } else if (networksFound) {
    for (int i = 0; i < networksFound; ++i) {
      networks.add(WiFi.SSID(i));
    }
    WiFi.scanDelete();
    if (WiFi.scanComplete() == -2) {
      WiFi.scanNetworks(true);
    }
  }

  serializeJson(doc, json);
  return json;
}

void Model::disconnectWiFi() {
  WiFi.disconnect();
}

String Model::statusWiFi() {
  DynamicJsonDocument doc(32);
  String json;
  doc["statusWiFi"] = WiFi.status();
  serializeJson(doc, json);
  return json;
}

void Model::authorization(String ssid, String password) {
  WiFi.softAPdisconnect(true);
  WiFi.begin(ssid, password);
}

void Model::restartESP() {
  ESP.restart();
}

void Model::switchRelay(int id, bool enable) {
  digitalWrite(relayPins[id], enable ? HIGH : LOW);
  relayStatuses[id] = enable;
}

void Model::climateControl() {
  if (autoMode) {
    if (Model::desiredTemperature < currentTemperature) {
      Model::switchRelay(0, true);
      Model::switchRelay(1, true);
    } else if (Model::desiredTemperature > currentTemperature) {
      Model::switchRelay(0, false);
      Model::switchRelay(1, false);
    }
  }
}

String Model::getRelayStatuses() {
  String json;
  DynamicJsonDocument doc(64);
  JsonArray statuses = doc.createNestedArray("statuses");
  statuses.add(relayStatuses[0]);
  statuses.add(relayStatuses[1]);

  serializeJson(doc, json);
  return json;
}
