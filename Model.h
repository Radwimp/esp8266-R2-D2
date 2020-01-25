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
    static String statusWiFi();
    static String getData();
    static void disconnectWiFi();
    static void authorization(String ssid, String password);
    static void restartESP();
    static void switchRelay(int id, bool enable);
    static void changeInterval(int interval);
  private:
    void checkSPIFFS();
    String convertToIsoTime(long timeStamp);
};

#endif
