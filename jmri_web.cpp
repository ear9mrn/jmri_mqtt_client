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
 
#include "jmri_web.h"

AsyncWebServer    JMRI_WEB::server(80);
AsyncEventSource  JMRI_WEB::events("/events");
uint8_t           JMRI_WEB::progress = 0;
int               JMRI_WEB::fsize = 0;
bool              JMRI_WEB::_shouldReboot = false;

void JMRI_WEB::web_init(){
 
    JMRI_HELPER::logging(1,F("Starting JMRI accessory config web server...\n\n"));

    server.addHandler(&events);
         
    //Send web page with input fields to client
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            
            JMRI_HELPER::logging(2,F("Heap size: %d\n"),ESP.getFreeHeap() );
            
            //redirect if we are not connected to wifi and in AP mode
            if (WiFi.status() != WL_CONNECTED){ 

                  request->redirect("/scan");

            //serve index.html from SPIFFS(LittleFS)
            } else {                

                  File file = LittleFS.open("/index.html", "r");
 
                  if (!file) {
                        JMRI_HELPER::logging(1,F("Error: Could not open SPIFFS(LittleFS) file /index.html...\n"));
                        server.onNotFound(notFound);
                  } else {
                    JMRI_HELPER::logging(2,F("HTML file present with size %dbytes...\n"), file.size());
                    file.close();
                  }
                  request->send(LittleFS, "/index.html", "text/html");
            }
            
            JMRI_HELPER::logging(2,F("Heap size: %d\n"),ESP.getFreeHeap() );
            
    });

    //serve css style sheet from SPIFFS(LittleFS)
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
            
            JMRI_HELPER::logging(2,F("Heap size: %d\n"),ESP.getFreeHeap() );       

                  File file = LittleFS.open("/style.css", "r");
 
                  if (!file) {
                        JMRI_HELPER::logging(1,F("Error: Could not open SPIFFS(LittleFS) file /style.css...\n"));
                        server.onNotFound(notFound);
                  } else {
                        JMRI_HELPER::logging(2,F("CSS file present with size %dbytes...\n"), file.size());
                        file.close();
                  }
                  request->send(LittleFS, "/style.css", "text/css");
            
            JMRI_HELPER::logging(2,F("Heap size: %d\n"),ESP.getFreeHeap() );
            
    });

    //serve css style sheet from SPIFFS(LittleFS)
    server.on("/logging.html", HTTP_GET, [](AsyncWebServerRequest *request){
            
            JMRI_HELPER::logging(2,F("Heap size: %d\n"),ESP.getFreeHeap() );       

            File file = LittleFS.open("/logging.html", "r");           
             
            if (!file) {
                  JMRI_HELPER::logging(1,F("Error: Could not open SPIFFS(LittleFS) file /logging.html...\n"));
                  server.onNotFound(notFound);
            } else {
                  JMRI_HELPER::logging(2,F("logging file present with size %dbytes...\n"), file.size());
                  file.close();
                  //AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", logging.html );
                  AsyncWebServerResponse *response = request->beginResponse(LittleFS,"/logging.html", "text/html");
                  response->addHeader("Log-Level",String(jmri_data.data.loglvl));
                  //jmri_data.data.loglvl
                  request->send(response);
            }
            //request->send(LittleFS, "/logging.html", "text/html");
            
            JMRI_HELPER::logging(2,F("Heap size: %d\n"),ESP.getFreeHeap() );
            
    });

    //serve javascript from SPIFFS(LittleFS)
    server.on("/jmri_js.js", HTTP_GET, [](AsyncWebServerRequest *request){
            
            JMRI_HELPER::logging(2,F("Heap size: %d\n"),ESP.getFreeHeap() );       

                  File file = LittleFS.open("/jmri_js.js", "r");
 
                  if (!file) {
                        JMRI_HELPER::logging(1,F("Error: Could not open SPIFFS(LittleFS) file /jmri_js.js...\n"));
                        server.onNotFound(notFound);
                  } else {
                        JMRI_HELPER::logging(2,F("javascript file present with size %dbytes...\n"), file.size());
                        file.close();
                  }
                  request->send(LittleFS, "/jmri_js.js", "text/javascript");
            
            JMRI_HELPER::logging(2,F("Heap size: %d\n"),ESP.getFreeHeap() );
            
    });
    
    //reset eeprom (button)
    server.on("/reset_eeprom", HTTP_GET, [](AsyncWebServerRequest *request){

            JMRI_HELPER::logging(1,F("Reset of EEPROM requested..."));
            JMRI_STORE::configeeprom();
            //JMRI_STORE::set_eepromUpdate(true);
            JMRI_MQTT::disconnect();
            request->send(200, "text/plain", "EEPROM Reset");
            WiFi.disconnect();
                     
    });  

    //i2c rescan (button)
    server.on("/i2cscan", HTTP_GET, [](AsyncWebServerRequest *request){
          
            JMRI_HELPER::logging(1,F("I2C rescan requested...\n"));

            JMRI_I2C::set_i2cUpdate(true); 
            request->redirect("/");             
            
            Event event;  
            sprintf(event.a, "empty" ); 
            sprintf(event.b, "refresh");   
            jmri_data.event.push(event);
                               
    });  

    //called when dot button is clicked (for lights and turnouts
    server.on("/dot_button_click", HTTP_POST, [] (AsyncWebServerRequest *request){

        AsyncWebParameter *s = request->getParam(0);
        AsyncWebParameter *m = request->getParam(1);

        char thepin = (char)m->value()[0];
        //JMRI_MQTT::sendMessage(s->value().toInt(), &thepin);
        Messg messg;
        messg.sysname = s->value().toInt();
        messg.messtate = thepin;
        jmri_data.messg.push(messg); 
              
        request->send(200, "text/plain", "Button Clicked");
                
    });
    
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
          request->redirect("https://www.jmri.org/images/jmri.ico");
    }); 

    //called request for github firmware update. 
    //Put version into variable that thens runs outside asyncwebserver
    server.on("/urlfirmwareUpload", HTTP_POST, [] (AsyncWebServerRequest *request){

        AsyncWebParameter *b = request->getParam(0);       
        JMRI_HELPER::logging(1,F("urlfirmwareUpload %s\n"),b->value().c_str());      
        jmri_data.urlUpdate = b->value().toFloat();
                                  
    });

     server.on("/firmwareUpload", HTTP_POST, [](AsyncWebServerRequest *request){
          _shouldReboot = !Update.hasError();
          AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", _shouldReboot?"OK":"FAIL");
          response->addHeader("Connection", "close");
          request->send(response);
          
    },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){

      if(!index){

            if(request->hasHeader("x-filesize")){
                  AsyncWebHeader* h = request->getHeader("x-filesize");
                  JMRI_HELPER::logging(1,F("Upgrade file size: %s bytes.\n"), h->value().c_str());
                  fsize = h->value().toInt();
                  progress = 0;
            }
                
            JMRI_HELPER::logging(1,F("Update Started: %s\n"), filename.c_str());
            Update.runAsync(true);
            
            if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
                  Update.printError(Serial);
            }
            
      }
      if(index){
            if ( ((float)(index+len)/(float)fsize) * 100 > progress){
                JMRI_HELPER::logging(1,F("."));
                progress+=5;
            }
            //JMRI_HELPER::logging(1,F("Progress: %d, last: %d\n"),index+len, len);
      }
      if(!Update.hasError()){
            if(Update.write(data, len) != len){
              Update.printError(Serial);
            }
      }
      if(final){
            if(Update.end(true)){
              JMRI_HELPER::logging(1,F("\nUpdate Success: %u bytes\n"), index+len);
              _shouldReboot = true;
            } else {
              Update.printError(Serial);
            }
      }
    });

    //Reading/restoring saved settings from file (json format)
    server.on("/restore", HTTP_POST, [](AsyncWebServerRequest *request) {
                  request->redirect("/");
                  }, 
                  [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {         

            
            if(!index){
                    
                    JMRI_HELPER::logging(2,F("Heap size: %u\n"), ESP.getFreeHeap());
                    JMRI_HELPER::logging(1,F("Restoring configuration from file %s\n"), filename.c_str()); 
                    request->_tempFile = LittleFS.open("/upload.json", "w");  
//                    if(request->hasHeader("x-filesize")){
//                          AsyncWebHeader* h = request->getHeader("x-filesize");
//                          Serial.printf("x-filesize: %s\n", h->value().c_str());
//                    }    
            }
            
            if(len) {
                    request->_tempFile.write(data,len);
            }

            if(final){

                    uint16_t n = len+index+1;
                    uint8_t total = 0;
                    
                    char* buff=(char *) malloc(n);

                    request->_tempFile.close();                
                    request->_tempFile = LittleFS.open("/upload.json", "r");
                    request->_tempFile.readBytes(buff, n-1);
                    buff[n-1] = '\0';
                    request->_tempFile.close();
                    
                    char *result;
                    result=(char *) malloc(10);
                    
                    jsonExtract(buff, "number", &result);
                    total = atoi(result);
                    char mess[50];
                    
                    sprintf(mess, "Restoring backup from file. %d Boards...\n", total);
                    //JMRI_HELPER::sendConsole(mess);
                            
                    jsonExtract(buff, "mqtthost", &result);
                    //JMRI_HELPER::logging(2,"result, host: %s\n",result);
                    sprintf( jmri_data.data.mqtt_server_host, result );
                    
                    jsonExtract(buff, "mqttport", &result);          
                    //Serial.printf("\nresult, port: %d\n", atoi(result));
                    jmri_data.data.mqtt_server_port = atoi(result) ;
                                        
                    jsonExtract(buff, "mqtttopic", &result);          
                    //Serial.printf("result, topic: %s\n", result);
                    sprintf( jmri_data.data.mqtt_topic, result );
                                                                                                       
                     for (uint8_t i=0; i<total; i++){   
                              jsonIndexList(buff, "addr", i, &result);          
                              //Serial.printf("\nresult idx: %d, addre: %s\n", i, result); 
                              jmri_data.data.addr[i] = (uint8_t)atoi(result);
                              jmri_data.devdata[i].addr = (uint8_t)atoi(result);
                              
                              jsonIndexList(buff, "bdesc", i, &result); 
                              sprintf(jmri_data.devdata[i].bdesc, result);
                                                                                                                           
                        for (uint8_t j=0; j<I2C_PINS; j++){ 
                                jsonIndexList(buff, "names", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, name: %d\n",i ,j, atoi(result)); 
                                jmri_data.devdata[i].names[j] = (ushort)atoi(result);

                                jsonIndexList(buff, "mode", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, mode: %c\n",i ,j, result[0]); 
                                jmri_data.devdata[i].mode[j] = result[0];

                                jsonIndexList(buff, "state", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, state: %c\n",i ,j, result[0]); 
                                jmri_data.devdata[i].state[j] = result[0];

                                jsonIndexList(buff, "pwm", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, pwm: %s\n",i ,j, result); 
                                if (strcmp(result,"true")==0) {
                                  jmri_data.devdata[i].pwm[j] = true;
                                } else {
                                  jmri_data.devdata[i].pwm[j] = false;
                                }
                                
                                jsonIndexList(buff, "lang", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, lang: %s\n",i ,j, result); 
                                jmri_data.devdata[i].lang[j] = atoi(result);

                                jsonIndexList(buff, "hang", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, hang: %s\n",i ,j, result); 
                                jmri_data.devdata[i].hang[j] = atoi(result);

                                jsonIndexList(buff, "desc", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, hang: %s\n",i ,j, result); 
                                sprintf( jmri_data.devdata[i].desc[j], result );                              
                                
                        }
                    }

                    free(result);
                    free(buff);  
                    JMRI_STORE::save();
                             
                    JMRI_HELPER::logging(2,F("Heap size: %u\n"), ESP.getFreeHeap());   
                    request->redirect("/");
            }    
      
    });

    //main json page. Provide all configuration to populate webpage
    server.on("/json", HTTP_GET, [] (AsyncWebServerRequest *request){

            JMRI_HELPER::logging(2,F("Heap size: %u\n"), ESP.getFreeHeap());
            AsyncResponseStream *response = request->beginResponseStream("application/json");
          
            JMRI_WEB::makejson(response);
            
            JMRI_HELPER::logging(2,F("Heap size: %u\n"), ESP.getFreeHeap());         
            request->send(response);

    });

    //call for json export configuration to file
    server.on("/json_exp", HTTP_GET, [] (AsyncWebServerRequest *request){

            JMRI_HELPER::logging(2,F("Heap size: %u\n"), ESP.getFreeHeap());
            AsyncResponseStream *response = request->beginResponseStream("application/json");
            response->addHeader("Content-Disposition", "attachment; filename=\"" + WiFi.macAddress() +  "_backup.json\"" );
            
            JMRI_WEB::makejson(response);
            
            JMRI_HELPER::logging(2,F("Heap size: %u\n"), ESP.getFreeHeap());       
            request->send(response);

    });

    //change debug/logging level
    server.on("/changeLogradio", HTTP_POST, [] (AsyncWebServerRequest *request){

        AsyncWebParameter *rval = request->getParam(0);
        jmri_data.data.loglvl = (uint8_t)rval->value().toInt();
        JMRI_STORE::saveConfig();
        JMRI_HELPER::logging(1,F("Logging level set to: %d\n"),jmri_data.data.loglvl);                            
        request->send(200, "text/plain", "Radio Button Changed");

    });
    
    //called when change in radio button is made
    server.on("/changeradio", HTTP_POST, [] (AsyncWebServerRequest *request){

        AsyncWebParameter *b = request->getParam(0);
        AsyncWebParameter *p = request->getParam(1);
        AsyncWebParameter *m = request->getParam(2);

        uint8_t board = (uint8_t)b->value().toInt();
        uint8_t thepin = (uint8_t)p->value().toInt();

        request->send(200, "text/plain", "Radio Button Changed");
        JMRI_HELPER::setRadioButton(&board, &thepin, m->value()[0] );                            

    });

    //called when change in PWM box is made
    server.on("/changechkb", HTTP_POST, [] (AsyncWebServerRequest *request){
       
        AsyncWebParameter *b = request->getParam(0);
        AsyncWebParameter *p = request->getParam(1);
        AsyncWebParameter *v = request->getParam(2);
        
        uint8_t board  = (uint8_t)b->value().toInt(); 
        uint8_t thepin = (uint8_t)p->value().toInt();
        bool cvalue;
        if (v->value() == "true"){
           cvalue = true;
        } else {
           cvalue = false;
        }
     
        JMRI_HELPER::setPWMCheck(&board, &thepin, &cvalue );
           
        request->send(200, "text/plain", "PWM Checkbox Changed");

    });

    //called when change in mqtt ip address is sent
    server.on("/changemqttip", HTTP_POST, [] (AsyncWebServerRequest *request){
       
        AsyncWebParameter *b = request->getParam(0);
        
        if (  b->value() == jmri_data.data.mqtt_server_host ){   
          //JMRI_HELPER::logging(1,"New MQTT host IP is the same as the old!");
        } else {
          //JMRI_HELPER::logging(1,"New MQTT host IP updated...");
          JMRI_WEB::update_mqtt_host( const_cast<char*>(b->value().c_str()) );
        }

        JMRI_HELPER::logging(2,F("Update MQTT IP: %s\n"),const_cast<char*>(b->value().c_str())); 
        request->send(200, "text/plain", "MQTT Host IP Changed");

    });

    //called when change in mqtt topic is sent
    server.on("/changemqtttopic", HTTP_POST, [] (AsyncWebServerRequest *request){
       
        AsyncWebParameter *b = request->getParam(0);
        
        if (  b->value() == jmri_data.data.mqtt_topic ){   
          JMRI_HELPER::logging(1,F("New topic same as old!"));
        } else {
          JMRI_HELPER::logging(1,F("New topic different!"));
          JMRI_WEB::update_mqtt_topic( const_cast<char*>(b->value().c_str()) );
        }

        JMRI_HELPER::logging(1,F("Update MQTT Topic: %s\n"),const_cast<char*>(b->value().c_str())); 
        request->send(200, "text/plain", "MQTT Host IP Changed");

    });

    //called when change in mqtt port is sent
    server.on("/changemqttport", HTTP_POST, [] (AsyncWebServerRequest *request){
       
        AsyncWebParameter *b = request->getParam(0);
        ushort newPort = (ushort)b->value().toInt();
        
        if (  newPort == jmri_data.data.mqtt_server_port ){   
          JMRI_HELPER::logging(1,F("New port same as old!"));
        } else {
          JMRI_HELPER::logging(1,F("New port different!"));
          JMRI_WEB::update_mqtt_port( newPort );
        }

        JMRI_HELPER::logging(1,F("Update MQTT port: %d\n"),newPort); 
        request->send(200, "text/plain", "MQTT Port Changed");

    });

    //called when change in any system names is sent
    server.on("/changename", HTTP_POST, [] (AsyncWebServerRequest *request){
       
        AsyncWebParameter *b = request->getParam(0);
        AsyncWebParameter *p = request->getParam(1);
        AsyncWebParameter *v = request->getParam(2);
        
        uint8_t board  = (uint8_t)b->value().toInt(); 
        uint8_t thepin = (uint8_t)p->value().toInt();
        ushort newName = (ushort)v->value().toInt();
        
        if (jmri_data.devdata[board].names[thepin] != newName){       
            jmri_data.devdata[board].names[thepin] = newName;   
            JMRI_STORE::saveBoard(&board); 
        }
        
        JMRI_HELPER::logging(1,F("Updated board: %d, pin %d with name: %d\n"),board, thepin, newName);                      
        request->send(200, "text/plain", "Name Changed...");

    });

    //called when change in any system names is sent
    server.on("/changedesc", HTTP_POST, [] (AsyncWebServerRequest *request){
       
        AsyncWebParameter *b = request->getParam(0);
        AsyncWebParameter *p = request->getParam(1);
        AsyncWebParameter *v = request->getParam(2);
        
        uint8_t board  = (uint8_t)b->value().toInt(); 
        uint8_t thepin = (uint8_t)p->value().toInt();
        
        sprintf(jmri_data.devdata[board].desc[thepin],"%.10s",v->value().c_str());
        JMRI_STORE::saveBoard(&board);
        
        JMRI_HELPER::logging(1,F("Updated board: %d, pin %d with description: %s\n"),board, thepin, v->value().c_str());                     
        request->send(200, "text/plain", "Description Changed...");

    });

     //called when change in any system names is sent
     server.on("/changebdesc", HTTP_POST, [] (AsyncWebServerRequest *request){
       
        AsyncWebParameter *b = request->getParam(0);
        AsyncWebParameter *d = request->getParam(1);
        
        uint8_t board  = (uint8_t)b->value().toInt(); 
        
        sprintf( jmri_data.devdata[board].bdesc, d->value().c_str() );
        JMRI_STORE::saveBoard(&board);
        
        JMRI_HELPER::logging(1,F("Updated board %d: name: %s,\n"),board, d->value().c_str());                     
        request->send(200, "text/plain", "Board name Changed...");

    });

    //called when change in any PWM angle is sent
    server.on("/anglechange", HTTP_POST, [] (AsyncWebServerRequest *request){
       
        AsyncWebParameter *b = request->getParam(0);
        AsyncWebParameter *p = request->getParam(1);
        AsyncWebParameter *v = request->getParam(2);
        AsyncWebParameter *hla = request->getParam(3);
        
        uint8_t board  = (uint8_t)b->value().toInt(); 
        uint8_t thepin = (uint8_t)p->value().toInt();
        uint8_t newAngle = (uint8_t)v->value().toInt();
        
        if ( hla->value() == "la"  ){
          jmri_data.devdata[board].lang[thepin] = newAngle;
          if (jmri_data.devdata[board].state[thepin] == 'C' && board == 0){
              jmri_data.servos[thepin].write(newAngle);
          }
        } else {
          jmri_data.devdata[board].hang[thepin] = newAngle;
          if (jmri_data.devdata[board].state[thepin] == 'T' && board == 0){
              jmri_data.servos[thepin].write(newAngle);
          }
        }

        JMRI_HELPER::logging(1,F("Updated board: %d, pin %d with name: %d\n"),board, thepin, newAngle);
        JMRI_STORE::saveBoard(&board);        
        request->send(200, "text/plain", "Created");

    });

    
    //When in AP mode page is served to provide method to enter local SSID and password
    server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){

              AsyncResponseStream *response = request->beginResponseStream("text/html"); 
              int n = WiFi.scanComplete();                  
           
              if(n == -2){
                    WiFi.scanNetworks(true);
                    request->send(200, "text/plain", "Problem scanning for WiFi access points...");
              } else if(n){
                    response->printf("%s\n","<!DOCTYPE HTML>\r\n<html>JMRI Accessory WiFi credentials setup.");
                    response->printf("%s<p>\n",WiFi.softAPIP().toString().c_str());
                    for (int i = 0; i < n; ++i){
    
                        if (!WiFi.isHidden(i)){
                            JMRI_HELPER::logging(0,F("%d:%s (%d dBm)\n"), i+1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));                              
                            response->printf("<li>%s (%d dBm)</li>\n",WiFi.SSID(i).c_str(), WiFi.RSSI(i) );
                        }
                    }
                    
                    WiFi.scanDelete();
                    response->printf("</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32>");
                    response->printf("<label>Pass: </label><input name='pass' length=64><br><input type='submit'></form></html>");
                    request->send(response);
              }              

});

    //Called when SSID and password are sent during intial setup in AP mode
    server.on("/setting", [](AsyncWebServerRequest *request) {

            char      qssid[33]; 
            char      qpass[65]; 
            uint16_t  statusCode;
            char      content[70];
            
            if (request->hasParam("ssid")) {
              sprintf(qssid,request->getParam("ssid")->value().c_str());
            }
      
            if (request->hasParam("pass")) {
              sprintf(qpass,request->getParam("pass")->value().c_str());
            }
            
            if (request->getParam("ssid")->value().length() > 0 && request->getParam("pass")->value().length() > 0) {
              
                  JMRI_HELPER::logging(1,F("Saving ssid and password to eeprom....\n"));
                  JMRI_HELPER::logging(1,F("ssid: %s pass: %s\n"), qssid, qpass);
      
                  //JMRI_WIFI::set_ssidupdate(false);
                  
                  sprintf(jmri_data.data.ssid, qssid);
                  sprintf(jmri_data.data.pass, qpass);
                  JMRI_STORE::saveConfig();
                 
                  sprintf(content, "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}");
                  statusCode = 200;
                
            } else {
                  sprintf(content, "{\"Error\":\"Problem sending connecting to ssid. Try again...\"}");
                  statusCode = 404;
                  JMRI_HELPER::logging(2,F("Sending 404\n"));
            }
            
            request->send(statusCode, "text/html", content);
            WiFi.softAPdisconnect (true);  
      
    });

    server.onNotFound(notFound);
    server.begin();
    
}

      
void JMRI_WEB::makejson(AsyncResponseStream *response) {

            uint8_t b=0, p=0;
            
            response->printf("{\"mqtthost\":\"%s\",\"mqttport\":%d,\"mqtttopic\":\"%s\",\"clientip\":\"%s\",\"clientmac\":\"%s\","
                              "\"number\":%d,\"nPCFDev\":%d,\"cver\":%04.2f,\"lver\":%04.2f,",
                              jmri_data.data.mqtt_server_host, jmri_data.data.mqtt_server_port, jmri_data.data.mqtt_topic, 
                              WiFi.localIP().toString().c_str(), WiFi.macAddress().c_str(),jmri_data.nDevices, 
                              jmri_data.nPCFDev, VERSION, jmri_data.jrmi_mqtt_v_latest);

            response->printf("\"addr\":[");

            for (b = 0; b<jmri_data.nDevices-1; b++){
                  response->printf("%d,",(int)jmri_data.devdata[b].addr);
            }
            response->printf("%d]",jmri_data.devdata[jmri_data.nDevices-1].addr);

            response->printf(",\"type\":[");
            for (b = 0; b<jmri_data.nDevices-1; b++){
                  response->printf("%d,",jmri_data.boardinfo[b].type);
            }
            response->printf("%d]",jmri_data.boardinfo[jmri_data.nDevices-1].type);
            
            response->printf(",\"bdesc\":[");
            for (b = 0; b<jmri_data.nDevices-1; b++){
                  response->printf("\"%s\",",jmri_data.devdata[b].bdesc);
            }
            response->printf("\"%s\"]",jmri_data.devdata[jmri_data.nDevices-1].bdesc);            
            
            response->printf(",\"names\":{");
            for (b = 0; b<jmri_data.nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("%d,",jmri_data.devdata[b].names[p]);
                }
                if (b<jmri_data.nDevices-1){
                  response->printf("%d],",jmri_data.devdata[b].names[I2C_PINS-1]);
                } else {
                  response->printf("%d]}",jmri_data.devdata[b].names[I2C_PINS-1]);
                }
             }  
             
            response->printf(",\"mode\":{");

            for (b = 0; b<jmri_data.nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("\"%c\",",jmri_data.devdata[b].mode[p]);
                }
                if (b<jmri_data.nDevices-1){
                  response->printf("\"%c\"],",jmri_data.devdata[b].mode[I2C_PINS-1]);
                } else {
                  response->printf("\"%c\"]}",jmri_data.devdata[b].mode[I2C_PINS-1]);
                }
            }
            
            response->printf(",\"state\":{");

            for (b = 0; b<jmri_data.nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("\"%c\",",jmri_data.devdata[b].state[p]);
                }
                if (b<jmri_data.nDevices-1){
                  response->printf("\"%c\"],",jmri_data.devdata[b].state[I2C_PINS-1]);
                } else {
                  response->printf("\"%c\"]}",jmri_data.devdata[b].state[I2C_PINS-1]);
                }
            } 

            response->printf(",\"pwm\":{");

            for (b = 0; b<jmri_data.nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("%s,", jmri_data.devdata[b].pwm[p] ? "true" : "false");
                }
                if (b<jmri_data.nDevices-1){
                  response->printf("%s],",jmri_data.devdata[b].pwm[I2C_PINS-1] ? "true" : "false");
                } else {
                  response->printf("%s",jmri_data.devdata[b].pwm[I2C_PINS-1] ? "true" : "false");
                }
            }

            response->printf("]},\"lang\":{");

             for (b = 0; b<jmri_data.nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("%d,", jmri_data.devdata[b].lang[p]);
                }
                if (b<jmri_data.nDevices-1){
                  response->printf("%d],",jmri_data.devdata[b].lang[I2C_PINS-1]);
                } else {
                  response->printf("%d]}",jmri_data.devdata[b].lang[I2C_PINS-1]);
                }
            }

            response->printf(",\"hang\":{");

            for (b = 0; b<jmri_data.nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("%d,",jmri_data.devdata[b].hang[p]);
                }
                if (b<jmri_data.nDevices-1){
                  response->printf("%d],",jmri_data.devdata[b].hang[I2C_PINS-1]);
                } else {
                  response->printf("%d]}",jmri_data.devdata[b].hang[I2C_PINS-1]);
                }
            }

            response->printf(",\"desc\":{");

            for (b = 0; b<jmri_data.nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("\"%s\",", jmri_data.devdata[b].desc[p]);
                }
                if (b<jmri_data.nDevices-1){
                  response->printf("\"%s\"],",jmri_data.devdata[b].desc[I2C_PINS-1]);
                } else {
                  response->printf("\"%s\"]}",jmri_data.devdata[b].desc[I2C_PINS-1]);
                }
            }

            response->printf("}");
            
}

// for when url is entered that does not exists.
void JMRI_WEB::notFound(AsyncWebServerRequest *request) {
  
       if (request->method() == HTTP_OPTIONS) {
               request->send(200);
       } else {
               request->send(404);
       }
}

void JMRI_WEB::send(char* buf, const char* target, unsigned long the_time ){
      events.send(buf, target, the_time);
}

//helper for updating the mqtt host
void JMRI_WEB::update_mqtt_host(char* mqtt_host){
        
        sprintf(jmri_data.data.mqtt_server_host, mqtt_host);  
        JMRI_STORE::saveConfig();    

        JMRI_MQTT::disconnect_mqtt();
        JMRI_MQTT::hostIPupdate(mqtt_host, jmri_data.data.mqtt_server_port);

}

//helper for updating the mqtt port
void JMRI_WEB::update_mqtt_port(ushort mqtt_port){
        
        jmri_data.data.mqtt_server_port = mqtt_port;
        JMRI_STORE::saveConfig();

        JMRI_MQTT::disconnect_mqtt();
        JMRI_MQTT::hostIPupdate(jmri_data.data.mqtt_server_host, mqtt_port);
  
}

//helper for updating the mqtt topic
void JMRI_WEB::update_mqtt_topic(char* mqtt_topic){

        sprintf(jmri_data.data.mqtt_topic, mqtt_topic);
        JMRI_STORE::saveConfig();

        JMRI_MQTT::disconnect_mqtt();
        JMRI_MQTT::topicupdate(mqtt_topic);
  
}

bool JMRI_WEB::shouldReboot(){
  return _shouldReboot;
}
