
#include "jmri_template.h"

void JMRI_HTML::html_init() {

        LittleFS.begin();

        //check if index.html exist in SPIFFS (LittleFS), if not create it.
        //see htmltemplate.h for the content of index.html
        if (LittleFS.exists("/indexx.html") ) {
            JMRI_HELPER::logging(2,F("HTML file present..."));
        } else {   

              JMRI_HELPER::logging(1,F("Attempting to format SPIFFS...\n"));
               
              if(LittleFS.format()){
                      JMRI_HELPER::logging(1,F("File System Formatted...\n"));
              } else {
                      JMRI_HELPER::logging(1,F("File System Formatting Error...\n"));
              }
              
              File f = LittleFS.open("/index.html", "w");   
              //copy content of the PROGMEM var to SPIFFS(LittleFS) file  
              f.printf_P(indexhtml);
              JMRI_HELPER::logging(2,F("index.html file created with size %d bytes\n"), f.size());
              f.close();
        }
      
        
        //check if style.css exist in SPIFFS(LittleFS), if not create it.
        //see style.css for the content of index.html
        if (LittleFS.exists("/jmri_js.js") ) {
              JMRI_HELPER::logging(2,F("jmri_js.js file present...\n"));
        } else {   
              
              File fc = LittleFS.open("/jmri_js.js", "w");
              //copy content of the PROGMEM var to SPIFFS(LittleFS) file       
              fc.printf_P(jscript);        
              JMRI_HELPER::logging(2,F("javascript file created with size %d bytes\n"), fc.size());
              fc.close();
           
        }

       //check if logbox.html exist in SPIFFS(LittleFS), if not create it.
        //see style.css for the content of index.html
        if (LittleFS.exists("/loggingx.html") ) {
              JMRI_HELPER::logging(2,F("logbox.html file present...\n"));
        } else {   
              
              File fc = LittleFS.open("/logging.html", "w");
              //copy content of the PROGMEM var to SPIFFS(LittleFS) file       
              fc.printf_P(logbox);          
              JMRI_HELPER::logging(2,F("logging.html file created with size %d bytes\n"), fc.size());
              fc.close();
           
        }
        
        //check if style.css exist in SPIFFS(LittleFS), if not create it.
        //see style.css for the content of index.html
        if (LittleFS.exists("/style.css") ) {
              JMRI_HELPER::logging(2,F("style.css file present...\n"));
        } else {   
              
              File fc = LittleFS.open("/style.css", "w");
              //copy content of the PROGMEM var to SPIFFS(LittleFS) file       
              fc.printf_P(stylecss);         
              JMRI_HELPER::logging(2,F("CSS file created with size %d bytes\n"), fc.size());
              fc.close();
           
        }
        
        JMRI_HELPER::logging(2,F("Heap size: %d\n"),ESP.getFreeHeap() );
         
}
