/*
 August, 10   2016 - overflo
 complete rewrite for ESP8266 

 March 2017, finally found time to work on this again.
 Thanks to fbr's motivation and software support on the raspberry pi side.

 TODO:
  switch to WiFiManager
  switch to software config for MQTT broker ip
  implement accelerometer protocols
  implement GAME logic

needs the following libraries to compile

Arduino (1.8.1)
EP8266WiFi 1.0 
PubSubClient for MQTT support


SparkFun_MMA8452Q (https://github.com/sparkfun/SparkFun_MMA8452Q_Arduino_Library)
OneButton Library (https://github.com/mathertel/OneButton)

WiFiManager ?


*/


#include <ESP8266WiFi.h>


// FOR MQTT but also used in wifi
//define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40];
char mqtt_port[6] = "1883";






char sysname[13] = {};



void setup()
{
  
  
    
  Serial.begin(115200);
  
  
  Serial.println("");
  Serial.println("<<< START >>>");

  createUniqueSystemName();


  motor_setup();   // ok

  button_setup(); //ok
  
  sound_setup(); //ok

  // setup the accelerometer
  motion_setup();   // this breaks sometimes.. why? breadboard issue?!


  leds_setup();

   wifi_setup();
  
   mqtt_setup();


  
}

void loop()
{
  
  mqtt_work();
  button_work();
  
  printOrientation();
    
  
} 

