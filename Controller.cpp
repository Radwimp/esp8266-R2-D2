#include "Arduino.h"
#include "Controller.h"
#include "Model.h"
#include "View.h"
#include "ESPAsyncWebServer.h"
#include "ArduinoJson.h"

AsyncWebServer server(80);

void Controller::initialization () {
  this->mapping();
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  server.begin();
  Serial.println("end initialization");
}

void Controller::mapping() {
  server.onNotFound([](AsyncWebServerRequest * request) {
    if (request->method() == HTTP_OPTIONS) {
      request->send(200);
    } else {
      request->send(404);
    }
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, View::processor);
  });

  server.on("/connect-to-wi-fi", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/connect.html", String(), false, View::processor);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/home.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/home.js", "application/javascript");
  });

  server.on("/connect.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/connect.js", "application/javascript");
  });

  server.on("/jquery-3.4.1.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/jquery-3.4.1.min.js", "application/javascript");
  });

  server.on("/vue.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/vue.js", "application/javascript");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });

  server.on("/localIP", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::getLocalIP());
  });

  server.on("/freeMemory", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::getFreeMemory());
  });

  server.on("/scanWiFi", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::scanWiFi());
  });

  server.on("/disconnectWiFi", HTTP_GET, [](AsyncWebServerRequest * request) {
    Model::disconnectWiFi();
    request->send(200);
  });

  server.on("/statusWiFi", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::statusWiFi());
  });

  server.on("/restartESP", HTTP_GET, [](AsyncWebServerRequest * request) {
    Model::restartESP();
    request->send(200);
  });

  server.on("/getData", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::getData());
  });

  server.on("/getTemperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::getTemperature());
  });

  server.on("/getRelayStatuses", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::getRelayStatuses());
  });

  server.on("/getIntervals", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::getIntervals());
  });

  server.on("/authorization", HTTP_POST, [](AsyncWebServerRequest * request) {
    String ssid;
    String password;
    
    if (request->hasArg("ssid")) {
      ssid = request->arg("ssid");
    }
    
    if (request->hasArg("password")) {
      password = request->arg("password");
    }
    
    Model::authorization(ssid, password);
    request->send(200);
  });

  server.on("/switchRelay", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    const String json = (char*)data;
    DynamicJsonDocument doc(512);
    deserializeJson(doc, json);
    const int id = doc["id"];
    const bool enable = doc["enable"];
    
    Model::switchRelay(id, enable);
    request->send(200);
  });

  server.on("/setIntervals", HTTP_POST, [](AsyncWebServerRequest * request) {}, NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    const String json = (char*)data;
    DynamicJsonDocument doc(512);
    deserializeJson(doc, json);
    
    Model::updateInterval = doc["update"];
    Model::enabledInterval = doc["enabled"];
    Model::disabledInterval = doc["disabled"];
    request->send(200);
  });
  
  server.on("/changeDesiredTemperature", HTTP_POST, [](AsyncWebServerRequest * request) {}, NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    const String json = (char*)data;
    DynamicJsonDocument doc(512);
    deserializeJson(doc, json);
    
    Model::desiredTemperature = doc["temperature"];
    request->send(200);
  });
  
  server.on("/setMode", HTTP_POST, [](AsyncWebServerRequest * request) {}, NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    const String json = (char*)data;
    DynamicJsonDocument doc(512);
    deserializeJson(doc, json);
    
    Model::autoMode = doc["autoMode"];
    request->send(200);
  }); 
}
