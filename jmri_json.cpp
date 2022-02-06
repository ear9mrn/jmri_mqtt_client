
#include "jmri_json.h"


void jsonIndexList(char * json,  const char* idxname, uint8_t idx, char **result){

      char *idx_result, *int_result;
      idx_result=(char *) malloc(10);
      uint8_t cnt = 0, outlen=0, idx_move=0;;    
      jsonExtract(json, idxname, &idx_result);
      int_result = idx_result;
      free(*result);     
      
      while ( *idx_result != '\0') {
        
          if ( *idx_result == ',' && cnt == idx){
              break;
          } else {
              if (*idx_result == ',' ) {
               
                  idx_result++;
                  idx_move++;
                  cnt++;
                  outlen=0;
                  int_result = idx_result;
              }
                  outlen++;
                  idx_result++;
                  idx_move++;
          }         
      }      

      if (idx > cnt){
          *result=(char *) malloc(10);
          sprintf(*result, "Undefined");
      } else {
          if (*int_result == '\"'){
            int_result++;
            outlen=outlen-2;
          }
          *result=(char *) malloc(outlen+1);
          sprintf(*result, "%.*s", outlen, int_result);
      }

      idx_result = (idx_result) - idx_move;        
      free(idx_result);
      
}

void jsonExtract(char *json, const char *nameArg, char **result){

      char *output, *int_result;
      uint16_t len = strlen(json), outlen=0;
      char mod[strlen(nameArg)+4];
      sprintf(mod,"\"%s\":",nameArg);
      
      output = strstr (json, mod);
      free(*result);
       
      if (output) {

             output = output + 3 + strlen(nameArg);
             
             char term;
             if (*output == '\"') {
                  term = '\"';
                  output++;
             } else if (*output == '{'){
                  term = '}';
                  output++;
             } else if (*output == '['){
                  term = ']';
                  output++;
             } else {
                  term = ',';
             }
             
             int_result = &(json[output - json]);
                                
             for(uint16_t j=0; j<len; j++){
                  if ( *output == term){
                      j=len;
                  } else {
                      output++;
                      outlen++;
                  }
             }
             

             *result=(char *) malloc(outlen+1);
             sprintf(*result, "%.*s", outlen, int_result);
             
      } else {
        
             *result=(char *) malloc(10);
             sprintf(*result, "Undefined");                       
      }
}

void jsonIndexList(char * json,  const char* idxname,   const char* idxname2, uint8_t idx, char **result) {

      char *result2;
      result2=(char *) malloc(10);
      jsonExtract(json, idxname, &result2); 
      jsonIndexList(result2, idxname2, idx,  result);       
      free(result2);          
  
}

void jsonIndexList(char * json,  const char* idxname,  uint8_t idxname2_no, uint8_t idx, char **result) {
     
      char *result2;
      result2=(char *) malloc(10);
      jsonExtract(json, idxname, &result2); 
      char idxname2[10];
      sprintf(idxname2, "%d", idxname2_no);
      jsonIndexList(result2, idxname2, idx,  result);       
      free(result2);         
  
}


// remove all white space from the json string... preserving strings
//String jsonRemoveWhiteSpace(const String& json){
//  int i = 0;
//  int cursor = 0;
//  int quote_count = 0;
//  String out = String();
//  char out_chars[json.length()+1];
//  
//  for(i=0; i<json.length(); i++){
//    if(json[i] == ' ' || json[i] == '\n' || json[i] == '\t' || json[i] == '\r'){
//      if(quote_count % 2){ // inside a string
//  out_chars[cursor++] = json[i];
//      }
//      else{ // outside a string!
//      }
//    }
//    else{
//      if(json[i] == 34){ // ascii dounble quote
//  //check for escaped quote
//  if(i > 0 && json[i - 1] == '\\'){
//    //escaped!
//  }
//  else{ // not escaped
//    quote_count++;
//  }
//      }
//      out_chars[cursor++] = json[i];
//    }
//  }
//  out_chars[cursor] = 0;
//  out = String(out_chars);
//  return out;
//}
