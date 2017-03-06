/*
 August, 10   2016 - overflo
 complete rewrite for ESP8266 









*/


#include <ESP8266WiFi.h>




char sysname[12] = {};



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
 // motion_setup();   


  leds_setup();

   wifi_setup();
  
   mqtt_setup();

//  leds_all(0,1023,0);

  
  
}

void loop()
{
  
  mqtt_work();
  button_work();
  
  //printOrientation();
    
  
} 

