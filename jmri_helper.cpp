
#include "jmri_helper.h"


//set turnout based in input
void JMRI_HELPER::setTurnout(ushort sysname, char* message){

        char    newstate   = 'C';
        uint8_t pins = PINS;

        
        //iterate over all boards and pins to find system name that matches
        //i am sure there is a better way to do this???
        for (uint8_t j=0; j<jmri_data.nDevices; j++){ 

                //we only iterate 9 pins for ESP8266, 16 for the rest.
                if (j > 0) pins = I2C_PINS; 
                
                for (uint8_t i = 0; i<pins; i++){ 

                     //if system name matches and mode is 'T' (turnout) do something
                     if (jmri_data.devdata[j].i2c_names[i] == sysname && jmri_data.devdata[j].i2c_mode[i] == 'T') {

                          //Serial.printf("board: %d, sysname: %d, Message: %s Type: %d\n", j, sysname, message, jmri_data.boardinfo[j].i2ctype);
                          //action for message THROWN
                          //logging(2,"board: %d pin: %d\n", j, i ); 
                          if ( strcmp(message,"THROWN")==0 ){ 
                            
                                newstate = 'T';                                                    
                                                                        
                                if (jmri_data.boardinfo[j].i2ctype == PCA9685) {
                                  
                                        if (jmri_data.devdata[j].i2c_pwm[i]){   //if this a PWM device                          
                                           jmri_data.boardinfo[j].pwm.setPWM(i, 0, JMRI_I2C::pulseWidth(jmri_data.devdata[j].hang[i]));
                                        } else {
                                           jmri_data.boardinfo[j].pwm.setPWM(i, 4096, 0);
                                        }
                                        
                                }  else if (jmri_data.boardinfo[j].i2ctype == PCF857x ) {
                                  
                                        jmri_data.boardinfo[j].PCF.write(i, LOW); 
                                                               
                                // ESP8266
                                } else {  
                                        jmri_data.servos[i].write(jmri_data.devdata[j].hang[i]);
                                }
                                
                          } else {
                                //action for message CLOSED
                                newstate = 'C';
                                if (jmri_data.boardinfo[j].i2ctype == PCA9685) {
                                  
                                        if (jmri_data.devdata[j].i2c_pwm[i]){ //if this a PWM device 
                                            jmri_data.boardinfo[j].pwm.setPWM(i, 0, JMRI_I2C::pulseWidth(jmri_data.devdata[j].lang[i]));
                                        } else {
                                            jmri_data.boardinfo[j].pwm.setPWM(i, 0, 4096);
                                        }     
                                } else if (jmri_data.boardinfo[j].i2ctype == PCF857x )  {
                                        jmri_data.boardinfo[j].PCF.write(i, HIGH);  
                                // ESP8266
                                } else {  
                                        jmri_data.servos[i].write(jmri_data.devdata[j].lang[i]);                                        
                                }
                          }                      

                          //save new state 
                          jmri_data.devdata[j].i2c_state[i] = newstate;    
                          //update dot colour on out web interface
                          setDot(&j, &i, &newstate); 
                          //this delay is required if multiple pins have the same system name
                          //due time it takes to update (send events) to webserver and updating/commit eeprom 
                          delay(10);                                         
                          //save new state to eeprom
                          JMRI_STORE::saveBoard(&j); 
                          logging(2,F("sysname: %d, Message: %s\n"), sysname, message);                    
                     }
                }
        }
}
//change light state
void JMRI_HELPER::setLight(ushort sysname, char* message){

        char    newstate  =   'N';
        uint8_t pins      =   PINS;
             
        //iterate over all boards and pins to find system name that matches
        //i am sure there is a better way to do this???
        for (uint8_t j=0; j<jmri_data.nDevices; j++){ 

                //we only iterate 9 pins for ESP8266, 16 for the rest.
                if (j > 0) pins = I2C_PINS;
                for (uint8_t i = 0; i<pins; i++){ 

                   //if system name matches and mode is 'L' (light) do something
                   if (jmri_data.devdata[j].i2c_names[i] == sysname && jmri_data.devdata[j].i2c_mode[i] == 'L') {
                
                        //Serial.printf("Light: %d State: %s Value: %d\n",sysname, message, jmri_data.data->i2c_value[j][i]);  
                        //action for message OFF
                        //logging(2,F("board: %d pin: %d\n"), j, i ); 
                        if ( strcmp(message,"OFF")==0 ){  
                              newstate = 'F';                              
                              if (jmri_data.boardinfo[j].i2ctype == PCA9685) {
                                  jmri_data.boardinfo[j].pwm.setPWM(i, 0,4096);
                              } else if (jmri_data.boardinfo[j].i2ctype == PCF857x ){
                                  jmri_data.boardinfo[j].PCF.write(i, LOW);
                              //ESP8266
                              } else {
                                  analogWrite(pintable[i],0);     
                              }
                        } else {
                              //action for message ON
                              newstate = 'N';
                              if (jmri_data.boardinfo[j].i2ctype == PCA9685) {
                                  jmri_data.boardinfo[j].pwm.setPWM(i, 4096, 0);
                              } else if (jmri_data.boardinfo[j].i2ctype == PCF857x ) {
                                  jmri_data.boardinfo[j].PCF.write(i, HIGH);                          
                              //ESP8266
                              } else {
                                  analogWrite(pintable[i],255);  
                              }
                             
                        }                      

                        //save state, update webinterface and save to eeprom
                        jmri_data.devdata[j].i2c_state[i] = newstate;
                        setDot(&j, &i, &newstate);  
                        //this delay is required if multiple pins have the same system name
                        //due time it takes to update (send events) to webserver and updating/commit eeprom 
                        delay(10);
                        JMRI_STORE::saveBoard(&j);
                        logging(2,F("sysname: %d, Message: %s\n"), sysname, message);
                   }
              }
        }    
}
//function called when user changes the mode (radio button) on web interface
void JMRI_HELPER::setRadioButton(uint8_t* board, uint8_t* pinnum, char pmode ){
  
        char newstate = 'N';
        Messg messg;
        
        //detach interrupt if this pin is used as a sensor
        if (jmri_data.devdata[*board].i2c_mode[*pinnum] == 'S' ){
          detachInterrupt(digitalPinToInterrupt(pintable[*pinnum]));
        }     

        //label board/pin with new mode 'T','L','S'
        jmri_data.devdata[*board].i2c_mode[*pinnum] = pmode;

        //if action if new mode is sensor
        if (pmode == 'S' && *board==0){

              //set pin and interrrupt
              pinMode(pintable[*pinnum],INPUT_PULLUP); 
              attachInterruptArg(digitalPinToInterrupt(pintable[*pinnum]), JMRI_STORE::pinISR, jmri_data.boardinfo[*board].ptr[*pinnum], CHANGE);                            

              //get current state of pin
              newstate = 'I';
              if (digitalRead(pintable[*pinnum]) == HIGH){
                      newstate = 'A';
              }

              //disable webpage dot, update webpage with current state and send MQTT message
              flipDot      (board, pinnum, "off");
              changeSensor (board, pinnum, &newstate);   
              //JMRI_MQTT::sendMessage(jmri_data.devdata[*board].i2c_names[*pinnum], &newstate );  
              messg.sysname = jmri_data.devdata[*board].i2c_names[*pinnum];
              messg.messtate = newstate;
              jmri_data.messg.push(messg);      

        //this only for PCF boards as PCA are output only.
        } else if (pmode == 'S' && *board > 0){
                                                                    
              newstate = 'I';
              if (jmri_data.boardinfo[*board].PCF.read(*pinnum) == LOW){
                      newstate = 'A';
              }

              //disable webpage dot, update webpage with current state and send MQTT message              
              flipDot      (board, pinnum, "off");
              changeSensor (board, pinnum, &newstate);
              //JMRI_MQTT::sendMessage(jmri_data.devdata[*board].i2c_names[*pinnum], &newstate );     

        //actions for change to light mode.
        } else if ( pmode == 'L') { 

              //set pinmode for ESP8266
              if ( *board == 0 ) {
                  pinMode(pintable[*pinnum],OUTPUT);
              } 

              //enable webpage dot, update webpage with current state and send MQTT message
              flipDot(board, pinnum, "on");
              jmri_data.devdata[*board].i2c_state[*pinnum] = newstate;                           
              //JMRI_MQTT::sendMessage(jmri_data.devdata[*board].i2c_names[*pinnum], &newstate );  
              messg.sysname = jmri_data.devdata[*board].i2c_names[*pinnum];
              messg.messtate = newstate;
              jmri_data.messg.push(messg);                         

        //actions for change to turnout mode.
        } else if ( pmode == 'T') { 

              //set pinmode for ESP8266
              if ( *board == 0 ) {
                  pinMode(pintable[*pinnum],OUTPUT);
              } 
              
              //enable webpage dot, update webpage with current state
              flipDot  (board, pinnum, "on");           
              newstate = 'C';
              
              //save new state, attach to servo array for ESP8266 and send MQTT message
              jmri_data.devdata[*board].i2c_state[*pinnum] = newstate;                                
              if ( *board == 0 ) jmri_data.servos[*pinnum].attach( pintable[*pinnum], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH); 
              //JMRI_MQTT::sendMessage(jmri_data.devdata[*board].i2c_names[*pinnum], &newstate );                              
              messg.sysname = jmri_data.devdata[*board].i2c_names[*pinnum];
              messg.messtate = newstate;
              jmri_data.messg.push(messg); 
              
                                        
        }
        //save changes to eeprom
        JMRI_STORE::saveBoard(board);
        logging(2,F("Board: %d, Pin: %d, mode updated to %c\n"), *board, *pinnum, pmode);   
                  
}

//function called when the PWM checkbox is changed
void JMRI_HELPER::setPWMCheck(uint8_t* board, uint8_t* pinnum, bool* newstate ){

        logging(2,F("Board: %d, pin: %d: state: %d\n"), *board, *pinnum, *newstate);
        //update state in struct and save to eeprom 
        jmri_data.devdata[*board].i2c_pwm[*pinnum] = *newstate;
        JMRI_STORE::saveBoard(board);  
        
}
// function called with change of pin mode (radio buttons), to enable or disable click dot.
void JMRI_HELPER::flipDot(uint8_t* board, uint8_t* pinnum, const char* state ){

        Event event;
        sprintf(event.a, "%ddot%d:%s", *board, *pinnum, state );
        sprintf(event.b, "dotEnable");  
        jmri_data.event.push(event);
        logging(2,F("Flip dot: %s\n"),event.a);
        
}
// function called to change click dot colour and html data-state property
void JMRI_HELPER::setDot(uint8_t *board, uint8_t* pinnum, char* state ){

        Event event;  
        sprintf(event.a, "%ddot%d:%c", *board, *pinnum, *state ); 
        sprintf(event.b, "setDot");   
        jmri_data.event.push(event);
        logging(2,F("Set dot: %s\n"),event.a);
        
}

//function called when a change to sensor state is detected (via interrupt)
void  JMRI_HELPER::changeSensor(uint8_t *board, uint8_t *pinnum, char* newstate){
        
        //JMRI_MQTT::sendMessage(jmri_data.devdata[*board].i2c_names[*pinnum], newstate );
        Messg messg;
        messg.sysname = jmri_data.devdata[*board].i2c_names[*pinnum];
        messg.messtate = *newstate;
        jmri_data.messg.push(messg); 
              
        jmri_data.devdata[*board].i2c_state[*pinnum] = *newstate;    
        setDot(board, pinnum, newstate);
        //logging(2,F("Interrupt Detected. Board: %d, Pin: %d, State: %c\n"), *board, *pinnum, *newstate);           
                        
}

void JMRI_HELPER::pollQueue(){ 

     if (jmri_data.event.count() > 0) {
      
          Event event = jmri_data.event.pop();        
          JMRI_WEB::send(event.a, event.b, millis());
          delay(20);
          
     }

     //PCF event queue
     if ( jmri_data.inter.count() > 0) {
      
          Inter inter = jmri_data.inter.pop();
          JMRI_HELPER::changeSensor( &inter.a, &inter.b, &inter.c);
          delay(20);
          
     }

     //PCF event queue
     if (jmri_data.pcfchange.count() > 0){
     
          JMRI_I2C::PCF_Change(jmri_data.pcfchange.pop()); 
          delay(20);         
     }

     //mqtt message queue
     if (jmri_data.messg.count() > 0){
     
          JMRI_MQTT::sendMessage(jmri_data.messg.pop());   
          delay(20);  
              
     }
     
}
void JMRI_HELPER::logging(uint8_t lvl, const __FlashStringHelper *format, ... ){

        if ( format == NULL || lvl > jmri_data.data.loglvl) {
            return;
        }

        va_list args;
        va_start(args, format );
        _p(lvl,format,args);
        va_end(args);
}

void JMRI_HELPER::_p(uint8_t lvl, const __FlashStringHelper *format, va_list args ){

      Event mevent;
      if(lvl == 2){
            sprintf(mevent.a, "DEBUG::");
      } else {
            mevent.a[0] = '\0';
      }
      #ifdef __AVR__
      vsnprintf_P(mevent.a + strlen(mevent.a), sizeof(mevent.a), (const char *)format, args); // progmem for AVR
      #else
      vsnprintf(mevent.a + strlen(mevent.a), sizeof(mevent.a), (const char *)format, args); // for the rest of the world
      #endif
      Serial.print(mevent.a);
      
   if (jmri_telnet::telnetUp()){
        jmri_telnet::telPrint(mevent.a);
   }
   
   //if (jmri_data.server){
   if (WiFi.status() == WL_CONNECTED){
      
        sprintf(mevent.b, "updatelog");   
        jmri_data.event.push(mevent);

   }
}
