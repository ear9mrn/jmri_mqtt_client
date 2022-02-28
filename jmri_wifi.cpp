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
 
#include "jmri_wifi.h"
WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;
float  WifiInterface::_jrmi_mqtt_v_latest = 99;

//initiates wifi setup
void WifiInterface::wifi_setup() {

        WiFi.disconnect();

        //try to connect to wifi
        WiFi.mode(WIFI_STA); 
        WiFi.begin(jmri_data.data.ssid, jmri_data.data.pass);   

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
            JMRI_HELPER::logging(1,F("\nWiFi lost connection, trying to reconnect."));
        });
        
        // Wait some time (xx seconds) to connect to wifi
        JMRI_HELPER::logging(1,F("Waiting for Wifi to come up..."));
        for(int i = 0; i < WIFITIMEOUT && WiFi.status() != WL_CONNECTED; i++) {
            JMRI_HELPER::logging(1,F("."));
            delay(800);
        } 
	
	if (WiFi.status() != WL_CONNECTED && 
	   strcmp(DEFAULTSSID, jmri_data.data.ssid) != 0 && 
	   strcmp(DEFAULTPASSWORD, jmri_data.data.pass) != 0 ) {

	   JMRI_HELPER::logging(1,F("\nProblem connecting to WiFi with saved credentials, trying user defined!\n"));
     JMRI_HELPER::logging(1,F("Saved SSID: %s, saved Pass: %s\n\n"), jmri_data.data.ssid, jmri_data.data.pass );
	   WiFi.begin(DEFAULTSSID, DEFAULTPASSWORD);
     JMRI_HELPER::logging(1,F("Waiting for Wifi to come up..."));
        for(int i = 0; i < WIFITIMEOUT && WiFi.status() != WL_CONNECTED; i++) {
            JMRI_HELPER::logging(1,F("."));
            delay(800);
        }
     if (WiFi.status() == WL_CONNECTED ) {
        JMRI_HELPER::logging(1,F("Saveing credentials...\n"));
        sprintf(jmri_data.data.ssid, DEFAULTSSID);
        sprintf(jmri_data.data.pass, DEFAULTPASSWORD);    
        JMRI_STORE::saveConfig();
     }
 
	}
          
        //failed. So request a new ssid and password
        if (WiFi.status() != WL_CONNECTED ) {
            JMRI_HELPER::logging(1,F("\nProblem connecting to wifi with existing credentials.\nAssuming a reset is required...\n"));
            JMRI_HELPER::logging(1,F("Saved SSID: %s Pass: %s\n\n"), jmri_data.data.ssid, jmri_data.data.pass );
            JMRI_HELPER::logging(1,F("User defined (credentials.h) SSID: %s Pass: %s\n\n"), DEFAULTSSID, DEFAULTPASSWORD);
            scan();
            
        } 
               
        JMRI_HELPER::logging(2,F("Heap size: %u\n"), ESP.getFreeHeap());  
            
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
