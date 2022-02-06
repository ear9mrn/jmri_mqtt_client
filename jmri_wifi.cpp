
#include "jmri_wifi.h"
WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;
float  WifiInterface::_jrmi_mqtt_v_latest = 99;

//initiates wifi setup
void WifiInterface::wifi_setup(const char* ssid, const char * pass) {

        WiFi.disconnect();

        //try to connect to wifi
        WiFi.mode(WIFI_STA); 
        WiFi.begin(ssid, pass);   

        gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP& event) {
            UNUSED(event);
            WiFi.setAutoReconnect(true);
            WiFi.persistent(true);
            JMRI_HELPER::logging(1,F("\nConnected to Wifi: %s\n"), WiFi.SSID());
            JMRI_HELPER::logging(1,F("Client IP: %s\n"), WiFi.localIP().toString().c_str());
            JMRI_HELPER::logging(1,F("MAC: %s\n\n"), WiFi.macAddress().c_str());
        });

        disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event){
            UNUSED(event);
            JMRI_HELPER::logging(1,F("\nWiFi lost connection"));
        });
        
        // Wait some time (xx seconds) to connect to wifi
        JMRI_HELPER::logging(1,F("Waiting for Wifi to come up..."));
        for(int i = 0; i < WIFITIMEOUT && WiFi.status() != WL_CONNECTED; i++) {
            JMRI_HELPER::logging(1,F("."));
            delay(800);
        }
          
        //failed. So request a new ssid and password
        if (WiFi.status() != WL_CONNECTED ) {//&& _ssidupdate == false){
            JMRI_HELPER::logging(1,F("\nProblem connecting to wifi with existing credentials.\nAssuming a reset is required...\n"));
            JMRI_HELPER::logging(1,F("SSID: %s Pass: %s\n\n"), ssid, pass );
            scan();
            
        } 
               
        JMRI_HELPER::logging(1,F("Heap size: %u\n"), ESP.getFreeHeap());  
            
}
//scan in AP to get list of broadcast SSID
//set up AP mode in order for user to provide local SSID and password
void WifiInterface::scan(){

          WiFi.disconnect();
          delay(100);
          WiFi.scanNetworks();
          JMRI_HELPER::logging(0,F("Scan completed.\n"));
          WiFi.softAP("JMRI-ACC-" + WiFi.macAddress(), "");
          JMRI_HELPER::logging(1,F("JMRI Accessory Wifi Credentials Modification"));
          JMRI_HELPER::logging(0,F("\nLocal access point running...\n"));
          JMRI_HELPER::logging(0,F("Connect to AP: JMRI-ACC-%s\n"), WiFi.macAddress().c_str() );
          JMRI_HELPER::logging(0,F("Then navigate to this IP: %s, using a browser.\n"), WiFi.softAPIP().toString().c_str());
                                
}

float WifiInterface::getLatestVer(){

      HTTPClient  _httpclient;
      BearSSL::WiFiClientSecure _wificlientssl;
      
      _wificlientssl.setInsecure();
      bool mfln = _wificlientssl.probeMaxFragmentLength("github.com", 443, 512);

      if (mfln){
            JMRI_HELPER::logging(2,F("MFLN supported\n"));
            _wificlientssl.setBufferSizes(512, 512);
      } else {
            JMRI_HELPER::logging(2,F("MFLN NOT supported\n"));
      }

  
      if (_httpclient.begin(_wificlientssl, "https://github.com/ear9mrn/jmri_mqtt_client/releases/latest")) {  

        uint16_t httpCode = _httpclient.GET();
  
        if (httpCode > 0) {
              // HTTP header has been send and Server response header has been handled
               JMRI_HELPER::logging(2,F("[HTTPS] GET... code: %d\n"), httpCode);
      
              // file found at server
              if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY || httpCode == 302) {
                    String payload = _httpclient.getString();
                    uint8_t pos1 = payload.indexOf("tag/");
                    pos1+=4;
                    uint8_t pos2 = payload.substring(pos1).indexOf("\"");
                    String payloadV = payload.substring(pos1,pos1+pos2);
                    _jrmi_mqtt_v_latest = payloadV.toFloat();
                    JMRI_HELPER::logging(2,F("Payload: %s\n"),payload.c_str()); 
                    JMRI_HELPER::logging(1,F("Latest version %04.2f\n"),_jrmi_mqtt_v_latest);
              }
        } else {
          
            JMRI_HELPER::logging(0,F("https: gitub.com request failed, error: %s\n"), _httpclient.errorToString(httpCode).c_str());
        }
        
        _httpclient.end();
        JMRI_HELPER::logging(2,F("Memory free : %d\n"), ESP.getFreeHeap());
      } else {
            JMRI_HELPER::logging(0,F("https: Unable to connect to github.com...\n"));
      } 
      JMRI_HELPER::logging(2,F("Heap size: %u\n"), ESP.getFreeHeap()); 
      
      return _jrmi_mqtt_v_latest;
}
