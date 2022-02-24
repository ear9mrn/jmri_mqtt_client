#ifndef URLUPDATE_H
#define URLUPDATE_H

#include "headers.h"
#include "config.h"

      extern jmriData jmri_data;
      void urlupdate();
      void update_started();
      void update_finished();
      void update_progress(unsigned int progress, unsigned int total);
      void update_error(int err);
      extern uint8_t nextval;
     
#endif
