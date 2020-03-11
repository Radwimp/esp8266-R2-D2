#include "Model.h"

const String espSsid = "ESP8266";
const String espPassword = "12345678";
unsigned long previousMillis;
unsigned long bottomMillis = 0;
unsigned long sideMillis = 0;
int bottomInterval;
int sideInterval;

Model model;

void setup() {
  Serial.begin(115200);
  Serial.println("start " + espSsid);
  model.initialization(espSsid, espPassword);
  model.dataRecording();
}

void loop() {
  unsigned long currentMillis = millis();

  if ((unsigned long)(currentMillis - previousMillis) >= Model::updateInterval * 1000) {
    model.dataRecording();
    model.climateControl();
    previousMillis = currentMillis;
  }

  if (Model::relayStatuses[0]) {
    if ((unsigned long)(currentMillis >= bottomMillis) && bottomMillis) {
      if(bottomInterval == Model::enabledInterval) {
        digitalWrite(Model::relayPins[0], HIGH);
        bottomInterval = Model::disabledInterval;
        bottomMillis = currentMillis + bottomInterval * 1000;
      } else if(bottomInterval == Model::disabledInterval) {
        digitalWrite(Model::relayPins[0], LOW);
        bottomInterval = Model::enabledInterval;
        bottomMillis = currentMillis + bottomInterval * 1000;
      } else {
        bottomInterval = Model::enabledInterval;
      }
    }

    if (!bottomMillis) {
      bottomInterval = Model::enabledInterval;
      bottomMillis = currentMillis + bottomInterval * 1000;
    }
  }
  
  if (Model::relayStatuses[1]) {
    if ((unsigned long)(currentMillis >= sideMillis) && sideMillis) {
      if(sideInterval == Model::enabledInterval) {
        digitalWrite(Model::relayPins[1], HIGH);
        sideInterval = Model::disabledInterval;
        sideMillis = currentMillis + bottomInterval * 1000;
      } else if(sideInterval == Model::disabledInterval) {
        digitalWrite(Model::relayPins[1], LOW);
        sideInterval = Model::enabledInterval;
        sideMillis = currentMillis + sideInterval * 1000;
      } else {
        sideInterval = Model::enabledInterval;
      }
    }

    if (!sideMillis) {
      sideInterval = Model::enabledInterval;
      sideMillis = currentMillis + sideInterval * 1000;
    }
  }
}
