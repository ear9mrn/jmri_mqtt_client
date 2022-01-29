
#include "jmri_telnet.h"
#include "helper.h"

//this enables you to view MQTT messages (sent and received) over telnet 
//i.e. without need of serial monitor via usb and remotely
//linux command line or windows cmd on same network "telnet <esp1266 ip>"

void JMRI_TEL::tel_init(jmriData *jmri_data) {

      jmri_tel=(JMRI_TEL *)calloc(1,sizeof(JMRI_TEL));
      jmri_tel->jmri_ptr = jmri_data;

      jmri_data->telnetServer = new WiFiServer(23);
      jmri_data->telnetServer->begin();

      jmri_data->telnetUp = true;
      
//      if (jmri_data->telnetServer->operator()) {
//          Serial.println(F("Telnet server up...\n"));
//      }
           
}


//print/send message to the telnet client
void JMRI_TEL::telPrint(char * text) {

  if (!jmri_tel->jmri_ptr->telnetClient)
  {
      jmri_tel->jmri_ptr->telnetClient = jmri_tel->jmri_ptr->telnetServer->available();
      
      if (jmri_tel->jmri_ptr->telnetClient) {
          Serial.printf("Telnet client connected...\n");
          jmri_tel->jmri_ptr->telnetClient.print("JMRI telnet server...\n");
      }
  }
  if (jmri_tel->jmri_ptr->telnetClient){
    jmri_tel->jmri_ptr->telnetClient.print(text);
  } 
}

void JMRI_TEL::clientConnected() {

    if (jmri_tel->jmri_ptr->telnetServer->hasClient()) {

        if (!jmri_tel->jmri_ptr->telnetClient || !jmri_tel->jmri_ptr->telnetClient.connected()) {
              if (jmri_tel->jmri_ptr->telnetClient) {
                  jmri_tel->jmri_ptr->telnetClient.stop();
                  //JMRI_HELPER::logging(1,"Telnet client disconneted...\n");
                  Serial.printf("Telnet client disconneted...\n");
              }
              
              jmri_tel->jmri_ptr->telnetClient = jmri_tel->jmri_ptr->telnetServer->available();
              //JMRI_HELPER::logging(1,"New Telnet client connected...\n");
              Serial.printf("New Telnet client connected...\n");
              jmri_tel->jmri_ptr->telnetClient.print("\n───░█ ░█▀▄▀█ ░█▀▀█ ▀█▀\n─▄─░█ ░█░█░█ ░█▄▄▀ ░█─\n░█▄▄█ ░█──░█ ░█─░█ ▄█▄\n\n");
              jmri_tel->jmri_ptr->telnetClient.print("JMRI MQTT Accessory telnet server...\nWelcome...\n\n");
              jmri_tel->jmri_ptr->telnetClient.flush();  // clear input buffer, else you get strange characters 
            }
      }
}


int JMRI_TEL::pointer(){
      return(telAddress);
}

JMRI_TEL *JMRI_TEL::jmri_tel=NULL;
int JMRI_TEL::telAddress=0;
