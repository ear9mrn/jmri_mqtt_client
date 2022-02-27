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
 
#include  "jmri_mqtt.h"

WiFiClient JMRI_MQTT::mqtt_wifi_client;
PubSubClient JMRI_MQTT::mqttclient(JMRI_MQTT::mqtt_wifi_client);

//initiate MQTT client. Connect and subscribe
void JMRI_MQTT::mqtt_init(){

  
        if (WiFi.status() != WL_CONNECTED ){
            return;
        }

        JMRI_HELPER::logging(1,F("MQTT broker IP address: %s\n"), jmri_data.data.mqtt_server_host );  
        JMRI_HELPER::logging(1,F("MQTT broker port: %d\n\n"),     jmri_data.data.mqtt_server_port );

        //connect and set callback
        mqttclient.setServer(jmri_data.data.mqtt_server_host, (int)jmri_data.data.mqtt_server_port);
        mqttclient.connect("ESPClient");
        //jmri_data->retainMSGTimer = millis();
        mqttclient.setCallback(JMRI_MQTT::callback);

        //subscribe to a topic
        // default: /trains/track/#
        if (mqttclient.state() == 0 ) {
              mqttclient.subscribe( jmri_data.data.mqtt_topic);
              JMRI_HELPER::logging(1,F("Subscribed to: %s\n"), jmri_data.data.mqtt_topic);
        } else {
               
              JMRI_HELPER::logging(1,F("Attempting connection to MQTT broker: %s on port: %d...\nFAILED...\n"), 
                                      jmri_data.data.mqtt_server_host, jmri_data.data.mqtt_server_port);
              delay(5000);
        }
           
}

//disconnect if required (wifi drop!).
void JMRI_MQTT::disconnect_mqtt(){

    if ( mqttclient.state() >= 0 ) {
        mqttclient.disconnect();  
        JMRI_HELPER::logging(1,F("Disconnected from MQTT broker...\n"));
    }
    
}

//callback i.e. messages received
void JMRI_MQTT::callback(char* topic, byte* message, unsigned int length) {

          //work around to ignore retained messaged when first connecting to broker!
          //if (millis() - jmri_data.retainMSGTimer < 2000){
          //  return;
          //}

          char *T[5];
          uint8_t k = 0; 

          char *token = strtok(topic, "/"); 

          //get topic from topic char array 
          if(token){
              T[k++] = strdup(token);
              token = strtok(NULL, "/");
              while(token){
                    T[k++] = strdup(token);
                    token = strtok(NULL, "/"); 
                }
          }
          
          char cmessage[length+1];
                 
          if ( strcmp(T[2], "turnout") != 0  && strcmp(T[2], "light") != 0 ){
            return;
          }
                 
          strncpy(cmessage, (char*)message, length); 
          cmessage[length] = '\0';

          //provide feedback to serial monitor and telnet client and browser console
          JMRI_HELPER::logging(1,F("Message arrived on topic: /%s/%s/%s/%s. Message: %s.\n"),T[0],T[1],T[2],T[3], cmessage); 

          //call helper to deal with action required for messages received
          if ( strcmp(T[2], "turnout") == 0 ){
          
                  JMRI_HELPER::setTurnout( atoi(T[3]), cmessage);                
            
          } else if ( strcmp(T[2], "light") == 0 ){

                  JMRI_HELPER::setLight( atoi(T[3]), cmessage);
                             
          }
                    
}

// helper to update host ip
void JMRI_MQTT::hostIPupdate(const char * mqtt_host, ushort port ) {

    mqttclient.setServer(mqtt_host, (int)port);
       
}

void JMRI_MQTT::topicupdate(const char * topic ) {

    mqttclient.subscribe(topic);

}
void  JMRI_MQTT::loop(){

    mqttclient.loop();
  
}
void  JMRI_MQTT::disconnect(){

    mqttclient.disconnect();
  
}
bool  JMRI_MQTT::connected(){

    return mqttclient.connected();
  
}

//create a message to send to broker/JMRI
void JMRI_MQTT::sendMessage(Messg messg){
  
        if ( mqttclient.state() != 0 ) {
             JMRI_HELPER::logging(0,F("Problem trying to send MQTT message!\n"));
             return;
          }
            
          if (messg.messtate == 0){
            return;
          }
          
          char topic[30];
          char report[10];

          switch (messg.messtate)
          {
            
          case 'T':
                    
              sprintf(topic,"/trains/track/turnout/%d", messg.sysname);
              sprintf(report,"THROWN");
              break;

          case 'C':
          
              sprintf(topic,"/trains/track/turnout/%d", messg.sysname);
              sprintf(report,"CLOSED");
              break;
                       
          case 'N': 

              sprintf(topic,"/trains/track/light/%d", messg.sysname);
              sprintf(report,"ON");
              break;

          case 'F':

              sprintf(topic,"/trains/track/light/%d", messg.sysname);
              sprintf(report,"OFF");
              break;
              
          case 'A': 
             
              sprintf(topic,"/trains/track/sensor/%d", messg.sysname);
              sprintf(report,"ACTIVE");
              break;
              
          case 'I':
              sprintf(topic,"/trains/track/sensor/%d", messg.sysname);
              sprintf(report,"INACTIVE");
              break;

          default:
              return;
         }
          
          mqttclient.publish(topic,report);
          JMRI_HELPER::logging(1,F("Message sent on topic: %s. Message: %s\n"), topic, report);
                     
}  
//void  JMRI_MQTT::sendMessage(ushort sysname, char* messtate){
//
//          if ( mqttclient.state() != 0 ) {
//             JMRI_HELPER::logging(0,F("Problem trying to send MQTT message!\n"));
//             return;
//          }
//            
//          if (sysname == 0){
//            return;
//          }
//          
//          char topic[30];
//          char report[10];
//        
//          if (*messtate == 'T' || *messtate == 'C'){
//            
//              sprintf(topic,"/trains/track/turnout/%d", sysname);
//              sprintf(report,"THROWN");
//              
//              if (*messtate == 'C'){
//                  sprintf(report,"CLOSED");
//              }
//              
//          } else if (*messtate == 'N' || *messtate == 'F'){
//
//              sprintf(topic,"/trains/track/light/%d", sysname);
//              sprintf(report,"ON");
//              
//              if (*messtate == 'F'){
//                  sprintf(report,"OFF");
//              }
//              
//          } else if (*messtate == 'A' || *messtate == 'I'){
//             
//              sprintf(topic,"/trains/track/sensor/%d", sysname);
//              sprintf(report,"ACTIVE");
//              
//              if (*messtate == 'I'){
//                  sprintf(report,"INACTIVE");
//              }
//              
//          }
//
//          mqttclient.publish(topic,report);
//          JMRI_HELPER::logging(1,F("Message sent on topic: %s. Message: %s\n"), topic, report);
//          //JMRI_HELPER::sendConsole(mess);
//                     
//}
