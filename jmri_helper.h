
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
