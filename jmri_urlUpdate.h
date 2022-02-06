#ifndef URLUPDATE_H
#define URLUPDATE_H

//#include <ESP8266httpUpdate.h>
#include <ESP8266WiFi.h>
#include "newconfig.h"
#include "config.h"

      void urlupdate(jmriData *jmri_data);
      void update_started();
      void update_finished();
      void update_progress(unsigned int progress, unsigned int total);
      void update_error(int err);
      extern uint8_t nextval;
     
#endif
