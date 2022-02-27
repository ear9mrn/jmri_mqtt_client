/*
 *  © 2022 Pete Nevill
 *  All rights reserved.
 *
 *  This file is part of JMRI MQTT Accesssory Client
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */
 
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
