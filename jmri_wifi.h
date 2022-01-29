
#ifndef JMRI_WIFI_h
#define JMRI_WIFI_h

#include  "config.h"

struct JMRI_WIFI{

          static JMRI_WIFI  *jmri_wifi;
          jmriData          *jmri_ptr;
          static void       wifi_init(jmriData *jmri_data);
          static void       setupAP();
          static void       scan();
          static int        wifiAddress;
          static void       reset();
          static int        pointer();
  
};

#endif
