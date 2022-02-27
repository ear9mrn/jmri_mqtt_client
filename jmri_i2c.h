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
 
#ifndef JMRI_I2C_h
#define JMRI_I2C_h

#include "config.h"
#include "headers.h"
#include  <EEPROM.h>

extern jmriData jmri_data;

class JMRI_I2C {

    public:
    
          static void     i2c_init();
          static int      pulseWidth(uint8_t angle);
          static void     PCF_Change(uint8_t pcf_board);
          static bool     i2cUpdate();
          static void     set_i2cUpdate(bool state);
  
    private:

          static bool     _i2cUpdate;
};          
#endif
