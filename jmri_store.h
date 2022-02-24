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
