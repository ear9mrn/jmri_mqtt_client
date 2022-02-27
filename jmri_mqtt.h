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
 
#ifndef JMRI_MQTT_h
#define JMRI_MQTT_h

#include "headers.h"
#include "config.h"
#include <PubSubClient.h>

extern jmriData jmri_data;

class JMRI_MQTT{

  public:

          static void       mqtt_init();
          static void       callback(char* topic, byte* message, unsigned int length);
          static void       loop();
          static void       disconnect();
          static bool       connected();
          static void       disconnect_mqtt();
          static void       hostIPupdate(const char *server_host, ushort port);         
          static void       topicupdate(const char *topic);  
          //static void       sendMessage(ushort sysname, char* newstate);
          static void       sendMessage(Messg messg);

  
  private:

          static PubSubClient       mqttclient; 
          static WiFiClient         mqtt_wifi_client;
};  
#endif
