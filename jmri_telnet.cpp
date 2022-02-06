
#include "jmri_telnet.h"

//this enables you to view MQTT messages (sent and received) over telnet 
//i.e. without need of serial monitor via usb and remotely
//linux command line or windows cmd on same network "telnet <esp1266 ip>"

WiFiClient   jmri_telnet::telnetClient;
WiFiServer  *jmri_telnet::telnetServer;

void jmri_telnet::tel_init() {

      telnetServer = new WiFiServer(23);
      telnetServer->begin();
        
}

//print/send message to the telnet client
void jmri_telnet::telPrint(char * text) {

  if (!telnetClient)
  {
      telnetClient = telnetServer->available();
      
      if (telnetClient) {
          Serial.print(F("Telnet client connected...\n"));
          telnetClient.print("JMRI telnet server...\n");
      }
  }
  if (telnetClient){
     telnetClient.print(text);
  } 
}

void jmri_telnet::clientConnected() {

    if (telnetServer->hasClient()) {

        if (!telnetClient || !telnetClient.connected()) {
              if (telnetClient) {
                  telnetClient.stop();
                  //JMRI_HELPER::logging(1,"Telnet client disconneted...\n");
                  Serial.print(F("Telnet client disconneted...\n"));
              }
              
              telnetClient = telnetServer->available();
              //JMRI_HELPER::logging(1,F("New Telnet client connected...\n"));
              Serial.print(F("New Telnet client connected...\n"));
              telnetClient.print("\n───░█ ░█▀▄▀█ ░█▀▀█ ▀█▀\n─▄─░█ ░█░█░█ ░█▄▄▀ ░█─\n░█▄▄█ ░█──░█ ░█─░█ ▄█▄\n\n");
              telnetClient.print("JMRI MQTT Accessory telnet server...\nWelcome...\n\n");
              telnetClient.flush();  // clear input buffer, else you get strange characters 
            }
      }
}
bool jmri_telnet::telnetUp() {
    return telnetClient;
}
