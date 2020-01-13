#ifndef  Model_h
#define  Model_h

#include  "Arduino.h"

class Model {
  public:
    void initialization(String ssid, String password);
    void dataRecording();
    static int interval;
    static String espSsid;
    static String espPassword;
    static String getFreeMemory();
    static String getLocalIP();
    static String scanWiFi();
    static String disconnectWiFi();
    static String statusWiFi();
    static String authorization(String ssid, String password);
    static String restartESP();
    static String getData();
    static String switchRelay(int id, bool enable);
    static String changeInterval(String interval);
  private:
    void checkSPIFFS();
    String convertToIsoTime(long timeStamp);
};

#endif
