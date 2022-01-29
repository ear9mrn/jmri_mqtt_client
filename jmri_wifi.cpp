
#include "jmri_wifi.h"
#include "jmri_web.h" 
#include "jmri_telnet.h"
#include "helper.h"

//initiates wifi setup
void JMRI_WIFI::wifi_init(jmriData *jmri_data){

        jmri_wifi=(JMRI_WIFI *)calloc(1,sizeof(JMRI_WIFI));
        jmri_wifi->jmri_ptr = jmri_data;
        
        //wait here if we are waiting for the user to 
        //provide a new ssid and password
        if (jmri_data->ssidupdate == true){
            delay(1000);
            return;
        }

        //try to connect to wifi
        jmri_data->wifi_client.mode(WIFI_STA);     
        WiFi.begin(jmri_data->data.ssid, jmri_data->data.pass);
        jmri_data->wifi_client = WiFi;
        
        // Wait some time (xx seconds) to connect to wifi
        JMRI_HELPER::logging(1,"Waiting for Wifi to come up...");
        for(int i = 0; i < WIFITIMEOUT && WiFi.status() != WL_CONNECTED; i++) {
            JMRI_HELPER::logging(1,".");
            delay(1000);
        }
          
        //failed. So request a new ssid and password
        if (WiFi.status() != WL_CONNECTED && jmri_data->ssidupdate == false){
            JMRI_HELPER::logging(1,"\nProblem connecting to wifi with existing credentials.\nAssuming a reset is required...\n");
            JMRI_HELPER::logging(1,"SSID: %s Pass: %s\n\n", jmri_data->data.ssid, jmri_data->data.pass );
            jmri_data->ssidupdate = true;
            JMRI_WIFI::scan();
            JMRI_WIFI::setupAP();
            JMRI_WEB::startAPWeb();
            JMRI_TEL::tel_init(jmri_data);
            
        } else {
        
           jmri_data->ssidupdate = false;
           JMRI_HELPER::logging(1,"\nConnected to Wifi: %s\n", jmri_wifi->jmri_ptr->data.ssid);
           IPAddress ip = WiFi.localIP();
           JMRI_HELPER::logging(1,"Client IP: %s\n", ip.toString().c_str());
           JMRI_HELPER::logging(1,"MAC: %s\n\n", WiFi.macAddress().c_str());
               
        }

}
//scan in AP to get list of broadcast SSID
void JMRI_WIFI::scan(){

          char output[500];
          jmri_wifi->jmri_ptr->wifi_client.disconnect();
          delay(100);
          
          int n = jmri_wifi->jmri_ptr->wifi_client.scanNetworks();
          JMRI_HELPER::logging(0,"Scan complete.\n");
          if (n == 0) {
            
            JMRI_HELPER::logging(0,"no networks found...\n");
            
          } else {

              if (n > 20) n=10;
              sprintf(output,"<ol>");
              JMRI_HELPER::logging(0,"%d networks found:\n", n);
              for (int i = 0; i < n; ++i) {
                  // Print SSID and RSSI for each network found   
                  JMRI_HELPER::logging(0,"%d:%s (%d dBm)\n", i+1, jmri_wifi->jmri_ptr->wifi_client.SSID(i), jmri_wifi->jmri_ptr->wifi_client.RSSI(i));  
                             
                  sprintf(output + strlen(output),"<li>%s (%d dBm)</li>\n", jmri_wifi->jmri_ptr->wifi_client.SSID(i).c_str(), jmri_wifi->jmri_ptr->wifi_client.RSSI(i) );
              }
              sprintf(output + strlen(output),"</ol>");                
              sprintf(jmri_wifi->jmri_ptr->wifilist, output);
          }                           
          
}

//set up AP mode in order for user to provide local SSID and password
void JMRI_WIFI::setupAP()
{
          //jmri_data->ssidupdate = true;
          jmri_wifi->jmri_ptr->wifi_client.softAP("JMRI-ACC-" + jmri_wifi->jmri_ptr->wifi_client.macAddress(), "");
          JMRI_HELPER::logging(1,"JMRI Accessory Wifi Credentials Modification");

}


int JMRI_WIFI::pointer(){
  return(wifiAddress);
}

JMRI_WIFI *JMRI_WIFI::jmri_wifi=NULL;
int JMRI_WIFI::wifiAddress=0;
