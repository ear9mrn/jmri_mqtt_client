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
 
#ifndef JMRI_STORE_h
#define JMRI_STORE_h

#include "config.h"
#include "headers.h"
#include  <EEPROM.h>
#include <Servo.h>

extern jmriData jmri_data;

class JMRI_STORE{

  public:
  
        static void     store_init();
        static void     save             ();
        static void     configeeprom     ();
        static void     getBoardInfo(uint8_t loc, uint8_t devloc); 
        static void     getBaseInfo();
        static void     saveBoard(uint8_t *pos);
        static void     saveConfig();
        static void     configSetup();
        static bool     eepromUpdate();
        static void     resetBoardinfo(uint8_t startVal=0);
        static void     IRAM_ATTR pinISR_PCF(void * myarg);
        static void     IRAM_ATTR pinISR(void * myarg);
        static uint8_t  eepromCap();
        static void     inc_eepromCap();

    private:
    
        static bool     _eepromUpdate;
        static uint8_t  _eepromCap;
   
};

#endif
