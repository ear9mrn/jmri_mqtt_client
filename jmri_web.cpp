
#include "jmri_web.h"
#include "jmri_store.h"
#include "mqtt.h"
#include "i2c.h"
#include "jmri_wifi.h"
#include "helper.h"
#include "jmri_json.h"
#include "jmri_urlUpdate.h"

AsyncWebServer server(80);
AsyncEventSource events("/events");

void JMRI_WEB::web_init(jmriData *jmri_data){

    //setup pointer
    jmri_web=(JMRI_WEB *)calloc(1,sizeof(JMRI_WEB));   
    JMRI_HELPER::logging(1,"Starting JMRI accessory config web server...\n\n");

    jmri_web->jmri_ptr = jmri_data;
    server.addHandler(&events);
      
    jmri_data->events = &events;
    jmri_data->server = &server;
    LittleFS.begin();

    //ESPhttpUpdate.onStart(JMRI_WEB::update_started);
    //ESPhttpUpdate.onEnd(JMRI_WEB::update_finished);
    //ESPhttpUpdate.onProgress(JMRI_WEB::update_progress);
    //ESPhttpUpdate.onError(JMRI_WEB::update_error);

    //for local testing...
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "http://nevill.uk.net");
    
    //Send web page with input fields to client
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            
            JMRI_HELPER::logging(2,"Heap size: %d\n",ESP.getFreeHeap() );
            
            //redirect if we are not connected to wifi and in AP mode
            if (jmri_web->jmri_ptr->ssidupdate){
                  request->redirect("/update");

            //serve index.html from SPIFFS(LittleFS)
            } else {                

                  File file = LittleFS.open("/index.html", "r");
 
                  if (!file) {
                        JMRI_HELPER::logging(1,"Error: Could not open SPIFFS(LittleFS) file /index.html...\n");
                        server.onNotFound(notFound);
                  } else {
                    JMRI_HELPER::logging(2,"HTML file present with size %dbytes...\n", file.size());
                    file.close();
                  }
                  request->send(LittleFS, "/index.html", "text/html");
            }
            
            JMRI_HELPER::logging(2,"Heap size: %d\n",ESP.getFreeHeap() );
            
    });

    //serve css style sheet from SPIFFS(LittleFS)
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
            
            JMRI_HELPER::logging(2,"Heap size: %d\n",ESP.getFreeHeap() );       

                  File file = LittleFS.open("/style.css", "r");
 
                  if (!file) {
                        JMRI_HELPER::logging(1,"Error: Could not open SPIFFS(LittleFS) file /style.css...\n");
                        server.onNotFound(notFound);
                  } else {
                        JMRI_HELPER::logging(2,"CSS file present with size %dbytes...\n", file.size());
                        file.close();
                  }
                  request->send(LittleFS, "/style.css", "text/css");
            
            JMRI_HELPER::logging(2,"Heap size: %d\n",ESP.getFreeHeap() );
            
    });

    //serve css style sheet from SPIFFS(LittleFS)
    server.on("/logging.html", HTTP_GET, [](AsyncWebServerRequest *request){
            
            JMRI_HELPER::logging(2,"Heap size: %d\n",ESP.getFreeHeap() );       

                  File file = LittleFS.open("/logging.html", "r");
 
                  if (!file) {
                        JMRI_HELPER::logging(1,"Error: Could not open SPIFFS(LittleFS) file /logging.html...\n");
                        server.onNotFound(notFound);
                  } else {
                        JMRI_HELPER::logging(2,"logging file present with size %dbytes...\n", file.size());
                        file.close();
                  }
                  request->send(LittleFS, "/logging.html", "text/html");
            
            JMRI_HELPER::logging(2,"Heap size: %d\n",ESP.getFreeHeap() );
            
    });

    //serve javascript from SPIFFS(LittleFS)
    server.on("/jmri_js.js", HTTP_GET, [](AsyncWebServerRequest *request){
            
            JMRI_HELPER::logging(2,"Heap size: %d\n",ESP.getFreeHeap() );       

                  File file = LittleFS.open("/jmri_js.js", "r");
 
                  if (!file) {
                        JMRI_HELPER::logging(1,"Error: Could not open SPIFFS(LittleFS) file /jmri_js.js...\n");
                        server.onNotFound(notFound);
                  } else {
                        JMRI_HELPER::logging(2,"javascript file present with size %dbytes...\n", file.size());
                        file.close();
                  }
                  request->send(LittleFS, "/jmri_js.js", "text/javascript");
            
            JMRI_HELPER::logging(2,"Heap size: %d\n",ESP.getFreeHeap() );
            
    });
    
    //reset eeprom (button)
    server.on("/reset_eeprom", HTTP_GET, [](AsyncWebServerRequest *request){

            JMRI_HELPER::logging(1,"Reset of EEPROM requested...");
            JMRI_STORE::configeeprom();
            jmri_web->jmri_ptr->eepromUpdate = true;
            jmri_web->jmri_ptr->client.disconnect ();
            jmri_web->jmri_ptr->wifi_client.disconnect();
            request->send(200, "text/plain", "EEPROM Reset");
                     
    });  

    //i2c rescan (button)
    server.on("/i2cscan", HTTP_GET, [](AsyncWebServerRequest *request){
          
            JMRI_HELPER::logging(1,"I2C rescan requested...\n");

            jmri_web->jmri_ptr->i2cUpdate = true; 
            //request->send(201, "text/plain", "Button Clicked");
            request->redirect("/");             
                               
    });  

    //called when dot button is clicked (for lights and turnouts
    server.on("/dot_button_click", HTTP_POST, [] (AsyncWebServerRequest *request){

        AsyncWebParameter *s = request->getParam(0);
        AsyncWebParameter *m = request->getParam(1);

        char thepin = (char)m->value()[0];
        JMRI_MQTT::sendMessage(s->value().toInt(), &thepin);
        
        request->send(200, "text/plain", "Button Clicked");
                
    });
    
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
          request->redirect("https://www.jmri.org/images/jmri.ico");
    }); 

    //called when dot button is clicked (for lights and turnouts
    server.on("/urlfirmwareUpload", HTTP_GET, [] (AsyncWebServerRequest *request){

        jmri_web->jmri_ptr->urlUpdate = true;
        
        //WiFiClient downlClient;
        //t_httpUpdate_return ret = ESPhttpUpdate.update(downlClient, "http://nevill.uk.net/inotest/WebUpdatermine.ino.nodemcu.bin");
        //t_httpUpdate_return ret = ESPhttpUpdate.update(downlClient, "http://nevill.uk.net/inotest", 80, "WebUpdatermine.ino.nodemcu.bin");
        
//        switch (ret) {
//        case HTTP_UPDATE_FAILED:
//          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
//          break;
//  
//        case HTTP_UPDATE_NO_UPDATES:
//          Serial.println("HTTP_UPDATE_NO_UPDATES");
//          break;
//  
//        case HTTP_UPDATE_OK:
//          Serial.println("HTTP_UPDATE_OK");
//          break;
//        }
          
        request->send(200, "text/plain", "URL upload");
                
    });
 

    server.on("/firmwareUpload", HTTP_POST, [](AsyncWebServerRequest *request){
          jmri_web->jmri_ptr->shouldReboot = !Update.hasError();
          AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", jmri_web->jmri_ptr->shouldReboot?"OK":"FAIL");
          response->addHeader("Connection", "close");
          request->send(response);
    },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
      if(!index){
            Serial.printf("Update Start: %s\n", filename.c_str());
            Update.runAsync(true);
            if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
                  Update.printError(Serial);
            }
      }
      if(!Update.hasError()){
            if(Update.write(data, len) != len){
              Update.printError(Serial);
            }
      }
      if(final){
            if(Update.end(true)){
              Serial.printf("Update Success: %uB\n", index+len);
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
                    
                    JMRI_HELPER::logging(2,"Heap size: %u\n", ESP.getFreeHeap());
                    JMRI_HELPER::logging(1,"Restoring configuration from file %s\n", filename.c_str()); 
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
                    sprintf( jmri_web->jmri_ptr->data.mqtt_server_host, result );
                    
                    jsonExtract(buff, "mqttport", &result);          
                    //Serial.printf("\nresult, port: %d\n", atoi(result));
                    jmri_web->jmri_ptr->data.mqtt_server_port = atoi(result) ;
                                        
                    jsonExtract(buff, "mqtttopic", &result);          
                    //Serial.printf("result, topic: %s\n", result);
                    sprintf( jmri_web->jmri_ptr->data.mqtt_topic, result );
                                                                                                       
                     for (uint8_t i=0; i<total; i++){   
                              jsonIndexList(buff, "addr", i, &result);          
                              //Serial.printf("\nresult idx: %d, addre: %s\n", i, result); 
                              jmri_web->jmri_ptr->data.i2c_addr[i] = (uint8_t)atoi(result);
                              jmri_web->jmri_ptr->devdata[i].i2c_addr = (uint8_t)atoi(result);
                              
                              jsonIndexList(buff, "bdesc", i, &result); 
                              sprintf(jmri_web->jmri_ptr->devdata[i].bdesc, result);
                                                                                                                           
                        for (uint8_t j=0; j<I2C_PINS; j++){ 
                                jsonIndexList(buff, "names", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, name: %d\n",i ,j, atoi(result)); 
                                jmri_web->jmri_ptr->devdata[i].i2c_names[j] = (ushort)atoi(result);

                                jsonIndexList(buff, "mode", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, mode: %c\n",i ,j, result[0]); 
                                jmri_web->jmri_ptr->devdata[i].i2c_mode[j] = result[0];

                                jsonIndexList(buff, "state", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, state: %c\n",i ,j, result[0]); 
                                jmri_web->jmri_ptr->devdata[i].i2c_state[j] = result[0];

                                jsonIndexList(buff, "pwm", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, pwm: %s\n",i ,j, result); 
                                if (strcmp(result,"true")==0) {
                                  jmri_web->jmri_ptr->devdata[i].i2c_pwm[j] = true;
                                } else {
                                  jmri_web->jmri_ptr->devdata[i].i2c_pwm[j] = false;
                                }
                                
                                jsonIndexList(buff, "lang", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, lang: %s\n",i ,j, result); 
                                jmri_web->jmri_ptr->devdata[i].lang[j] = atoi(result);

                                jsonIndexList(buff, "hang", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, hang: %s\n",i ,j, result); 
                                jmri_web->jmri_ptr->devdata[i].hang[j] = atoi(result);

                                jsonIndexList(buff, "desc", i, j, &result);          
                                //Serial.printf("\nresult board: %d, idx: %d, hang: %s\n",i ,j, result); 
                                sprintf( jmri_web->jmri_ptr->devdata[i].desc[j], result );                              
                                
                        }
                    }

                    free(result);
                    free(buff);  
                    JMRI_STORE::save();
                             
                    JMRI_HELPER::logging(2,"Heap size: %u\n", ESP.getFreeHeap());   
                    request->redirect("/");
            }    
      
    });

    //main json page. Provide all configuration to populate webpage
    server.on("/json2", HTTP_GET, [] (AsyncWebServerRequest *request){

            JMRI_HELPER::logging(2,"Heap size: %u\n", ESP.getFreeHeap());
            AsyncResponseStream *response = request->beginResponseStream("application/json");
          
            JMRI_WEB::makejson(response);
            
            JMRI_HELPER::logging(2,"Heap size: %u\n", ESP.getFreeHeap());         
            request->send(response);

    });

    //call for json export configuration to file
    server.on("/json", HTTP_GET, [] (AsyncWebServerRequest *request){

            JMRI_HELPER::logging(2,"Heap size: %u\n", ESP.getFreeHeap());
            AsyncResponseStream *response = request->beginResponseStream("application/json");
            response->addHeader("Content-Disposition", "attachment; filename=\"" + jmri_web->jmri_ptr->wifi_client.macAddress() +  "_backup.json\"" );

            JMRI_WEB::makejson(response);
            
            JMRI_HELPER::logging(2,"Heap size: %u\n", ESP.getFreeHeap());       
            request->send(response);

    });

    //change debug/logging level
    server.on("/changeLogradio", HTTP_POST, [] (AsyncWebServerRequest *request){

        AsyncWebParameter *rval = request->getParam(0);
        jmri_web->jmri_ptr->data.loglvl = (uint8_t)rval->value().toInt();
        JMRI_STORE::saveConfig();
        JMRI_HELPER::logging(1,"Logging level set to: %d\n",jmri_web->jmri_ptr->data.loglvl);                            
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
        
        if (  b->value() == jmri_web->jmri_ptr->data.mqtt_server_host ){   
          //JMRI_HELPER::logging(1,"New MQTT host IP is the same as the old!");
        } else {
          //JMRI_HELPER::logging(1,"New MQTT host IP updated...");
          JMRI_WEB::update_mqtt_host( const_cast<char*>(b->value().c_str()) );
        }

        JMRI_HELPER::logging(2,"Update MQTT IP: %s\n",const_cast<char*>(b->value().c_str())); 
        request->send(200, "text/plain", "MQTT Host IP Changed");

    });

    //called when change in mqtt topic is sent
    server.on("/changemqtttopic", HTTP_POST, [] (AsyncWebServerRequest *request){
       
        AsyncWebParameter *b = request->getParam(0);
        
        if (  b->value() == jmri_web->jmri_ptr->data.mqtt_topic ){   
          JMRI_HELPER::logging(1,"New topic same as old!");
        } else {
          JMRI_HELPER::logging(1,"New topic different!");
          JMRI_WEB::update_mqtt_topic( const_cast<char*>(b->value().c_str()) );
        }

        JMRI_HELPER::logging(1,"Update MQTT Topic: %s\n",const_cast<char*>(b->value().c_str())); 
        request->send(200, "text/plain", "MQTT Host IP Changed");

    });

    //called when change in mqtt port is sent
    server.on("/changemqttport", HTTP_POST, [] (AsyncWebServerRequest *request){
       
        AsyncWebParameter *b = request->getParam(0);
        ushort newPort = (ushort)b->value().toInt();
        
        if (  newPort == jmri_web->jmri_ptr->data.mqtt_server_port ){   
          JMRI_HELPER::logging(1,"New port same as old!");
        } else {
          JMRI_HELPER::logging(1,"New port different!");
          JMRI_WEB::update_mqtt_port( newPort );
        }

        JMRI_HELPER::logging(1,"Update MQTT port: %d\n",newPort); 
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
        
        if (jmri_web->jmri_ptr->devdata[board].i2c_names[thepin] != newName){       
            jmri_web->jmri_ptr->devdata[board].i2c_names[thepin] = newName;   
            JMRI_STORE::saveBoard(&board); 
        }
        
        JMRI_HELPER::logging(1,"Updated board: %d, pin %d with name: %d\n",board, thepin, newName);                      
        request->send(200, "text/plain", "Name Changed...");

    });

    //called when change in any system names is sent
    server.on("/changedesc", HTTP_POST, [] (AsyncWebServerRequest *request){
       
        AsyncWebParameter *b = request->getParam(0);
        AsyncWebParameter *p = request->getParam(1);
        AsyncWebParameter *v = request->getParam(2);
        
        uint8_t board  = (uint8_t)b->value().toInt(); 
        uint8_t thepin = (uint8_t)p->value().toInt();
        
        sprintf(jmri_web->jmri_ptr->devdata[board].desc[thepin],"%.10s",v->value().c_str());
        JMRI_STORE::saveBoard(&board);
        
        JMRI_HELPER::logging(1,"Updated board: %d, pin %d with description: %s\n",board, thepin, v->value().c_str());                     
        request->send(200, "text/plain", "Description Changed...");

    });

     //called when change in any system names is sent
     server.on("/changebdesc", HTTP_POST, [] (AsyncWebServerRequest *request){
       
        AsyncWebParameter *b = request->getParam(0);
        AsyncWebParameter *d = request->getParam(1);
        
        uint8_t board  = (uint8_t)b->value().toInt(); 
        
        sprintf( jmri_web->jmri_ptr->devdata[board].bdesc, d->value().c_str() );
        JMRI_STORE::saveBoard(&board);
        
        JMRI_HELPER::logging(1,"Updated board %d: name: %s,\n",board, d->value().c_str());                     
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
          jmri_web->jmri_ptr->devdata[board].lang[thepin] = newAngle;
          if (jmri_web->jmri_ptr->devdata[board].i2c_state[thepin] == 'C' && board == 0){
              jmri_web->jmri_ptr->servos[thepin].write(newAngle);
          }
        } else {
          jmri_web->jmri_ptr->devdata[board].hang[thepin] = newAngle;
          if (jmri_web->jmri_ptr->devdata[board].i2c_state[thepin] == 'T' && board == 0){
              jmri_web->jmri_ptr->servos[thepin].write(newAngle);
          }
        }

        JMRI_HELPER::logging(1,"Updated board: %d, pin %d with name: %d\n",board, thepin, newAngle);
        JMRI_STORE::saveBoard(&board);        
        request->send(200, "text/plain", "Created");

    });

    
    //When in AP mode page is served to provide method to enter local SSID and password
    //server.on("/update", [](AsyncWebServerRequest *request) {         
        //char output[1300]; 
        
        //sprintf(output,"%s\n","<!DOCTYPE HTML>\r\n<html>JMRI Wifi Accessory credentials setup.");            
        //sprintf(output + strlen(output),"%s<p>\n",jmri_web->jmri_ptr->wifi_client.softAPIP().toString().c_str());
        //sprintf(output + strlen(output), jmri_web->jmri_ptr->wifilist);
        //sprintf(output + strlen(output), "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32>");
        //sprintf(output + strlen(output), "<label>Pass: </label><input name='pass' length=64><br><input type='submit'></form></html>");
        //request->send(200, "text/html", output );            
    //});

    server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request){

            AsyncResponseStream *response = request->beginResponseStream("text/html");       
            response->printf("%s\n","<!DOCTYPE HTML>\r\n<html>JMRI Wifi Accessory credentials setup.");            
            response->printf("%s<p>\n",jmri_web->jmri_ptr->wifi_client.softAPIP().toString().c_str());
            response->printf( jmri_web->jmri_ptr->wifilist );
            response->printf("</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32>");
            response->printf("<label>Pass: </label><input name='pass' length=64><br><input type='submit'></form></html>");
            //wifilist(response);            
            //JMRI_HELPER::logging(2,"Heap size: %u\n", ESP.getFreeHeap());         
            request->send(response);

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
              
                  JMRI_HELPER::logging(1,"Saving ssid and password to eeprom....\n");
                  JMRI_HELPER::logging(1,"ssid: %s pass: %s\n", qssid, qpass);
      
                  jmri_web->jmri_ptr->ssidupdate = false;
                  
                  sprintf(jmri_web->jmri_ptr->data.ssid, qssid);
                  sprintf(jmri_web->jmri_ptr->data.pass, qpass);
                  JMRI_STORE::saveConfig();
                  
                  jmri_web->jmri_ptr->wifi_client.disconnect();
                 
                  sprintf(content, "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}");
                  statusCode = 200;
                
            } else {
                  sprintf(content, "{\"Error\":\"Problem sending connecting to ssid. Try again...\"}");
                  statusCode = 404;
                  JMRI_HELPER::logging(2,"Sending 404\n");
            }
            
            request->send(statusCode, "application/json", content);   
      
    });

    server.onNotFound(notFound);
    server.begin();
    
}

void JMRI_WEB::update_started() {
      Serial.println("CALLBACK:  HTTP update process started");
    }
    
void JMRI_WEB::update_finished() {
      Serial.println("CALLBACK:  HTTP update process finished");
    }
    
void JMRI_WEB::update_progress(int cur, int total) {
      Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
    }
    
void JMRI_WEB::update_error(int err) {
      Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
    }
      
void JMRI_WEB::makejson(AsyncResponseStream *response) {

            uint8_t b=0, p=0;
            
            response->printf("{\"mqtthost\":\"%s\",\"mqttport\":%d,\"mqtttopic\":\"%s\",\"clientip\":\"%s\",\"clientmac\":\"%s\",\""
                              "ssid\":\"%s\",\"pass\":\"%s\",\"number\":%d,\"nPCFDev\":%d,",
                              jmri_web->jmri_ptr->data.mqtt_server_host, jmri_web->jmri_ptr->data.mqtt_server_port, jmri_web->jmri_ptr->data.mqtt_topic, 
                              jmri_web->jmri_ptr->wifi_client.localIP().toString().c_str(), jmri_web->jmri_ptr->wifi_client.macAddress().c_str(), jmri_web->jmri_ptr->data.ssid, 
                              jmri_web->jmri_ptr->data.pass,jmri_web->jmri_ptr->nDevices, jmri_web->jmri_ptr->nPCFDev);

            response->printf("\"addr\":[");

            for (b = 0; b<jmri_web->jmri_ptr->nDevices-1; b++){
                  response->printf("%d,",(int)jmri_web->jmri_ptr->devdata[b].i2c_addr);
            }
            response->printf("%d]",jmri_web->jmri_ptr->devdata[jmri_web->jmri_ptr->nDevices-1].i2c_addr);

            response->printf(",\"type\":[");
            for (b = 0; b<jmri_web->jmri_ptr->nDevices-1; b++){
                  response->printf("%d,",jmri_web->jmri_ptr->boardinfo[b].i2ctype);
            }
            response->printf("%d]",jmri_web->jmri_ptr->boardinfo[jmri_web->jmri_ptr->nDevices-1].i2ctype);
            
            response->printf(",\"bdesc\":[");
            for (b = 0; b<jmri_web->jmri_ptr->nDevices-1; b++){
                  response->printf("\"%s\",",jmri_web->jmri_ptr->devdata[b].bdesc);
            }
            response->printf("\"%s\"]",jmri_web->jmri_ptr->devdata[jmri_web->jmri_ptr->nDevices-1].bdesc);            
            
            response->printf(",\"names\":{");
            for (b = 0; b<jmri_web->jmri_ptr->nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("%d,",jmri_web->jmri_ptr->devdata[b].i2c_names[p]);
                }
                if (b<jmri_web->jmri_ptr->nDevices-1){
                  response->printf("%d],",jmri_web->jmri_ptr->devdata[b].i2c_names[I2C_PINS-1]);
                } else {
                  response->printf("%d]}",jmri_web->jmri_ptr->devdata[b].i2c_names[I2C_PINS-1]);
                }
             }  
             
            response->printf(",\"mode\":{");

            for (b = 0; b<jmri_web->jmri_ptr->nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("\"%c\",",jmri_web->jmri_ptr->devdata[b].i2c_mode[p]);
                }
                if (b<jmri_web->jmri_ptr->nDevices-1){
                  response->printf("\"%c\"],",jmri_web->jmri_ptr->devdata[b].i2c_mode[I2C_PINS-1]);
                } else {
                  response->printf("\"%c\"]}",jmri_web->jmri_ptr->devdata[b].i2c_mode[I2C_PINS-1]);
                }
            }
            
            response->printf(",\"state\":{");

            for (b = 0; b<jmri_web->jmri_ptr->nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("\"%c\",",jmri_web->jmri_ptr->devdata[b].i2c_state[p]);
                }
                if (b<jmri_web->jmri_ptr->nDevices-1){
                  response->printf("\"%c\"],",jmri_web->jmri_ptr->devdata[b].i2c_state[I2C_PINS-1]);
                } else {
                  response->printf("\"%c\"]}",jmri_web->jmri_ptr->devdata[b].i2c_state[I2C_PINS-1]);
                }
            } 

            response->printf(",\"pwm\":{");

            for (b = 0; b<jmri_web->jmri_ptr->nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("%s,", jmri_web->jmri_ptr->devdata[b].i2c_pwm[p] ? "true" : "false");
                }
                if (b<jmri_web->jmri_ptr->nDevices-1){
                  response->printf("%s],",jmri_web->jmri_ptr->devdata[b].i2c_pwm[I2C_PINS-1] ? "true" : "false");
                } else {
                  response->printf("%s",jmri_web->jmri_ptr->devdata[b].i2c_pwm[I2C_PINS-1] ? "true" : "false");
                }
            }

            response->printf("]},\"lang\":{");

             for (b = 0; b<jmri_web->jmri_ptr->nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("%d,", jmri_web->jmri_ptr->devdata[b].lang[p]);
                }
                if (b<jmri_web->jmri_ptr->nDevices-1){
                  response->printf("%d],",jmri_web->jmri_ptr->devdata[b].lang[I2C_PINS-1]);
                } else {
                  response->printf("%d]}",jmri_web->jmri_ptr->devdata[b].lang[I2C_PINS-1]);
                }
            }

            response->printf(",\"hang\":{");

            for (b = 0; b<jmri_web->jmri_ptr->nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("%d,", jmri_web->jmri_ptr->devdata[b].hang[p]);
                }
                if (b<jmri_web->jmri_ptr->nDevices-1){
                  response->printf("%d],",jmri_web->jmri_ptr->devdata[b].hang[I2C_PINS-1]);
                } else {
                  response->printf("%d]}",jmri_web->jmri_ptr->devdata[b].hang[I2C_PINS-1]);
                }
            }

            response->printf(",\"desc\":{");

            for (b = 0; b<jmri_web->jmri_ptr->nDevices; b++){
                response->printf("\"%d\":[",b);
                for (p = 0; p<I2C_PINS-1; p++){
                  response->printf("\"%s\",", jmri_web->jmri_ptr->devdata[b].desc[p]);
                }
                if (b<jmri_web->jmri_ptr->nDevices-1){
                  response->printf("\"%s\"],",jmri_web->jmri_ptr->devdata[b].desc[I2C_PINS-1]);
                } else {
                  response->printf("\"%s\"]}",jmri_web->jmri_ptr->devdata[b].desc[I2C_PINS-1]);
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

//helper for updating the mqtt host
void JMRI_WEB::update_mqtt_host(char* mqtt_host){
        
        sprintf(jmri_web->jmri_ptr->data.mqtt_server_host, mqtt_host);  
        JMRI_STORE::saveConfig();    

        JMRI_MQTT::disconnect_mqtt();
        JMRI_MQTT::hostIPupdate();

}

//helper for updating the mqtt port
void JMRI_WEB::update_mqtt_port(ushort mqtt_port){
        
        jmri_web->jmri_ptr->data.mqtt_server_port = mqtt_port;
        JMRI_STORE::saveConfig();

        JMRI_MQTT::disconnect_mqtt();
        JMRI_MQTT::hostIPupdate();
  
}

//helper for updating the mqtt topic
void JMRI_WEB::update_mqtt_topic(char* mqtt_topic){

        sprintf(jmri_web->jmri_ptr->data.mqtt_topic, mqtt_topic);
        JMRI_STORE::saveConfig();

        JMRI_MQTT::disconnect_mqtt();
        JMRI_MQTT::hostIPupdate();
  
}

//Instructions to intial wifi setup and initiates AP mode 
void JMRI_WEB::startAPWeb(){

      JMRI_HELPER::logging(0,"\nLocal access point running...\n");
      //Serial.println("Connect to AP: JMRI-ACC-" + jmri_web->jmri_ptr->wifi_client.macAddress().c_str());
      JMRI_HELPER::logging(0,"Connect to AP: JMRI-ACC-%s\n", jmri_web->jmri_ptr->wifi_client.macAddress().c_str() );
      JMRI_HELPER::logging(0,"Then navigate to this IP: %s, using a browser.\n", jmri_web->jmri_ptr->wifi_client.softAPIP().toString().c_str());
      
}

int JMRI_WEB::pointer(){
  return(webAddress);
}

JMRI_WEB *JMRI_WEB::jmri_web=NULL;
int JMRI_WEB::webAddress=0;
