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
