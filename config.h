#ifndef JMRI_CONFIG_h
#define JMRI_CONFIG_h

#include <Adafruit_PWMServoDriver.h>
#include <Servo.h>
#include <PCF8575.h>

//either edit credientials.h or #define below DEFAULTPASSWORD and DEFAULTSSID.
#include "credentials.h"                                  

#define   VERSION             0.10
#define   DEFAULTMQTTSERVER   "192.168.1.45"              //default MQTT broker ip address
#define   DEFAULTMQTTPORT     1883                        //default MQTT broker port
#define   DEFAULTMQTTTOPIC    "/trains/track/#"           //default sensor topic
#ifndef   DEFAULTSSID
  #define   DEFAULTSSID         "ssid"                    //default SSID if not defined in credentials.h
#endif
#ifndef DEFAULTPASSWORD 
  #define   DEFAULTPASSWORD     "pass"                    //default wifi password if not defined in credentials.h
#endif
#define   PINS                9                           //number of pins available on base board (ESP8266)
#define   I2C_PINS            16                          //number of pins available on I2C devices
#define   DEVICES             12                          //maximum number of devices to manage (limit is due to JSON buffer size in memory!)
#define   WIFITIMEOUT         20                          //wifi timout (seconds)
#define   SDA                 10                          //I2C SDA pin
#define   SCL                 9                           //I2C SCL pin


//board identifier
//#define   ESP8266             1           //our base board (already defined by IDE!) 
#define   PCA9685             2           //i2c address range 0x40 to 0x47  
#define   PCF857x             3           //i2c address range 0x20 to 0x27
#define   UNDEF               255

//pwm servo parameters 
#define   MIN_PULSE_WIDTH       500       //these are the default value as per the library 
#define   MAX_PULSE_WIDTH       2500      //and seem to be reasonable
#define   DEFAULT_PULSE_WIDTH   1500
#define   FREQUENCY             50

#define   EESTORE_ID "JMRI"               //first 4 charaters of eeprom to determin if store exists
#define   UNUSED(x) (void)(x)

/* pin mapping ESP8266
 * D0 -> 16 (GPO) No Interrupt (No input)
 * D1 -> 5  (GPIO & PWM)
 * D2 -> 4  (GPIO & PWM)  
 * D3 -> 0  (GPO & PWM) (No input)
 * D4 -> 2  (GPO & PWM) (No input)
 * D5 -> 14 (GPIO & PWM) 
 * D6 -> 12 (GPIO & PWM)
 * D7 -> 13 (GPIO & PWM)
 * D8 -> 15 (GPO & PWM) (No input)
 * I2C 
 * (PCF8574) 0-7  (GPIO - no PWM)
 * (PCF8575) 0-15 (GPIO - no PWM)
 * (PCA9685) 0-15 (Output only PWM+digital)
 */
 
//map break out board pin labels to ESP8266 pins d0-d8
const int       pintable[]  =   {16,5,4,0,2,14,12,13,15}; 
const long      interval    =   19;             //for debounce of interrupts (ms) 
const uint8_t   s_only[]    =   {0,3,4,8};      //these pins cannot be used as inputs on ESP8266!
const uint8_t   intrrupt[]  =   {1,2,5,6,7};    //ESP8266 interrupt pins!


//This is the main configuration data that is saved to the EEPROM.
struct EEStoreData{
  
        char    id[sizeof(EESTORE_ID)+1]; 
        char    mqtt_server_host[16];              
        ushort  mqtt_server_port;
        char    mqtt_topic [20];
        char    ssid[33];
        char    pass[65];
        uint8_t i2c_addr[DEVICES];   
        uint8_t loglvl;                   //0=none,1=info,2=verbose;
        
};

//board information saved to eeprom. maximum 11 additional boards
struct EEStoreDevData{   
          
        uint8_t i2c_addr;                     //I2C address (has to be unique)
        char    bdesc      [21];              //name or description for each board
        ushort  i2c_names  [I2C_PINS];        //JMRI sysname 0-65535
        char    i2c_mode   [I2C_PINS];        //pin mode ('T'==turnout,'L'==light,'S'==sensor)
        char    i2c_state  [I2C_PINS];        //pin state. Remember last state of light or turnout 'T'==thrown,'C',closed,'N'==on,'F'==off
        bool    i2c_pwm    [I2C_PINS];        //Is turnout digital (O/1) or PWM.
        uint8_t lang       [I2C_PINS];        //For PWM low angle of servo.
        uint8_t hang       [I2C_PINS];        //For PWM high angle of servo.
        char    desc       [I2C_PINS][11];    //Brief description of attached device 10 char limit.
        
};
  

//a struct with I2C board information created and used at run time.
struct binfo { 

        uint8_t   eepromaddr;                 //address of board info in EEPROM
        uint8_t   i2caddr;                    //I2C address (has to be unique)
        uint8_t   i2ctype;                    //board type
        uint8_t   interrupt[I2C_PINS][2];     //interrupt contains board and pin info that is sent to the interrupt.
        void      *ptr[I2C_PINS];             //interrupt pointer to info above
        uint16_t  sensorState = 0;            //Sensor state
        Adafruit_PWMServoDriver pwm;          //PCA9685 pwm object for this board if appropriate
        PCF8575                 PCF;          //PCF8575 object for this board if appropriate
        
} ;    

struct jmriData{ 
                  
        EEStoreData   		data;                       //main config struct
        EEStoreDevData    devdata[DEVICES];           //devices config struct
        binfo             boardinfo[DEVICES];         //array of structs containting all board info
                        
	      volatile unsigned long 	previousMillis = 0;   //debounce for interrupt
        volatile bool     pcfChange = false;          //bool that indicates interrupt occured
        volatile uint8_t  pcfBChange = 0;             //board that triggered interrupt
        
        unsigned long 		retainMSGTimer = 0;         //delay for MQTT message retrieve at boot time!
        uint8_t       		nDevices = 0;               //total number of devices including main ESP8266
        uint8_t       		nPCADev  = 0;               //number of PCA devices attached
        uint8_t       		nPCFDev  = 0;               //number of PCF devices attached      
                         
        Servo             servos[PINS];               //array for attached servos to main ESP8266
        volatile bool     bootComplete = false;       //boot complete flag (block input until boot completed)
        float             urlUpdate = 0.0;
        float             jrmi_mqtt_v_latest = 99.0;  //get the latest version number from github

};


#endif
