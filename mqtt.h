
#ifndef MQTT_h
#define MQTT_h

#include  "config.h"

struct JMRI_MQTT{

          static JMRI_MQTT  *jmri_mqtt;
          jmriData          *jmri_ptr;
          static void       mqtt_init(jmriData *jmri_data);
          static void       callback(char* topic, byte* message, unsigned int length);
          static void       reconnect();
          static void       disconnect_mqtt();
          static void       hostIPupdate();         
          static void       sendMessage(ushort sysname, char* newstate);
          //static void       publish(char* topic, char* message);
          static int        mqttAddress;
          static int        pointer();
  
};


#endif
