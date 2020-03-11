#include "Arduino.h"
#include "View.h"
#include "NTPClient.h"
#include "DHTesp.h"
#include "OneWire.h"
#include "DallasTemperature.h"

DHTesp dht;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

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

String Model::getTemperature() {
  char temperatureCString[6];
  float tempC;
  String json;
  DynamicJsonDocument doc(32);

  DS18B20.requestTemperatures();
  tempC = DS18B20.getTempCByIndex(0);
  dtostrf(tempC, 2, 2, temperatureCString);

  doc["temperature"] = temperatureCString;
  serializeJson(doc, json);

  return (json);
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
