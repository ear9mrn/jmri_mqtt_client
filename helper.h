
#ifndef HELPER_h
#define HELPER_h

#include "config.h"

struct JMRI_HELPER {

        static JMRI_HELPER *jmri_help;
        jmriData        *jmri_ptr;
        static void     help_init       (jmriData *jmri_data);
        static void     setTurnout      (ushort sysname, char* message);
        static void     setLight        (ushort sysname, char* message);
        static void     setRadioButton  (uint8_t* board, uint8_t* pinnum, char bmode );
        static void     flipDot         (uint8_t* board, uint8_t* pinnum, const char* state );
        static void     setDot          (uint8_t* board, uint8_t* pinnum, char* state );
        static void     changeSensor    (uint8_t* board, uint8_t* pinnum, char* newstate);
        static void     setPWMCheck     (uint8_t* board, uint8_t* pinnum, bool* newstate);  
        static void     logging         (uint8_t lvl, const char *format, ...); 
    
        static int      helpAddress;
        static int      pointer();
  
};

#endif
