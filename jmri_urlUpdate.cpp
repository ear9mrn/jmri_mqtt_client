
#include "jmri_urlUpdate.h"

uint8_t nextval = 0;
        
void urlupdate(jmriData *jmri_data){

    if  ( WiFi.status() == WL_CONNECTED ) {

        nextval = 0;
        JMRI_HELPER::logging(1,F("Heap size: %u\n"), ESP.getFreeHeap());
        
        BearSSL::WiFiClientSecure wificlientssl;
        bool mfln = wificlientssl.probeMaxFragmentLength("github.com", 443, 512);  
        JMRI_HELPER::logging(1,F("MFLN supported: %s\n"), mfln ? "yes" : "no");

        if (mfln) {
              wificlientssl.setBufferSizes(512, 512);
        } else {
           JMRI_HELPER::logging(0,F("Unable to update ESP8266 due to lack of MFLN support.\n"));
           JMRI_HELPER::logging(0,F("Try downloading the latest firmware version from github and updating manually.\n"));
           JMRI_HELPER::logging(0,F("The latest version can be found here: \"https://github.com/ear9mrn/jmri_mqtt_client/releases\".\n"));
           jmri_data->urlUpdate = 0.0;
           return;
        }

        ESPhttpUpdate.onProgress(update_progress);
        ESPhttpUpdate.onStart(update_started);
        ESPhttpUpdate.onError(update_error);
        ESPhttpUpdate.onEnd(update_finished);
        
        wificlientssl.setInsecure();
        ESPhttpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        char host[150];
        sprintf(host, "https://github.com/ear9mrn/jmri_mqtt_client/releases/download/%04.2f/jmri_mqtt_client.ino.nodemcu_%04.2f.bin",jmri_data->urlUpdate,jmri_data->urlUpdate);
        JMRI_HELPER::logging(1,F("Attempting to download and update ESP2866...\n"));
        JMRI_HELPER::logging(1,F("Source file URL: %s\n\n"),host);        
        
        t_httpUpdate_return ret = ESPhttpUpdate.update(wificlientssl, host );
      
        switch (ret) {
          case HTTP_UPDATE_FAILED:
            JMRI_HELPER::logging(0,F("HTTP_UPDATE_FAILED Error (%d): %s\n"), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;
    
          case HTTP_UPDATE_NO_UPDATES:
            JMRI_HELPER::logging(2,F("HTTP_UPDATE_NO_UPDATES"));
            break;
    
          case HTTP_UPDATE_OK:
            JMRI_HELPER::logging(2,F("HTTP_UPDATE_OK"));
            break;
        }
        
        JMRI_HELPER::logging(1,F("Heap size: %u\n"), ESP.getFreeHeap());
    }
     jmri_data->urlUpdate = 0.0;
}

// need to fix so this only shows incremenatal progress ~10%
void update_progress(unsigned int progress, unsigned int total) {

            if ( ((float)progress / (float)total) * 100 > nextval){
                JMRI_HELPER::logging(1,F("."));
                nextval+=5;
            }
            
};

void update_started() {
      JMRI_HELPER::logging(1,F("HTTP update process started.\n"));
      //nextval=0;
}
    
void update_finished() {
      JMRI_HELPER::logging(1,F("\nHTTP update completed.\n\n"));
      JMRI_HELPER::logging(2,F("Heap size: %u\n"), ESP.getFreeHeap());
}
    
    
void update_error(int err) {
       JMRI_HELPER::logging(1,F("HTTP update fatal error code %d\n"), err);
}
