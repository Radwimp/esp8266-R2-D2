#include "Arduino.h"
#include "Model.h"
#include "Controller.h"
#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
#include "FS.h"

#define ONE_WIRE_BUS 5
#define NUMBER_OF_RECORDS 10

DHTesp dht;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
IPAddress ip(192,168,0,144);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

bool Model::autoMode = false;
bool Model::relayStatuses[2] = {false, false};
int Model::relayPins[2]= {1, 16};
int Model::updateInterval = 180;
int Model::enabledInterval = 10;
int Model::disabledInterval = 5;
int Model::desiredTemperature = 30;
String Model::espSsid;
String Model::espPassword;

int currentRecord;
int currentTemperature;
String arrData[NUMBER_OF_RECORDS];

Controller controller;

void Model::initialization (String espSsid, String espPassword) {
  Model::espSsid = espSsid;
  Model::espPassword = espPassword;
  this->checkSPIFFS();

  Model::authorization("Marshmallow Justice", "11111111");

  if (!WiFi.status()) {
    WiFi.softAP(espSsid, espPassword);
  };

  DS18B20.begin();
  controller.initialization();
  timeClient.begin();
  timeClient.setTimeOffset(3600 * 2);
  dht.setup(DHTpin, DHTesp::DHT11);

  pinMode(Model::relayPins[0], OUTPUT);
  pinMode(Model::relayPins[1], OUTPUT);

  digitalWrite(Model::relayPins[0], HIGH);
  digitalWrite(Model::relayPins[1], HIGH);
}

void Model::checkSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
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
  WiFi.softAP(espSsid, espPassword);
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
  WiFi.config(ip, gateway, subnet);
}

void Model::restartESP() {
  ESP.restart();
}

void Model::switchRelay(int id, bool enable) {
  digitalWrite(Model::relayPins[id], enable ? LOW : HIGH);
  Model::relayStatuses[id] = enable;
}

void Model::climateControl() {
//  if (autoMode) {
//    if (Model::desiredTemperature > currentTemperature) {
//      Model::switchRelay(0, true);
//      Model::switchRelay(1, true);
//    } else if (Model::desiredTemperature < currentTemperature) {
//      Model::switchRelay(0, false);
//      Model::switchRelay(1, false);
//    }
//  }
}

String Model::getRelayStatuses() {
  String json;
  DynamicJsonDocument doc(64);
  JsonArray statuses = doc.createNestedArray("statuses");
  statuses.add(Model::relayStatuses[0]);
  statuses.add(Model::relayStatuses[1]);

  serializeJson(doc, json);
  return json;
}

String Model::getIntervals() {
  String json;
  DynamicJsonDocument doc(64);
  doc["update"] = Model::updateInterval;
  doc["enabled"] = Model::enabledInterval;
  doc["disabled"] = Model::disabledInterval;


  serializeJson(doc, json);
  return json;
}
