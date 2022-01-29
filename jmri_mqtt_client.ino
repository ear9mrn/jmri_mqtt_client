
#include  "config.h"
#include  "jmri_store.h"
#include  "jmri_wifi.h"
#include  "jmri_web.h"                 
#include  "i2c.h" 
#include  "mqtt.h"
#include  "helper.h"
#include  "htmltemplate.h"
#include  "jmri_telnet.h"
#include  "jmri_urlUpdate.h"

jmriData jmri_data;  

void setup() {

      Serial.begin(115200);
      while (! Serial); 
      
      //randomize turn on/power draw if multiple devices used. 
      delay(random(1000,4000)); 
      
      //initiate helper functions
      JMRI_HELPER::help_init(&jmri_data);
      
      Serial.print(F("\n   ░█ ░█▀▄▀█ ░█▀▀█ ▀█▀\n ▄ ░█ ░█░█░█ ░█▄▄▀ ░█ \n░█▄▄█ ░█  ░█ ░█ ░█ ▄█▄\n\n"));
      Serial.println(F("JMRI ESP8266 accessory client."));
      JMRI_HELPER::logging(2, "Initial heap size: %d\n",ESP.getFreeHeap()); 

      //initiate/read eeprom
      JMRI_STORE::store_init(&jmri_data); 

      //initiate helper functions
      //JMRI_HELPER::help_init(&jmri_data);

      //check for any i2c expansion boards
      JMRI_I2C::i2c_init(&jmri_data);

      //create html templates and save to SPIFFS(LittleFS) (if required)
      JMRI_HTML::html_init();
            
      //start client config web server
      JMRI_WEB::web_init(&jmri_data);   
      
      //connect to wifi
      JMRI_WIFI::wifi_init(&jmri_data);
           
      //connect to mqtt broker
      JMRI_MQTT::mqtt_init(&jmri_data);
      
      //Setup telnet server
      JMRI_TEL::tel_init(&jmri_data);

      jmri_data.bootComplete = true;
      JMRI_HELPER::logging(2,"End of setup heap size: %d\n",ESP.getFreeHeap());

}


void loop() {


      //update eeprom if required
      if (jmri_data.eepromUpdate) JMRI_STORE::store_init(&jmri_data);

      //update i2c connections if required
      if ( jmri_data.i2cUpdate ) JMRI_I2C::i2c_init(&jmri_data);
  
      //check wifi is still connected.
      if (jmri_data.wifi_client.status() != WL_CONNECTED) JMRI_WIFI::wifi_init(&jmri_data);  
    
      //check we are still connected to MQTT broker
      if (!jmri_data.client.connected()) JMRI_MQTT::reconnect();

      //MQTT loop for messages
      jmri_data.client.loop(); 

      //check for any sensor changes on PCF857x boards
      if (jmri_data.pcfChange ) JMRI_I2C::PCF_Change();

      //Check if new telnet client has connected
      JMRI_TEL::clientConnected();

       if(jmri_data.shouldReboot){
          Serial.println("Rebooting...");
          delay(100);
          ESP.restart();
      }
      if (jmri_data.urlUpdate) urlupdate();
  
      delay(10);
       
}
