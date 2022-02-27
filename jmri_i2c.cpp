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
 
#include "jmri_i2c.h"

bool  JMRI_I2C::_i2cUpdate = false;

void JMRI_I2C::i2c_init() {


      JMRI_HELPER::logging(2,F("Heap size: %u\n"), ESP.getFreeHeap()); 
     
      byte error, address;
      bool unique; 

      //if a rescan reset some values
      if (_i2cUpdate){
        
            JMRI_STORE::getBaseInfo();
            for (uint8_t j=1; j<jmri_data.nDevices; j++){
                jmri_data.boardinfo[j].addr = 0;
            }
            JMRI_STORE::configSetup();
            jmri_data.nDevices = 1;
            jmri_data.nPCADev = 0;
            jmri_data.nPCFDev = 0;
            _i2cUpdate = false;
                   
      }

      //set SDA and SCL pins
      JMRI_HELPER::logging(1,F("Scanning for I2C devices...\n"));
      Wire.begin(SDA, SCL);
          
      for (address = 1; address < 127; address++ )
      {
          //test first address
          Wire.beginTransmission(address);
          error = Wire.endTransmission();
          
          if (error == 0){    
             
            //check to ensure address is unique
            unique = true;

            //is the address already in our list?
            for (uint8_t j=1; j<jmri_data.nDevices; j++){
                if (jmri_data.boardinfo[j].addr == address){
                   JMRI_HELPER::logging(1,F("####Multiple I2C device found at address %x\n"), (int)address);  
                   JMRI_HELPER::logging(1,F("\n####Ignoring, could lead to unpredictable results, please fix..\n"));
                   unique = false;
                   break;
                }             
            }

            //i2c limit for us is 11 
            if ( jmri_data.nDevices > 12 ){
                   JMRI_HELPER::logging(1,F("Maximum number of I2C devices reached. "));
                   JMRI_HELPER::logging(1,F("Ignoring remaining boards...\n"));
                   unique = false;
                   break;             
            }

            //i2c PCF limit is 5 
            if ( jmri_data.nPCFDev > 5 ){
                   JMRI_HELPER::logging(1,F("Maximum number of PCF857x devices reached. "));
                   JMRI_HELPER::logging(1,F("Ignoring any future boards discovered...\n"));
                   unique = false;
                   break;             
            }

            //i2c PCA limit is 11
            if ( jmri_data.nPCADev > 11 ){
                   JMRI_HELPER::logging(1,F("Maximum number of PCF8575 devices reached. "));
                   JMRI_HELPER::logging(1,F("Ignoring any future boards discovered...\n"));
                   unique = false;
                   break;             
            }

            //address range for PCA9685 boards
            if (address >= 0x40 && address <= 0x7F && unique){ 

                      //search "eeprom" for existing I2C devices with same address
                      for (uint8_t j=0; j<DEVICES; j++){
                          if (address == jmri_data.data.addr[j]){
                              JMRI_HELPER::logging(1,F("I2C PCA9685 device found at address %x\n"), (int)address);  
                              JMRI_HELPER::logging(1,F("Located data in EEPROM, retrieving..."));                                                        
                              JMRI_STORE::getBoardInfo(j ,jmri_data.nDevices);                                                          
                              j=DEVICES;                            
                          } 

                          if (j == DEVICES-1){
                              JMRI_HELPER::logging(1,F("New I2C PCA9685 device found at address %x\n"), (int)address);          
                              jmri_data.devdata[jmri_data.nDevices].addr = (uint8_t)address;

                              if (JMRI_STORE::eepromCap() <= DEVICES){
                                  jmri_data.data.addr[JMRI_STORE::eepromCap()] = (uint8_t)address;
                                  jmri_data.boardinfo[jmri_data.nDevices].eepromaddr = JMRI_STORE::eepromCap();
                                  JMRI_STORE::inc_eepromCap();  //increment++
                                  JMRI_STORE::saveBoard(&jmri_data.nDevices);
                              }
                          }                        
                      }                     

                      //setup board for PWM
                      jmri_data.boardinfo[jmri_data.nDevices].pwm = Adafruit_PWMServoDriver(address);
                      jmri_data.boardinfo[jmri_data.nDevices].pwm.begin();
                      jmri_data.boardinfo[jmri_data.nDevices].pwm.setPWMFreq(FREQUENCY);

                      //save board address
                      jmri_data.boardinfo[jmri_data.nDevices].addr = (uint8_t)address;
                      jmri_data.boardinfo[jmri_data.nDevices].type = PCA9685;                      
                      jmri_data.devdata[jmri_data.nDevices].addr = (uint8_t)address;

                      JMRI_HELPER::logging(2,F("Board: %d, Type: %d\n"),jmri_data.nDevices, jmri_data.boardinfo[jmri_data.nDevices].type );                
                      jmri_data.nDevices++;
                      jmri_data.nPCADev++;
                    
            }

            //address range for PCF8575 boards
            if (address >= 0x20 && address <= 0x27 && unique){ 

                      //search "eeprom" for existing I2C devices with same address
                      for (uint8_t j=0; j<DEVICES; j++){
                          if (address == jmri_data.data.addr[j]){
                              JMRI_HELPER::logging(1,F("I2C PCF8575 device found at address %x\n"), (int)address);  
                              JMRI_HELPER::logging(1,F("Located data in EEPROM, retrieving..."));
                              JMRI_STORE::getBoardInfo(j ,jmri_data.nDevices);     
                              j=DEVICES;
                              
                          } 

                          if (j == DEVICES-1){
                              JMRI_HELPER::logging(1,F("New I2C PCF8575 device found at address %x\n"), (int)address);
                              jmri_data.devdata[jmri_data.nDevices].addr = (uint8_t)address;

                              if (JMRI_STORE::eepromCap() <= DEVICES){
                                  jmri_data.data.addr[JMRI_STORE::eepromCap()] = (uint8_t)address;
                                  jmri_data.boardinfo[jmri_data.nDevices].eepromaddr = JMRI_STORE::eepromCap();
                                  JMRI_STORE::inc_eepromCap(); 
                                  JMRI_STORE::saveBoard(&jmri_data.nDevices);
                              }                 
                          }                          
                      }

                      //set up board for I/O
                      jmri_data.boardinfo[jmri_data.nDevices].PCF.setAddress( address );
                      jmri_data.boardinfo[jmri_data.nDevices].PCF.begin();

                      //save I2C address                                        
                      jmri_data.boardinfo[jmri_data.nDevices].addr = (uint8_t)address;
                      jmri_data.boardinfo[jmri_data.nDevices].type = PCF857x;
                      jmri_data.devdata[jmri_data.nDevices].addr = (uint8_t)address;
               
                      //read sensor state of all pins
                      jmri_data.boardinfo[jmri_data.nDevices].sensorState = jmri_data.boardinfo[jmri_data.nDevices].PCF.read16();

                      //detatch interrupts for main board so new one can be attached
                      uint8_t pinInter = pintable[intrrupt[jmri_data.nPCFDev]];
                      JMRI_HELPER::logging(2,F("Board: %d, Type: %d\n"),jmri_data.nDevices, jmri_data.boardinfo[jmri_data.nDevices].type );                                                                                                      
                      if( jmri_data.devdata[0].mode  [intrrupt[jmri_data.nPCFDev]] == 'S' ) {
                        detachInterrupt(digitalPinToInterrupt(pinInter));
                      }

                      //setup interrupt 
                      pinMode(pinInter, INPUT_PULLUP);
                      attachInterruptArg(digitalPinToInterrupt(pinInter), JMRI_STORE::pinISR_PCF, jmri_data.boardinfo[jmri_data.nDevices].ptr[0], FALLING);
                      jmri_data.devdata[0].names [intrrupt[jmri_data.nPCFDev]]  =   0;
                      jmri_data.devdata[0].mode  [intrrupt[jmri_data.nPCFDev]]  =  'S';                 
                      sprintf(jmri_data.devdata[0].desc[intrrupt[jmri_data.nPCFDev]],"PCFbrd %d", jmri_data.nDevices);                 

                      jmri_data.nPCFDev++;
                      jmri_data.nDevices++;       
                                                                
            }
          }  else if (error != 4 && error != 2) {
            JMRI_HELPER::logging(1,F("Unknown error (%d) at address %x\n"), error, (int)address);
          }
          
      }
     
      if (jmri_data.nDevices <= 1){
          JMRI_HELPER::logging(1,F("No I2C deviced found...\n"));
      } else {

          if( JMRI_STORE::eepromCap() == DEVICES) {
                JMRI_STORE::save();
          }
                                             
          JMRI_HELPER::logging(1,F("\nI2C scan complete...\n"));

      }
      _i2cUpdate = false;
      JMRI_HELPER::logging(2,F("Heap size: %u\n"), ESP.getFreeHeap()); 
      
}
bool JMRI_I2C::i2cUpdate(){
      return _i2cUpdate;
}
void JMRI_I2C::set_i2cUpdate(bool state){
      _i2cUpdate = state;
}
//set pulse width based on angles set by user.
int JMRI_I2C::pulseWidth(uint8_t angle)
{
        int pulse_wide, analog_value;
        
        pulse_wide   = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
        analog_value = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);
        
        return analog_value;
}

// called when interrupt is called.
// at this point we do not know which 
// pin has been changed or its state
void JMRI_I2C::PCF_Change(uint8_t pcf_board){

        //char newstate = 'A';
        uint8_t wbit;
        uint16_t x;


        //JMRI_HELPER::logging(2,F("Interrupt Detected....\n"));  
        //delay(interval);

        //get the last state from memory of all 
        //the pins on the board which intiated the interrupt


        //get the current state of all the pins from the board that intiated the interrupt
        x = jmri_data.boardinfo[pcf_board].PCF.read16();     
        //determin which bit has change using xor with the last state                                           
        wbit = __builtin_ctz(jmri_data.boardinfo[pcf_board].sensorState ^ x); 

        //try again... if the first attempt failed (i.e. implied no change)
        // pin value should be should <=15. 255 = no change or error!
        if (wbit > 15 ) {
                x = jmri_data.boardinfo[pcf_board].PCF.read16();                                               
                wbit = __builtin_ctz(jmri_data.boardinfo[pcf_board].sensorState ^ x); 
        }

        if (jmri_data.data.loglvl == 2){;
            char binary[17];
            itoa(x,binary,2);
            JMRI_HELPER::logging(2,F("Board: %d, Pin: %d, Val: %d, BIN: %s\n"), pcf_board, wbit, 1 & (x >> wbit), binary );
        }
        
        //no change! do nothing...
        if (wbit > 15){
              jmri_data.previousMillis = millis();       
              JMRI_HELPER::logging(1,F("Problem reading pin change...!\n"));
              return;
        }

        //save new state to memory
        jmri_data.boardinfo[pcf_board].sensorState = x;

        if (jmri_data.devdata[pcf_board].mode[wbit] == 'S' && wbit <=15 ){

              Inter inter;
              inter.a = pcf_board; //board
              inter.b = wbit; //pin
              inter.c = 'A';
              
              //get value of bit thats change
              //if bit is == 1 newstate is INACTIVE
              if ( 1 & (x >> wbit) ) {
                    inter.c = 'I';            
              } 
              
              //update webpage and send MQTT message
              jmri_data.inter.push(inter);
              //JMRI_HELPER::changeSensor( &pcf_board, &wbit, &newstate );                     
                   
        }
        
        jmri_data.previousMillis = millis();     

}
