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
 
#ifndef HELPER_h
#define HELPER_h

#include "config.h"
#include "headers.h"

extern jmriData jmri_data;

class JMRI_HELPER {

  public:

        static void     pollQueue       ();
        static void     setTurnout      (ushort sysname, char* message);
        static void     setLight        (ushort sysname, char* message);
        static void     setRadioButton  (uint8_t* board, uint8_t* pinnum, char bmode );
        static void     flipDot         (uint8_t* board, uint8_t* pinnum, const char* state );
        static void     setDot          (uint8_t* board, uint8_t* pinnum, char* state );
        static void     changeSensor    (uint8_t* board, uint8_t* pinnum, char* newstate);
        static void     setPWMCheck     (uint8_t* board, uint8_t* pinnum, bool* newstate);  
        static void     logging         (uint8_t lvl, const __FlashStringHelper *format, ... );
        static void     _p              (uint8_t lvl, const __FlashStringHelper *format, va_list args );

  private:

  
};

      
  
#endif
