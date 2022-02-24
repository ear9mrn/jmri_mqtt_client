
#ifndef TELNET_h
#define TELNET_h

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "headers.h"
#include "config.h"

class jmri_telnet {

  public:
  
        static void     tel_init();
        static void     telPrint(char *text);
        static void     clientConnected();
        static bool     telnetUp();
            
  private:

        static WiFiClient   telnetClient;
        static WiFiServer   *telnetServer;
        
};

#endif
