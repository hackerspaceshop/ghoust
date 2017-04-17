/*
 August, 10   2016 - overflo
 complete rewrite for ESP8266 

 March 2017, finally found time to work on this again.
 Thanks to fbr's motivation and software support on the raspberry pi side.


needs the following libraries to compile

Arduino (1.8.1)
EP8266WiFi 1.0 
PubSubClient for MQTT support

SparkFun_MMA8452Q (https://github.com/sparkfun/SparkFun_MMA8452Q_Arduino_Library)
OneButton Library (https://github.com/mathertel/OneButton)

WiFiManager         //https://github.com/tzapu/WiFiManager
ArduinoJson       //https://github.com/bblanchon/ArduinoJson


///////////// REMARKS

The whole sound implementation relies on delay() and should tehrefor not be used DURING a game but only on init and end of a game when there are no sensor values to be read.



BUGS: 

Sometimes the accelerometer wont initialize.
The whole thing just hangs in there and does nothing.
This seems to be a hardware problem on the breakoutboard.
wontfix :(



//TODOS


-- read battery values and switch off if battery is too low

- einen MQTT tree definieren und im wiki dokumentieren damit ich auf die
entsprechenden resourcen listen kann

-bessere vibrationspattern finden

-- alles timer basiert machen statt delay






-vernuenftigere werte fuer shock thresholds finden bzw den algorithmus
anders umsetzen.
die werte wurden im alten code einfach addiert.
ich hab da wieder ein wert*wert reingenommen um negative
beschleunigungswerte auszugleichen.
lustigerweise war dazu grad was bei hackaday:
http://hackaday.com/2017/04/13/say-it-with-me-root-mean-square/



*/


#include <ESP8266WiFi.h>
#include <Ticker.h>




// FOR MQTT but also used in wifiManager setup
//define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40];
char mqtt_port[5] = "1883";


char sysname[13] = {};



void setup()
{
  
  
    
  Serial.begin(115200);
  
  
  Serial.println("");
  Serial.println("<<< START >>>");

  battery_setup();
  
  createUniqueSystemName();

  motor_setup();   // ok

  button_setup(); //ok
  
  sound_setup(); //ok

  leds_setup();

  wifi_setup();
  
  mqtt_setup();


  // setup the accelerometer
 // motion_setup();   // this breaks sometimes.. why? seems to be a hardware issue with the breakoutboard after several reeboots. usually everything works again after switching off the device for some time (over night)

  
  
}





void loop()
{

 battery_check();

  
 mqtt_work();
 button_work();
 leds_work();

 // maybe if we switch to asynchronous sound playback..
 //sound_work();
 
 // printOrientation();
//  printAcceleration();


 // shock_detect();

} 

