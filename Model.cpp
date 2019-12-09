#include "Arduino.h"
#include "Model.h"
#include "Controller.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "DHTesp.h"

#define DHTpin 3
#define NUMBER_OF_RECORDS 96

DHTesp dht;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String arrData[NUMBER_OF_RECORDS];
int currentRecord;

Controller controller;

void Model::initialization (String espSsid, String espPassword) {
  this->checkSPIFFS();
  WiFi.softAP(espSsid, espPassword);
  controller.initialization();
  timeClient.begin();
  timeClient.setTimeOffset(3600*2);
  dht.setup(DHTpin, DHTesp::DHT11);
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
  float temperatures = dht.getTemperature();

  DynamicJsonDocument doc(1024);
  doc["timeStamp"] = convertToIsoTime(timeStamp);
  doc["statusInfo"] = statusInfo;
  doc["humidity"] =  humidity;
  doc["temperatures"] = temperatures;

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
  Serial.println(WiFi.scanNetworks());
  String json;
  DynamicJsonDocument doc(32);
  doc["freeMemory"] = ESP.getFreeHeap();
  serializeJson(doc, json);
  return json;
}

String Model::scannerWiFi() {
  String json;
  DynamicJsonDocument doc(1024);
  JsonArray data = doc.createNestedArray("networks");
  int count = WiFi.scanComplete();
  if (count == -2) {
    WiFi.scanNetworks(true);
  } else if (count) {
    for (int i = 0; i < count; ++i) {
      data.add(WiFi.SSID(i));
    }
    WiFi.scanDelete();
    if (WiFi.scanComplete() == -2) {
      WiFi.scanNetworks(true);
    }
  }
  serializeJson(doc, json);
  return json;
}

String Model::disconnectWiFi() {
  DynamicJsonDocument doc(32);
  String json;
  if (WiFi.disconnect()) {
    doc["disconnectWiFi"] = "success";
  } else {
    doc["disconnectWiFi"] = "fail";
  };
  serializeJson(doc, json);
  return json;
}

String Model::statusWiFi() {
  DynamicJsonDocument doc(32);
  String json;
  doc["statusWiFi"] = WiFi.status();
  serializeJson(doc, json);
  return json;
}

String Model::authorization(String ssid, String password) {
  WiFi.begin(ssid, password);
  return "connection";
}

String Model::restartESP() {
  DynamicJsonDocument doc(32);
  String json;
  doc["restart"] = "success";
  serializeJson(doc, json);
  Serial.println("restart");
  ESP.restart();
  return json;
}
