#include "Arduino.h"
#include "Controller.h"
#include "Model.h"
#include "View.h"
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void Controller::initialization () {
  this->mapping();
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();
  Serial.println("end initialization");
}

void Controller::mapping() {
  server.onNotFound([](AsyncWebServerRequest * request) {
    request->send(404);
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

  server.on("/scannerWiFi", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::scannerWiFi());
  });

  server.on("/disconnectWiFi", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::disconnectWiFi());
  });

  server.on("/statusWiFi", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::statusWiFi());
  });

  server.on("/restartESP", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::restartESP());
  });

  server.on("/getData", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", Model::getData());
  });

  server.on("/authorization", HTTP_POST, [](AsyncWebServerRequest * request) {
    String ssid;
    String password;
    if (request->hasArg("ssid"))
      ssid = request->arg("ssid");
    if (request->hasArg("password"))
      password = request->arg("password");
    request->send(200, "text/html", Model::authorization(ssid, password));
  });

}
