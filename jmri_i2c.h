
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
