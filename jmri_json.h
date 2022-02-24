#ifndef JSONLIB2_H
#define JSONLIB2_H

#include <cstring>
#include "headers.h"

      //String jsonRemoveWhiteSpace(const String& json);
      void jsonExtract(char *json, const char *nameArg, char **result);
      void jsonIndexList(char * json, const char* idxname, uint8_t idx, char **result);
      void jsonIndexList(char * json, const char* idxname, const char* idxname2, uint8_t idx, char **result); 
      void jsonIndexList(char * json, const char* idxname, uint8_t idxname2_no, uint8_t idx, char **result); 
     
#endif
