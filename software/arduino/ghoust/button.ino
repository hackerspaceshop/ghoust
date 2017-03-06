
// BELONGS TO BUTTON//

#include <OneButton.h>

#define BUTTON_PIN    2 //D4  GPIO2 (builtin led)
OneButton button(BUTTON_PIN, true);

//////////////////////



void button_setup()
{
  
  
    Serial.println("button_setup()");
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);


  button.setClickTicks(99);
  button.setPressTicks(1000);
  button.attachClick(button_click);
//  button.attachDoubleClick(button_doubleclick); // does not work very well :(
  button.attachLongPressStart(button_longpress);

 
  
}




void button_click()
{
 //Serial.println("Button clicked");  
 mqtt_publish("events/button","CLICK");
}




void button_longpress()
{
 //Serial.println("Button LONPRESSED");  
 mqtt_publish("events/button","LONGPRESS");
}


/* broken dreams below.
 
void button_doubleclick()
{
 //Serial.println("Button DOUBLEclicked");  
 mqtt_publish("events/button","DOUBLECLICK");
}

 */



void button_work()
{
  button.tick(); 
}




