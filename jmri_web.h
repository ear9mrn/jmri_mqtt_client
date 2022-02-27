/*
 *  © 2022 Pete Nevill
 *  All rights reserved.
 *
 *  This file is part of JMRI MQTT Accesssory Client
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */
 
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
