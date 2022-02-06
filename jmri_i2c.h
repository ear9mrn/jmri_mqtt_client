
#ifndef JMRI_I2C_h
#define JMRI_I2C_h

#include "config.h"
#include "newconfig.h"
#include  <EEPROM.h>

class JMRI_I2C {

    public:
    
          static void     i2c_init(jmriData *jmri_data);
          static int      pulseWidth(uint8_t angle);
          static void     PCF_Change(jmriData *jmri_data);
          static bool     i2cUpdate();
          static void     set_i2cUpdate(bool state);
  
    private:

          static bool     _i2cUpdate;
};          
#endif
