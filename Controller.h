#ifndef  Controller_h
#define  Controller_h
#include  "Arduino.h"
#include <ESPAsyncWebServer.h>

class Controller {
  public:
    void initialization();
  private:
    void mapping();
};

#endif
