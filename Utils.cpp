#include "Arduino.h"
#include "Utils.h"
#include "ArduinoJson.h"

String Utils::jsonFormatter(int size, int argsAmount, unsigned first, ...) {
  String json;
  DynamicJsonDocument doc(size);
  JsonArray data = doc.createNestedArray("data");
  unsigned *p = &first;

  while(argsAmount--) {
    data.add(*p);
  }
  
  serializeJson(doc, json);
  return json;
}
