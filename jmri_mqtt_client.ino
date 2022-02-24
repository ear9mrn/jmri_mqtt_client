

#include  "headers.h"
jmriData jmri_data;  

void setup() {

      Serial.begin(115200);
      while (! Serial); 

      //randomize turn on/power draw if multiple devices used. 
      delay(random(1000,4000)); 
      
      Serial.print(F("\n   ░█ ░█▀▄▀█ ░█▀▀█ ▀█▀\n ▄ ░█ ░█░█░█ ░█▄▄▀ ░█ \n░█▄▄█ ░█  ░█ ░█ ░█ ▄█▄\n\n"));
      Serial.println(F("JMRI ESP8266 accessory client."));
      JMRI_HELPER::logging(2,F("Initial heap size: %d\n"),ESP.getFreeHeap()); 

      //initiate/read eeprom
      JMRI_STORE::store_init(); 

      //check for any i2c expansion boards
      JMRI_I2C::i2c_init();

      //create html templates and save to SPIFFS(LittleFS) (if required)
      JMRI_HTML::html_init();
            
      //start client config web server
      JMRI_WEB::web_init();   
      
      //connect to wifi
      WifiInterface::wifi_setup();
            
      jmri_data.jrmi_mqtt_v_latest = WifiInterface::getLatestVer();
            
      //connect to mqtt broker
      JMRI_MQTT::mqtt_init();
      
      //Setup telnet server
      jmri_telnet::tel_init();

      jmri_data.bootComplete = true;
      JMRI_HELPER::logging(2,F("End of setup heap size: %d\n"),ESP.getFreeHeap());

}


void loop() {


      //update eeprom if required
      if (JMRI_STORE::eepromUpdate()) JMRI_STORE::store_init();

      //update i2c connections if required
      if ( JMRI_I2C::i2cUpdate() ) JMRI_I2C::i2c_init();
  
      //check wifi is still connected. 
      if (WiFi.status() != WL_CONNECTED && WiFi.softAPIP() != IPAddress(192, 168, 4, 1)) {
            WifiInterface::wifi_setup();  
      }
    
      //check we are still connected to MQTT broker
      if (!JMRI_MQTT::connected()) JMRI_MQTT::mqtt_init();

      //MQTT loop for messages
      JMRI_MQTT::loop();

      //Check if new telnet client has connected
      jmri_telnet::clientConnected();

      if(JMRI_WEB::shouldReboot()){
          JMRI_HELPER::logging(0,F("Rebooting...\n"));
          delay(100);
          ESP.restart();
      }

      //run next job on queue
      JMRI_HELPER::pollQueue();
      
      if (jmri_data.urlUpdate > 0.0) urlupdate();  
       
}
