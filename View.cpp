#include "Arduino.h"
#include "View.h"

String View::processor(const String & var) {
  if (var == "TITLE")
    return "ESP8266";
  if (var == "YAC")
    return "You are connected";
  if (var == "SWF")
    return "Scanning Wi-Fi:";
  if (var == "LI")
    return "Local ip:";
  if (var == "D")
    return "Disconnect";
  if (var == "H")
    return "Home";
  if (var == "CTWF")
    return "Connect to Wi-Fi";
  if (var == "c")
    return "connection";
}
