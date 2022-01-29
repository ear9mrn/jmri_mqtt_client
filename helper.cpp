
#include "helper.h"
#include "jmri_store.h"
#include "i2c.h"
#include "mqtt.h"
#include "jmri_telnet.h"

//initate helper functions and set pointer to data and itself
void JMRI_HELPER::help_init(jmriData *jmri_data) {

        jmri_help=(JMRI_HELPER *)calloc(1,sizeof(JMRI_HELPER));
        jmri_help->jmri_ptr = jmri_data;

}

//set turnout based in input
void JMRI_HELPER::setTurnout(ushort sysname, char* message){

        char    newstate   = 'C';
        uint8_t pins = PINS;

        
        //iterate over all boards and pins to find system name that matches
        //i am sure there is a better way to do this???
        for (uint8_t j=0; j<jmri_help->jmri_ptr->nDevices; j++){ 

                //we only iterate 9 pins for ESP8266, 16 for the rest.
                if (j > 0) pins = I2C_PINS; 
                
                for (uint8_t i = 0; i<pins; i++){ 

                     //if system name matches and mode is 'T' (turnout) do something
                     if (jmri_help->jmri_ptr->devdata[j].i2c_names[i] == sysname && jmri_help->jmri_ptr->devdata[j].i2c_mode[i] == 'T') {

                          //Serial.printf("board: %d, sysname: %d, Message: %s Type: %d\n", j, sysname, message, jmri_help->jmri_ptr->boardinfo[j].i2ctype);
                          //action for message THROWN
                          //logging(2,"board: %d pin: %d\n", j, i ); 
                          if ( strcmp(message,"THROWN")==0 ){ 
                            
                                newstate = 'T';                                                    
                                                                        
                                if (jmri_help->jmri_ptr->boardinfo[j].i2ctype == PCA9685) {
                                  
                                        if (jmri_help->jmri_ptr->devdata[j].i2c_pwm[i]){   //if this a PWM device                          
                                           jmri_help->jmri_ptr->boardinfo[j].pwm.setPWM(i, 0, JMRI_I2C::pulseWidth(jmri_help->jmri_ptr->devdata[j].hang[i]));
                                        } else {
                                           jmri_help->jmri_ptr->boardinfo[j].pwm.setPWM(i, 4096, 0);
                                        }
                                        
                                }  else if (jmri_help->jmri_ptr->boardinfo[j].i2ctype == PCF857x ) {
                                  
                                        jmri_help->jmri_ptr->boardinfo[j].PCF.write(i, LOW); 
                                                               
                                // ESP8266
                                } else {  
                                        jmri_help->jmri_ptr->servos[i].write(jmri_help->jmri_ptr->devdata[j].hang[i]);
                                }
                                
                          } else {
                                //action for message CLOSED
                                newstate = 'C';
                                if (jmri_help->jmri_ptr->boardinfo[j].i2ctype == PCA9685) {
                                  
                                        if (jmri_help->jmri_ptr->devdata[j].i2c_pwm[i]){ //if this a PWM device 
                                            jmri_help->jmri_ptr->boardinfo[j].pwm.setPWM(i, 0, JMRI_I2C::pulseWidth(jmri_help->jmri_ptr->devdata[j].lang[i]));
                                        } else {
                                            jmri_help->jmri_ptr->boardinfo[j].pwm.setPWM(i, 0, 4096);
                                        }     
                                } else if (jmri_help->jmri_ptr->boardinfo[j].i2ctype == PCF857x )  {
                                        jmri_help->jmri_ptr->boardinfo[j].PCF.write(i, HIGH);  
                                // ESP8266
                                } else {  
                                        jmri_help->jmri_ptr->servos[i].write(jmri_help->jmri_ptr->devdata[j].lang[i]);                                        
                                }
                          }                      

                          //save new state 
                          jmri_help->jmri_ptr->devdata[j].i2c_state[i] = newstate;    
                          //update dot colour on out web interface
                          setDot(&j, &i, &newstate); 
                          //this delay is required if multiple pins have the same system name
                          //due time it takes to update (send events) to webserver and updating/commit eeprom 
                          delay(10);                                         
                          //save new state to eeprom
                          JMRI_STORE::saveBoard(&j); 
                          logging(2,"sysname: %d, Message: %s\n", sysname, message);                    
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
        for (uint8_t j=0; j<jmri_help->jmri_ptr->nDevices; j++){ 

                //we only iterate 9 pins for ESP8266, 16 for the rest.
                if (j > 0) pins = I2C_PINS;
                for (uint8_t i = 0; i<pins; i++){ 

                   //if system name matches and mode is 'L' (light) do something
                   if (jmri_help->jmri_ptr->devdata[j].i2c_names[i] == sysname && jmri_help->jmri_ptr->devdata[j].i2c_mode[i] == 'L') {
                
                        //Serial.printf("Light: %d State: %s Value: %d\n",sysname, message, jmri_help->jmri_ptr->data->i2c_value[j][i]);  
                        //action for message OFF
                        //logging(2,"board: %d pin: %d\n", j, i ); 
                        if ( strcmp(message,"OFF")==0 ){  
                              newstate = 'F';                              
                              if (jmri_help->jmri_ptr->boardinfo[j].i2ctype == PCA9685) {
                                  jmri_help->jmri_ptr->boardinfo[j].pwm.setPWM(i, 0,4096);
                              } else if (jmri_help->jmri_ptr->boardinfo[j].i2ctype == PCF857x ){
                                  jmri_help->jmri_ptr->boardinfo[j].PCF.write(i, LOW);
                              //ESP8266
                              } else {
                                  analogWrite(pintable[i],0);     
                              }
                        } else {
                              //action for message ON
                              newstate = 'N';
                              if (jmri_help->jmri_ptr->boardinfo[j].i2ctype == PCA9685) {
                                  jmri_help->jmri_ptr->boardinfo[j].pwm.setPWM(i, 4096, 0);
                              } else if (jmri_help->jmri_ptr->boardinfo[j].i2ctype == PCF857x ) {
                                  jmri_help->jmri_ptr->boardinfo[j].PCF.write(i, HIGH);                          
                              //ESP8266
                              } else {
                                  analogWrite(pintable[i],255);  
                              }
                             
                        }                      

                        //save state, update webinterface and save to eeprom
                        jmri_help->jmri_ptr->devdata[j].i2c_state[i] = newstate;
                        setDot(&j, &i, &newstate);  
                        //this delay is required if multiple pins have the same system name
                        //due time it takes to update (send events) to webserver and updating/commit eeprom 
                        delay(10);
                        JMRI_STORE::saveBoard(&j);
                        logging(2,"sysname: %d, Message: %s\n", sysname, message);
                   }
              }
        }    
}
//function called when user changes the mode (radio button) on web interface
void JMRI_HELPER::setRadioButton(uint8_t* board, uint8_t* pinnum, char pmode ){
  
        char newstate = 'N';
        
        //detach interrupt if this pin is used as a sensor
        if (jmri_help->jmri_ptr->devdata[*board].i2c_mode[*pinnum] == 'S' ){
          detachInterrupt(digitalPinToInterrupt(pintable[*pinnum]));
        }     

        //label board/pin with new mode 'T','L','S'
        jmri_help->jmri_ptr->devdata[*board].i2c_mode[*pinnum] = pmode;

        //if action if new mode is sensor
        if (pmode == 'S' && *board==0){

              //set pin and interrrupt
              pinMode(pintable[*pinnum],INPUT_PULLUP); 
              attachInterruptArg(digitalPinToInterrupt(pintable[*pinnum]), JMRI_STORE::pinISR, jmri_help->jmri_ptr->boardinfo[*board].ptr[*pinnum], CHANGE);                            

              //get current state of pin
              newstate = 'I';
              if (digitalRead(pintable[*pinnum]) == HIGH){
                      newstate = 'A';
              }

              //disable webpage dot, update webpage with current state and send MQTT message
              flipDot      (board, pinnum, "off");
              changeSensor (board, pinnum, &newstate);   
              JMRI_MQTT::sendMessage(jmri_help->jmri_ptr->devdata[*board].i2c_names[*pinnum], &newstate );       

        //this only for PCF boards as PCA are output only.
        } else if (pmode == 'S' && *board > 0){
                                                                    
              newstate = 'I';
              if (jmri_help->jmri_ptr->boardinfo[*board].PCF.read(*pinnum) == LOW){
                      newstate = 'A';
              }

              //disable webpage dot, update webpage with current state and send MQTT message              
              flipDot      (board, pinnum, "off");
              changeSensor (board, pinnum, &newstate);
              //JMRI_MQTT::sendMessage(jmri_help->jmri_ptr->devdata[*board].i2c_names[*pinnum], &newstate );     

        //actions for change to light mode.
        } else if ( pmode == 'L') { 

              //set pinmode for ESP8266
              if ( *board == 0 ) {
                  pinMode(pintable[*pinnum],OUTPUT);
              } 

              //enable webpage dot, update webpage with current state and send MQTT message
              flipDot(board, pinnum, "on");
              jmri_help->jmri_ptr->devdata[*board].i2c_state[*pinnum] = newstate;                           
              JMRI_MQTT::sendMessage(jmri_help->jmri_ptr->devdata[*board].i2c_names[*pinnum], &newstate );                           

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
              jmri_help->jmri_ptr->devdata[*board].i2c_state[*pinnum] = newstate;                                
              if ( *board == 0 ) jmri_help->jmri_ptr->servos[*pinnum].attach( pintable[*pinnum], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH); 
              JMRI_MQTT::sendMessage(jmri_help->jmri_ptr->devdata[*board].i2c_names[*pinnum], &newstate );                
                                        
        }
        //save changes to eeprom
        JMRI_STORE::saveBoard(board);
        logging(2,"Board: %d, Pin: %d, mode updated to %c\n", *board, *pinnum, pmode);   
                  
}

//function called when the PWM checkbox is changed
void JMRI_HELPER::setPWMCheck(uint8_t* board, uint8_t* pinnum, bool* newstate ){

        logging(2,"Board: %d, pin: %d: state: %d\n", *board, *pinnum, *newstate);
        //update state in struct and save to eeprom 
        jmri_help->jmri_ptr->devdata[*board].i2c_pwm[*pinnum] = *newstate;
        JMRI_STORE::saveBoard(board);  
        
}
// function called with change of pin mode (radio buttons), to enable or disable click dot.
void JMRI_HELPER::flipDot(uint8_t* board, uint8_t* pinnum, const char* state ){

        char mess[15];        
        sprintf(mess, "%ddot%d:%s", *board, *pinnum, state );
        jmri_help->jmri_ptr->events->send( mess, "dotEnable", millis());
        //logging(2,"Flip dot: %s\n",mess);
        
}
// function called to change click dot colour and html data-state property
void JMRI_HELPER::setDot(uint8_t *board, uint8_t* pinnum, char* state ){

        char mess[15];   
        sprintf(mess, "%ddot%d:%c", *board, *pinnum, *state );     
        jmri_help->jmri_ptr->events->send(mess, "setDot", millis());
        //logging(2,"Set dot: %s\n",jmri_help->jmri_ptr->mess);
                 
}

//function called when a change to sensor state is detected (via interrupt)
void  JMRI_HELPER::changeSensor(uint8_t *board, uint8_t *pinnum, char* newstate){
        
        JMRI_MQTT::sendMessage(jmri_help->jmri_ptr->devdata[*board].i2c_names[*pinnum], newstate );
        jmri_help->jmri_ptr->devdata[*board].i2c_state[*pinnum] = *newstate;    
        setDot(board, pinnum, newstate);
        //logging(2,"Interrupt Detected. Board: %d, Pin: %d, State: %c\n", *board, *pinnum, *newstate);           
                        
}


void JMRI_HELPER::logging (uint8_t lvl, const char *format, ...){

   if ( format == NULL || lvl > jmri_help->jmri_ptr->data.loglvl) {
      return;
   }

   va_list valist;
   va_start(valist, format);

   int num = 0;
   char *token = NULL;
   char c = ' ';
   int i = 0;
   int len = strlen(format);
   int found = 0;
   char outtxt[200];
   outtxt[0] = '\0';
   
   while ( format[i] != '\0' )
   {
       num = 0;
       found = 0;
       token = NULL;

       if ( ( format[i] == '%' ) && ( ( i + 1 ) < len ) )
       {
          switch ( format[i+1] )
          {
             case 'd': {
                  found = 1;
                  num = va_arg(valist, int);
                  sprintf(outtxt + strlen(outtxt),"%d",num);      

             }
             break;

             case 'u': {
                  found = 1;
                  num = va_arg(valist, int);
                  sprintf(outtxt + strlen(outtxt),"%d",num);  
             }
             break;
             
             case 'x': {
                  found = 1;
                  num = va_arg(valist, int);
                  sprintf(outtxt + strlen(outtxt),"%#02x",num);               
             }
             break;
 
             case 's':{
                  found = 1;
                  token = va_arg(valist, char *);
                  if ( token != NULL ){
                     int j = 0;
                     while ( token[j] != '\0' ){
                        sprintf(outtxt + strlen(outtxt),"%c",token[j]);                      
                        j++;
                     }
                  }
             }
             break;
             
             case 'c': {
                  found = 1;
                  c = (char) va_arg(valist, int );
                  sprintf(outtxt + strlen(outtxt),"%c", c);               
             }
             break;
             
             case 'b': {
                  found = 1;
                  num = va_arg(valist, int );
                  char buffer [20];
                  itoa (num,buffer,2);
                  sprintf(outtxt + strlen(outtxt),"%s", buffer);            
             }
             break;
          

          }
          if ( found != 0 )
          {
             i += 2;
             continue;
          }
       }
       
       sprintf(outtxt + strlen(outtxt),"%c",format[i]);     
       i++;
   }
   
   va_end(valist);
   Serial.print(outtxt);
     
   if (jmri_help->jmri_ptr->telnetUp){
        JMRI_TEL::telPrint(outtxt);
   }
   
   if (jmri_help->jmri_ptr->server){
        jmri_help->jmri_ptr->events->send(outtxt, "updatelog", millis());
   }
   
}

//jrmi_helper struct pointer
int JMRI_HELPER::pointer(){
      return(helpAddress);
}

//jrmi_helper struct address
JMRI_HELPER *JMRI_HELPER::jmri_help=NULL;
int JMRI_HELPER::helpAddress=0;
