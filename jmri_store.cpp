
#include  "jmri_store.h"

jmriData  *JMRI_STORE::jmri_ptr;
bool    JMRI_STORE::_eepromUpdate = false;
uint8_t JMRI_STORE::_eepromCap = 0;

//interaction with EEPROM to save config and board information
void JMRI_STORE::store_init(jmriData *jmri_data){

    //pointer
    //jmri_store=(JMRI_STORE *)calloc(1,sizeof(JMRI_STORE));   
    jmri_ptr = jmri_data;

    //reset setup parameters during initial config for reset
    if (_eepromUpdate == true){
      _eepromUpdate = false;
      JMRI_I2C::set_i2cUpdate(true);  
      jmri_data->nDevices = 0;
      jmri_data->nPCADev = 0;
      jmri_data->nPCFDev = 0;
    }

    //start EERPOM and set size
    EEPROM.begin(4096);
    JMRI_HELPER::logging(1,F("\nGetting configuration data from EEPROM...\n"));

    //get main config info
    EEPROM.get(0,jmri_data->data);

    //check if first 4 charaters are correct (do we have a valid store?) If not, create one
    if ( strncmp(jmri_data->data.id,EESTORE_ID,sizeof(EESTORE_ID))!=0 ) { 
        JMRI_HELPER::logging(1,F("EEPROM is not configured. Setting up EEPROM...\n"));      
        configeeprom();  
        EEPROM.get(0,jmri_data->data);          
    }

    //get board EPS8266 config from EEPROM.
    uint16_t pos = sizeof(EEStoreData);
    EEPROM.get(pos,jmri_data->devdata[0]);
    
    //count number of devices stored in EEstore
    for (uint8_t i=1; i<DEVICES; i++){
      if (jmri_data->data.i2c_addr[i] > 0){
          _eepromCap++;
      }
    }  

    JMRI_HELPER::logging(1,F("%d boards stored in EEPROM...\n"),_eepromCap);
    
    //add 1 to device count for main ESP8266 main board.
    jmri_data->nDevices++;

    //how much EEPROM memory used to store data?
    ushort eepromSize   = ( sizeof(EEStoreData) +
                        sizeof(EEStoreDevData) * DEVICES  ) ;
    JMRI_HELPER::logging(2,F("EEPROM usage: %d bytes (max 4096 bytes).\n\n"), eepromSize );
    
    if(eepromSize > 4096){

        JMRI_HELPER::logging(1,F("\n##########################WARNING#############################\n"
                                 "Config data size (%dbytes) is greater than limit (4096 bytes).\n"
                                 "##########################WARNING#############################\n\n"), eepromSize );

    }

    configSetup();  
    JMRI_HELPER::logging(2,F("Retrieved configuration from EEPROM successfully...\n\n"));

}

uint8_t JMRI_STORE::eepromCap(){
      return _eepromCap;
}
void  JMRI_STORE::inc_eepromCap(){
    _eepromCap++;
}       
bool JMRI_STORE::eepromUpdate(){
    return _eepromUpdate;
}
//void JMRI_STORE::set_eepromUpdate(bool updt_eerpom){
//    _eepromUpdate = updt_eerpom;
//}
        

void JMRI_STORE::configSetup() {

    uint8_t b = 0;
    
    resetBoardinfo(1);
    jmri_ptr->boardinfo[b].i2ctype = ESP8266;  
    
    //read configuration for main board.
    for (uint8_t p=0; p<PINS; p++){
         
        //make pointers for interrupts.
        jmri_ptr->boardinfo[b].interrupt[p][0] = b;
        jmri_ptr->boardinfo[b].interrupt[p][1] = p;
        jmri_ptr->boardinfo[b].ptr[p] = &jmri_ptr->boardinfo[b].interrupt[p];
                       
        if (jmri_ptr->devdata[b].i2c_mode[p] == 'T' ){
          
              //this stops all the servos moving at boot
              //set them to the last know position before attaching.
              if (jmri_ptr->devdata[b].i2c_state[p] == 'C' ) {
                      jmri_ptr->servos[p].write(jmri_ptr->devdata[b].lang[p]);                                          
              } else { 
                      jmri_ptr->servos[p].write(jmri_ptr->devdata[b].hang[p]);                  
              }             
              jmri_ptr->servos[p].attach( pintable[p], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);

        //set as input and interrupt if a sensor 
        } else if (jmri_ptr->devdata[b].i2c_mode[p] == 'S' ){
           
              pinMode(pintable[p], INPUT_PULLUP); 
              attachInterruptArg(digitalPinToInterrupt(pintable[p]), JMRI_STORE::pinISR, jmri_ptr->boardinfo[b].ptr[p], CHANGE);

        //set output and state if a light
        } else if (jmri_ptr->devdata[b].i2c_mode[p] == 'L' ) {
          
              pinMode(pintable[p], OUTPUT);             
              if (jmri_ptr->devdata[b].i2c_state[p] == 'N'){ //N == ON; F == OFF
                    digitalWrite(pintable[p], HIGH);
              } else {
                    digitalWrite(pintable[p], LOW);
              }
        }
    } 

}
void JMRI_STORE::resetBoardinfo(uint8_t startVal) {

    //set rest of struct array to default values prior to I2C scan
    for (uint8_t b=startVal; b<DEVICES; b++){      
       
          jmri_ptr->boardinfo[b].i2caddr         = 0;
          jmri_ptr->boardinfo[b].i2ctype         = UNDEF;
          jmri_ptr->boardinfo[b].sensorState     = 0;
          jmri_ptr->boardinfo[b].interrupt[0][0] = b;
          jmri_ptr->boardinfo[b].interrupt[0][1] = 0;
          jmri_ptr->boardinfo[b].ptr[0] = &jmri_ptr->boardinfo[b].interrupt[0];      
            
          jmri_ptr->devdata[b].i2c_addr          = 0 ;
          sprintf(jmri_ptr->devdata[b].bdesc," ");
                
          for (uint8_t i=0; i<I2C_PINS; i++){
                jmri_ptr->devdata[b].i2c_mode  [i]  = 'T';
                jmri_ptr->devdata[b].i2c_state [i]  = 'C';
                jmri_ptr->devdata[b].i2c_names [i]  =  0;
                jmri_ptr->devdata[b].i2c_pwm   [i]  =  true;   
                jmri_ptr->devdata[b].lang      [i]  =  45;  
                jmri_ptr->devdata[b].hang      [i]  =  135; 
                sprintf(jmri_ptr->devdata[b].desc[i]," ");          
          }
    }       
}

// full eeprom reset or initial settings for new ESP8266
void JMRI_STORE::configeeprom(){

        //clear the eeprom
        for (uint16_t i = 0 ; i < EEPROM.length() ; i++) {
              EEPROM.write(i, 0);
        }

        //write default settings to eeprom
        sprintf(jmri_ptr->data.id,EESTORE_ID);
        sprintf(jmri_ptr->data.mqtt_server_host,DEFAULTMQTTSERVER);
        jmri_ptr->data.mqtt_server_port = DEFAULTMQTTPORT;
        sprintf(jmri_ptr->data.mqtt_topic, DEFAULTMQTTTOPIC);
        sprintf(jmri_ptr->data.ssid, DEFAULTSSID);
        sprintf(jmri_ptr->data.pass, DEFAULTPASSWORD);    
        jmri_ptr->data.loglvl = 1; 
        //sprintf(jmri_ptr->data.version, VERSION);

        resetBoardinfo();
         
        for (uint8_t j=0; j<DEVICES; j++){
          
                  jmri_ptr->data.i2c_addr[j] = 0 ;
        }
        
        //commit info to eeprom
        save();
        //reset(); 

}

//For sensor interrupts on ESP8266
void IRAM_ATTR JMRI_STORE::pinISR(void * myarg){
  
    //ensure we have not called this too quickly (debounce) and boot is completed.
    if (millis() - jmri_ptr->previousMillis > interval && jmri_ptr->bootComplete) {

        //board and pin info from pointer
        uint8_t board  = ((uint8_t*)myarg)[0];
        uint8_t thepin = ((uint8_t*)myarg)[1];
        char newstate = 'I';
        
        //JMRI_HELPER::logging(1,F("Interrupt Detected...")); 

        //set new state and update sensor info (send MQTT)
        if (jmri_ptr->devdata[board].i2c_state[thepin] == 'I' ){
              newstate = 'A';                    
        }     
        
        JMRI_HELPER::changeSensor( &board, &thepin, &newstate);
        jmri_ptr->previousMillis = millis();      
       
    }
 
}

//For sensor interrupts that come from I2C (PCF857x boards)
void IRAM_ATTR JMRI_STORE::pinISR_PCF(void * myarg){

    //ensure we have not called this too quickly (debounce) and boot is completed.
    if ( millis() - jmri_ptr->previousMillis > interval && jmri_ptr->bootComplete ) {

        //we only know which board sent the interrupt. 
        //We work out state and pin later.
        //uint8_t board  = ((uint8_t*)myarg)[0];
        //uint8_t thepin = ((uint8_t*)myarg)[1];
        //JMRI_HELPER::logging(2,F("Interrupt board: %d\n"),board);
        jmri_ptr->pcfBChange = ((uint8_t*)myarg)[0];
        jmri_ptr->pcfChange = true;      
          
    }

    jmri_ptr->previousMillis = millis();
    
}

//save all data to the eeprom store
void JMRI_STORE::save()
{   
           
      JMRI_HELPER::logging(2,F("Saved to EEPROM\n"));

      for(uint8_t i=0; i<DEVICES; i++){
         jmri_ptr->data.i2c_addr[i] = jmri_ptr->devdata[i].i2c_addr;
         jmri_ptr->boardinfo[i].eepromaddr = i;
      }
      
      EEPROM.put(0,jmri_ptr->data);     
      uint16_t pos = sizeof(EEStoreData);      
      EEPROM.put(pos,jmri_ptr->devdata);      
      EEPROM.commit(); 
           
}

//save just main config to the eeprom store
void JMRI_STORE::saveConfig() {

      //while (jmri_ptr->eepromblock) {}
      //jmri_ptr->eepromblock = true;

      for(uint8_t i=0; i<DEVICES; i++){
         jmri_ptr->data.i2c_addr[i] = jmri_ptr->devdata[i].i2c_addr;
      }
        
      EEPROM.put(0,jmri_ptr->data);
      EEPROM.commit(); 
      //jmri_ptr->eepromblock = false;
      
}

//get board info from eeprom store
void JMRI_STORE::getBoardInfo(uint8_t loc, uint8_t devloc){ 
       
      uint16_t pos = sizeof(EEStoreData) 
              + sizeof(EEStoreDevData)*loc ;
      
      jmri_ptr->boardinfo[devloc].eepromaddr = loc;
      EEPROM.get(pos,jmri_ptr->devdata[devloc]);
                       
}

//get base board info from eeprom store
void JMRI_STORE::getBaseInfo(){ 
       
      uint16_t pos = sizeof(EEStoreData);
      
      jmri_ptr->boardinfo[0].eepromaddr = 0;
      EEPROM.get(pos,jmri_ptr->devdata[0]);
                      
}
//save just one specific board info to the eeprom store
void JMRI_STORE::saveBoard(uint8_t *board){   

//      for(uint8_t i=0; i<DEVICES; i++){
//         jmri_ptr->data.i2c_addr[i] = jmri_ptr->devdata[i].i2c_addr;
//      }

      EEPROM.put(0,jmri_ptr->data);
      
      uint16_t pos = sizeof(EEStoreData) +
                sizeof(EEStoreDevData) * jmri_ptr->boardinfo[*board].eepromaddr;                
       
      //Serial.printf("Eeprom position: %d, board: %d, storeadd: %d\n", pos, *board, jmri_ptr->boardinfo[*board].eepromaddr);
      EEPROM.put(pos,jmri_ptr->devdata[*board]);  

      EEPROM.commit(); 
      
}
