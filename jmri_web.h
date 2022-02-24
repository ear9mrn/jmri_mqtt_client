#ifndef JMRI_WEB_h
#define JMRI_WEB_h

#include "config.h"
#include "headers.h"
#include <ESPAsyncWebServer.h>

extern jmriData jmri_data;

class JMRI_WEB{

    public:
    
        static void   web_init();
        static void   notFound(AsyncWebServerRequest *request);
        static void   update_mqtt_host(char* mqtt_host);
        static void   update_mqtt_port(ushort mqtt_port);
        static void   update_mqtt_topic(char* mqtt_topic);
        static void   makejson(AsyncResponseStream *response);      
        static bool   shouldReboot();
        static void   send(char* buf, const char* target, unsigned long the_time );
  
    private:

        static uint8_t progress;
        static int     fsize;
        static AsyncWebServer server;
        static AsyncEventSource events;
        static bool    _shouldReboot;

};
#endif
