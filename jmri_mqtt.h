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
