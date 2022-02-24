
#ifndef JMRI_WIFI_h
#define JMRI_WIFI_h

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "config.h"
#include "headers.h"

extern jmriData jmri_data;

class WifiInterface{
  public:
 
          static void   wifi_setup ();                         
          static float  getLatestVer();
  private:

          static void  scan();
          static float  _jrmi_mqtt_v_latest ;
};

#endif
