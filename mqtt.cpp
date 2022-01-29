
#include  "mqtt.h"
#include  "helper.h"
#include  "jmri_wifi.h"
#include  "i2c.h"
#include  "jmri_telnet.h"

WiFiClient mqtt_client;
PubSubClient client(mqtt_client);

//initiate MQTT client. Connect and subscribe
void JMRI_MQTT::mqtt_init(jmriData *jmri_data){

        //Pointer
        jmri_mqtt=(JMRI_MQTT *)calloc(1,sizeof(JMRI_MQTT));
        jmri_mqtt->jmri_ptr = jmri_data;
        
        if (jmri_data->wifi_client.status() != WL_CONNECTED || jmri_data->ssidupdate == true ) { 
            return;
        }

        JMRI_HELPER::logging(1,"MQTT broker IP address: %s\n", jmri_data->data.mqtt_server_host );  
        JMRI_HELPER::logging(1,"MQTT broker port: %d\n\n",     jmri_data->data.mqtt_server_port );

        //connect and set callback
        client.setServer(jmri_data->data.mqtt_server_host, (int)jmri_data->data.mqtt_server_port);
        client.connect("ESPClient");
        jmri_data->retainMSGTimer = millis();
        client.setCallback(JMRI_MQTT::callback);
        jmri_data->client = client;

        //subscribe to a topic
        // default: /trains/track/#
        if (jmri_data->client.state() == 0 ) {
              jmri_data->client.subscribe(jmri_data->data.mqtt_topic);
              JMRI_HELPER::logging(1,"Subscribed to: %s\n", jmri_data->data.mqtt_topic);
        }
           
}

//disconnect if required (wifi drop!).
void JMRI_MQTT::disconnect_mqtt(){

    if ( jmri_mqtt->jmri_ptr->client.state() >= 0 ) {
        jmri_mqtt->jmri_ptr->client.disconnect();  
        JMRI_HELPER::logging(1,"Disconnected from MQTT broker...\n");
    }
    
}

//callback i.e. messages received
void JMRI_MQTT::callback(char* topic, byte* message, unsigned int length) {

          //work around to ignore retained messaged when first connecting to broker!
          if (millis() - jmri_mqtt->jmri_ptr->retainMSGTimer < 2000){
            return;
          }

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
          JMRI_HELPER::logging(1,"Message arrived on topic: /%s/%s/%s/%s. Message: %s.\n",T[0],T[1],T[2],T[3], cmessage); 

          //call helper to deal with action required for messages received
          if ( strcmp(T[2], "turnout") == 0 ){
          
                  JMRI_HELPER::setTurnout( atoi(T[3]), cmessage);                
            
          } else if ( strcmp(T[2], "light") == 0 ){

                  JMRI_HELPER::setLight( atoi(T[3]), cmessage);
                             
          }
                    
}

// helper to update host ip
void JMRI_MQTT::hostIPupdate() {

    jmri_mqtt->jmri_ptr->client.setServer(jmri_mqtt->jmri_ptr->data.mqtt_server_host, jmri_mqtt->jmri_ptr->data.mqtt_server_port);
    
}

//reconnect if connection is lost...
void JMRI_MQTT::reconnect() {

        if (jmri_mqtt->jmri_ptr->wifi_client.status() != WL_CONNECTED || jmri_mqtt->jmri_ptr->ssidupdate == true ) {
          return;
        }

        JMRI_HELPER::logging(1,"Not connected to MQTT broker: %s on port: %d\n",jmri_mqtt->jmri_ptr->data.mqtt_server_host, jmri_mqtt->jmri_ptr->data.mqtt_server_port);       
            
        // Loop until we're reconnected
        while (!jmri_mqtt->jmri_ptr->client.connected()) {
               
              JMRI_HELPER::logging(1,"Attempting MQTT connection to MQTT broker: %s on port: %d\n", jmri_mqtt->jmri_ptr->data.mqtt_server_host, jmri_mqtt->jmri_ptr->data.mqtt_server_port);
              
              // check we have wifi connection
              if (jmri_mqtt->jmri_ptr->wifi_client.status() != WL_CONNECTED){
                    JMRI_WIFI::wifi_init(jmri_mqtt->jmri_ptr);  
              }
              
              JMRI_MQTT::mqtt_init(jmri_mqtt->jmri_ptr);
               
              // Wait 5 seconds before retrying
              delay(5000);
        }

}

//helper. create a message to send to broker/JMRI
void  JMRI_MQTT::sendMessage(ushort sysname, char* messtate){

          if (sysname == 0){
            return;
          }
          
          char topic[30];
          char report[10];
        
          if (*messtate == 'T' || *messtate == 'C'){
            
              sprintf(topic,"/trains/track/turnout/%d", sysname);
              sprintf(report,"THROWN");
              
              if (*messtate == 'C'){
                  sprintf(report,"CLOSED");
              }
              
          } else if (*messtate == 'N' || *messtate == 'F'){

              sprintf(topic,"/trains/track/light/%d", sysname);
              sprintf(report,"ON");
              
              if (*messtate == 'F'){
                  sprintf(report,"OFF");
              }
              
          } else if (*messtate == 'A' || *messtate == 'I'){
             
              sprintf(topic,"/trains/track/sensor/%d", sysname);
              sprintf(report,"ACTIVE");
              
              if (*messtate == 'I'){
                  sprintf(report,"INACTIVE");
              }
              
          }

          jmri_mqtt->jmri_ptr->client.publish(topic,report);
          //char mess[100];
          //sprintf(mess, "Message sent on topic: %s. Message: %s\n", topic, report); 
          JMRI_HELPER::logging(1,"Message sent on topic: %s. Message: %s\n", topic, report);
          //JMRI_HELPER::sendConsole(mess);
                     
}

int JMRI_MQTT::pointer(){
      return(mqttAddress);
}

JMRI_MQTT *JMRI_MQTT::jmri_mqtt=NULL;
int JMRI_MQTT::mqttAddress=0;
