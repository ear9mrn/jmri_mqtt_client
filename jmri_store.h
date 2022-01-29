#ifndef JMRI_STORE_h
#define JMRI_STORE_h

#include  "config.h"
#include <Servo.h>

struct JMRI_STORE{
  
        static JMRI_STORE  *jmri_store;
        jmriData        *jmri_ptr;
        static void     store_init(jmriData *jmri_dat);
        static void     save             ();
        static void     configeeprom     ();
        static int      storeAddress;
        static void     reset();
        static int      pointer();
        static void     getBoardInfo(uint8_t loc, uint8_t devloc); 
        static void     getBaseInfo();
        static void     saveBoard(uint8_t *pos);
        static void     saveConfig();
        static void     configSetup();
        static void     resetBoardinfo(uint8_t startVal=0);
        //static ICACHE_RAM_ATTR  void pinISR(void * myarg);
        //static ICACHE_RAM_ATTR  void pinISR_PCF(void * myarg);
        static void IRAM_ATTR pinISR_PCF(void * myarg);
        static void IRAM_ATTR pinISR(void * myarg);

};

#endif
