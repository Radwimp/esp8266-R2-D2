#ifndef  Model_h
#define  Model_h

#include  "Arduino.h"

class Model {
  public:
    void initialization(String ssid, String password);
    void dataRecording();
    static bool autoMode;
    static int interval;
    static int desiredTemperature;
    static String espSsid;
    static String espPassword;
    static String getFreeMemory();
    static String getLocalIP();
    static String scanWiFi();
    static String statusWiFi();
    static String getData();
    static String getRelayStatuses();
    static void disconnectWiFi();
    static void authorization(String ssid, String password);
    static void restartESP();
    static void switchRelay(int id, bool enable);
    static void changeInterval(int interval);
    static void climateControl();
  private:
    void checkSPIFFS();
    String convertToIsoTime(long timeStamp);
};

#endif
