
#ifndef JMRI_WIFI_h
#define JMRI_WIFI_h

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "config.h"
#include "newconfig.h"



class WifiInterface{
  public:
 
          static void   wifi_setup (    const char *wifiESSID,
                                     const char *wifiPassword);                         
          static float  getLatestVer();
  private:

          static void  scan();
          static float  _jrmi_mqtt_v_latest ;
};

#endif
