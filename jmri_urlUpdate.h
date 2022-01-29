#ifndef URLUPDATE_H
#define URLUPDATE_H

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>


      void urlupdate();
      void update_started();
      void update_finished();
      void update_progress(int cur, int total);
      void update_error(int err);

     
#endif
