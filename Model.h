#ifndef  Model_h
#define  Model_h

#include  "Arduino.h"

class Model {
  public:
    void initialization(String ssid, String password);
    static String getFreeMemory();
    static String getLocalIP();
    static String scannerWiFi();
    static String disconnectWiFi();
    static String statusWiFi();
    static String authorization(String ssid, String password);
    static String restartESP();
    static String getData();
    void dataRecording();
  private:
    void checkSPIFFS();
    String convertToIsoTime(long timeStamp);
};

#endif
