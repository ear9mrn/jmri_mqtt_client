
#ifndef I2C_h
#define I2C_h

#include "config.h"

struct JMRI_I2C {

          static JMRI_I2C *jmri_i2c;
          jmriData        *jmri_ptr;
          static void     i2c_init(jmriData *jmri_data);
          static void     sortStorage();
          static int      pulseWidth(uint8_t angle);
          static void     PCF_Change();
          static int      i2cAddress;
          static int      pointer();
  
};

#endif
