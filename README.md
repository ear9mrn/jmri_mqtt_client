JMRI Accessory MQTT Client

Introduction:
JMRI Accessory MQTT client is a web client based on the ESP8266 development board.


![image](https://user-images.githubusercontent.com/471260/155894545-6a74d38a-c0e5-4213-81d9-4a599226e5e5.png)


Key benefits:
	-Fully web based configuration, no coding required
	-Connection to JMRI is via WiFi enabling multiple clients
	-Supports both input and output (turn-outs/lights/sensors) 
	-Plug-n-play additional I/O boards such as PCF857x and PCA9685
	-JMRI already fully supports MQTT


How does it work?
For the example of activating a turnout. Within JRMI when a turnout is thrown, JMRI sends an MQTT message to the broker. The message looks something like this  “/trains/track/turnout/44 THROWN”. The JMRI accessory client will “see” the message and respond to the message by throwing the turnout.

For the example of a sensor it works in the opposite manner. The JMRI accessory client response to any changes in sensor state by sending an MQTT message to the broker. JMRI “sees” this message and update the sensor state within JMRI as appropriate.

Below is a simple MQTT block diagram. The MQTT broker acts as a middle man between JMRI and any clients connected to the same broker. There is no limit to the number of JMRI MQTT clients (IP addresses). 

![block](https://user-images.githubusercontent.com/471260/155901574-b8f28dc8-12a8-4d75-8083-562dd6f77d41.png)


![image](https://user-images.githubusercontent.com/471260/155894597-63b46d57-ea2e-4219-ae96-281ce6b25830.png)
       
       
Simple broker setup example.

    1. Enable MQTT within JMRI:
       Under File->preferences->Connections add a new tab and select MQTT under “System manufacturer” and “MQTT Connection” under System connections. Then set the IP address or hostname of the machine that is/will run the MQTT broker. In my case JMRI and mosquitto (Linux MQTT server) are running on the same machine.
       Leave the last two parameters as default.

    2. Set up a MQTT broker:
       I am running Ubuntu and to install and run mosquitto MQTT broker is very simple with three commands. 
       
       “sudo apt install mosquitto”
       “sudo systemctl enable mosquitto”
       “sudo systemctl start mosquitto”
       
       	Now whenever the machine is running mosquitto is running. No further configuration is required.
       
    3. Setup and configure ESP8266 Dev board as a JMRI accessory MQTT client:

	*Note: there a multiple versions of the ESP8266 dev board and pin numbering can vary from board to board. 
	
In this example we will connect one servo
       

![esp8266_serov](https://user-images.githubusercontent.com/471260/155894646-a6245006-ab70-4377-8d93-c290d3aeb227.png)


Connect the ESP8266 to your PC using a USB cable. Download and upload the JMRI Accessory MQTT client script from the following github repository https://github.com/ear9mrn/jmri_mqtt_client.  Once installed power cycle the board, watch the serial output to see if things are working correctly, next step instructions are also provided there. 


![image](https://user-images.githubusercontent.com/471260/155894663-4467aba4-fe3b-4a06-bea7-72d99670e2c2.png)

The easiest way to connect to your local WiFi is to edit the credentials.h file adding your SSID and password.

#define   DEFAULTSSID              "ssid"
#define   DEFAULTPASSWORD          "pass"

Alternatively follow the next steps.

Next step is to connect the ESP8266 to your local WiFi. During the initiation process the ESP8266 will create its own access point with an SSID that looks something like “JMRI-ACC-E8:DB:84:E0:A7:96”. 

The simplest method is to connect to this access point with a smart phone (no password is required). Then enter the following IP address (192.168.4.1) into a browser on your smartphone. You should be presented with a list of visible local access points enter the name of the access point you wish to connect to (needs to be the same network as the MQTT broker) and the password, hit submit.

If you have the serial monitor open you should see output indicating that a WiFi connection is established.


![image](https://user-images.githubusercontent.com/471260/155894694-8557c8ca-02e6-4717-9a3b-be889d5102f4.png)

![image](https://user-images.githubusercontent.com/471260/155894700-55b98fca-09ae-47e1-9df1-418d9cebbc50.png)


You should now be able to navigate with any browser on the same network to the IP address that was assigned to your ESP8266 device (see serial monitor output), in this example this is 192.168.1.144. When you do this (and everything has worked correctly) you should be presented with the client configuration page. 

![image](https://user-images.githubusercontent.com/471260/155894708-4b0294b8-cd46-4b1f-9ad4-c0360d1f66d3.png)



First set the IP address of the MQTT broker you are running (same IP address set in the JMRI setup) hit update.

*Note: all parameters (enter/updated) are saved to the ESP8266 EPROM and so will survive a reboot or power cycle. 

As a quick test to see if everything is working enter a system number for pin zero (value between 1-65535, zero means not used), in my example I have use 44.

![image](https://user-images.githubusercontent.com/471260/155894713-6c687deb-d5e5-49fa-853e-73b2c60c05f3.png)


Add a new turnout (under MQTT tab) with the same system name value.

![image](https://user-images.githubusercontent.com/471260/155894727-b367e6ce-5e6f-40da-8608-85cad2dc693b.png)


If you click on state button changing CLOSED/THROWN you should see the colour of the state dot change in the web interface of the same system name. You can also click the state dot (red/green) on the web interface and this should change the state of the turnout within JMRI. If you have the servo connected as shown above this should also move.


![turnout_test](https://user-images.githubusercontent.com/471260/155894751-97619c64-0eff-43c8-a999-c51617412d3f.png)



Create a sensor:

As a quick test to test a sensor enter a system number for pin one (value between 1-65535, zero means not used), in my example I have use 77 and change the mode of the pin to sensor by selecting the radio button for sensor.


![image](https://user-images.githubusercontent.com/471260/155894760-4cb9752a-18e9-4636-8d57-02e9bd6b03b5.png)


Add a new sensor (under MQTT tab) with the same system name value.

![image](https://user-images.githubusercontent.com/471260/155894770-2e6aa8d8-b6b2-4852-8f66-7aed672a2705.png)



Now connect pin 1 (D1) to ground and you should see the colour of the state dot turn red and the status in the JMRI sensor table change to Inactive.


![sensor_test](https://user-images.githubusercontent.com/471260/155894797-4cb98d75-e2f1-4f1e-874f-bc3ec9a27b89.png)


ESP8266 Expansion Board

An option I have found to be helpful is to use expansion board. This provides an I/O pin with an associated GND and 5V pin that enables a three pin servo to be attached directly without additional wiring and keeps things tidy.

![image](https://user-images.githubusercontent.com/471260/155898758-2e3b4d0d-ab7c-4696-9544-2ffb45867cc6.png)


JMRI Accessory MQTT Client expansion

JMRI Accessory MQTT Client allows for the addition of expansion boards for additional I/O (using I2C protocol). With the latest version up to 5 PCF8575 boards can be attached and 12 PCA9685 (output only) with the total number of attached boards not to exceed 12 (due to EPROM and memory limitations of the ESP8266). This gives a total of 192 additional pins. This is plug n play, when attached they are self configured and an additional tab should appear on the web configuration. 


![boards](https://user-images.githubusercontent.com/471260/155901126-66b8ea85-e729-4a6a-a733-72d700468326.png)

SDA is attached to SD2 (pin GPIO10) and SCL is attached to SD3 (pin GPIO11) of the ESP8266. I2C boards can be daisy chained so any number of boards can be attached to the same line. Only caveat is all boards have to have unique I2C addresses for this to work.

![sda_scl](https://user-images.githubusercontent.com/471260/155901158-9752652a-98f1-4a4c-b685-317c9e31f69c.png)


I2C addresses:
	PCA9685: 0x40 to 0x7F
	PCF8575: 0x20 to 0x27

JMRI Accessory MQTT Client uses these addresses to identify which boards are attached and automatically configures itself accordingly. You only have to connect the SDA and SCL pins to the correct pins on the ESP8266 (GND and 5V separately) and the board will automatically appear in the web configuration.

*Warning: Do not try to power any attached boards or servos through the ESP8266 power as it cannot provide the amps required to drive them.  


Select the appropriate tab to configure the attached boards. The board name can be edited to help identify them. In this example I have 4 PCA9685 boards attached (these are output only boards so sensor option is disabled). As stated before, all configuration is saved to the EPROM and will survive a reboot, power cycle.


![image](https://user-images.githubusercontent.com/471260/155901179-f19aa8a4-1eef-453f-bed1-f3551a05a306.png)


Connecting  PCF8575 boards.

As these are I/O boards an additional interrupt line has to be attached to the ESP8266.  Pins 1,2,5,6,7 can be used as interrupt pins (this why we are limited to 5 of these boards). 


![interrupt](https://user-images.githubusercontent.com/471260/155902443-ecf75062-2d3a-4e4b-9d40-888d6cd679eb.png)


The SDA and SCL are connected as normal (and 5V and GND to external supply). But in addition the interrupt (INT) is connected to D1.  D1 will automatically be disable for use in the configuration if an I/O board is attached.

It is important that the I/O interrupts are connected to the right pin on the EPS8266 as they are assigned in numerical order of increasing I2C address. Pins 1,2,5,6,7 are used, pin1 first board (lowest I2C address), pin 2 for the next highest I2C address etc. The pins are only set to interrupt if boards are connected otherwise they can be used as standard I/O pins. 


