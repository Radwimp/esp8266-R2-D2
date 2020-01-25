#include "Model.h"

const String espSsid = "ESP8266";
const String espPassword = "12345678";
unsigned long previousMillis;

Model model;

void setup() {
  Serial.begin(115200);
  Serial.println("start " + espSsid);
  model.initialization(espSsid, espPassword);
}

void loop() {
  unsigned long currentMillis = millis();

  if ((unsigned long)(currentMillis - previousMillis) >= Model::interval * 1000 * 60) {
    model.dataRecording();
    model.climateControl();
    previousMillis = currentMillis;
  }
}
