#ifndef JMRI_STORE_h
#define JMRI_STORE_h

#include "config.h"
#include "newconfig.h"
#include  <EEPROM.h>
#include <Servo.h>

class JMRI_STORE{

  public:
  
        static void     store_init(jmriData *jmri_dat);
        static void     save             ();
        static void     configeeprom     ();
        static void     getBoardInfo(uint8_t loc, uint8_t devloc); 
        static void     getBaseInfo();
        static void     saveBoard(uint8_t *pos);
        static void     saveConfig();
        static void     configSetup();
        static bool     eepromUpdate();
        //static void     set_eepromUpdate(bool updt_eerpom);
        static void     resetBoardinfo(uint8_t startVal=0);
        static void IRAM_ATTR pinISR_PCF(void * myarg);
        static void IRAM_ATTR pinISR(void * myarg);
        static uint8_t  eepromCap();
        static void     inc_eepromCap();

    private:
    
        static bool     _eepromUpdate;
        static uint8_t  _eepromCap;
        static jmriData  *jmri_ptr;
};

#endif
