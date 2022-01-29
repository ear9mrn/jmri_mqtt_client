#ifndef JMRI_WEB_h
#define JMRI_WEB_h

#include <ESPAsyncWebServer.h>
#include  "config.h"
#include <LittleFS.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>

struct JMRI_WEB{

        static JMRI_WEB *jmri_web;
        jmriData      *jmri_ptr;
        static void   web_init(jmriData *jmri_data);
        static void   notFound(AsyncWebServerRequest *request);
        static void   update_mqtt_host(char* mqtt_host);
        static void   update_mqtt_port(ushort mqtt_port);
        static void   update_mqtt_topic(char* mqtt_topic);
        static void   makejson(AsyncResponseStream *response);
        static void   update_started();
        static void   update_finished();
        static void   update_progress(int cur, int total);
        static void   update_error(int err);
        static void   startAPWeb();
        static int    webAddress;
        static int    pointer();
  
};

enum DownloadResult {
    HTTP_DOWNLOAD_FAILED,
    HTTP_DOWNLOAD_OK
};


#endif
