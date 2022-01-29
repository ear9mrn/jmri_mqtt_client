
#ifndef TELNET_h
#define TELNET_h
  
#include "config.h"

struct JMRI_TEL {

        static JMRI_TEL *jmri_tel;
        jmriData        *jmri_ptr;
        static void     tel_init       (jmriData *jmri_data);
        static void     telPrint(char *text);
        static void     clientConnected();
        static int      telAddress;
        static int      pointer();
         
};

#endif
