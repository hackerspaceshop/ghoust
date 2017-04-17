// TODO:  ASYNCHRON MIT TIMER lib arbeiten, delay() weg..


#include <stdio.h>
#include <string.h>
const int R_PIN = 13;   // D7 GPIO 13
const int G_PIN = 12;   // D6 GPIO 12
const int B_PIN = 14;   // D5 GPIO 14



// the current value of R,G,B channels.
// set via MQTT requests
// this is what we will reset to if we receive a color setting request with a timeout (for example on warning)
int current_r =0 ;
int current_g =0 ;
int current_b =0 ;



Ticker ledticker;


//EFFECTS
const int EFFECT_RAINBOW=1;


// set via MQTT preset
int enabled_led_effect=0;



void leds_setup()
{
  
  
  Serial.println("leds_setup()");


  //analogWriteFreq(200);


  pinMode(R_PIN,OUTPUT);
  pinMode(G_PIN,OUTPUT);
  pinMode(B_PIN,OUTPUT);
  
  leds_off();

  
}


// set from timer callback function
int ticker_called = 0;

// called from callback
void leds_ticker()
{
 ticker_called=1;
}




// handle led stuff all the time called from loop()
void leds_work()
{
 // this is set via timer and resets colors after timeout 
 if(ticker_called)
 {
    Serial.println("(leds) ticker_called set!, resetting leds");
    set_led(current_r,current_g,current_b);
    ledticker.detach();
    ticker_called=0;
 }

 // some effect was enabled, lets do this..
 if(enabled_led_effect)
 {
  switch(enabled_led_effect)
  {
   case EFFECT_RAINBOW:
 //      ticker.attach_ms(40, effect_rainbow);
     effect_rainbow();
   break;
  }

 }
 /*
 else
 {
  ticker.detach();
 }
 */


}









void leds_off()
{
  analogWriteFreq(200);
  
  analogWrite(R_PIN,get_color(0)); 
  analogWrite(G_PIN,get_color(0));
  analogWrite(B_PIN,get_color(0));  
  
}


void set_led(int r, int g, int b)
{
  
  
//  leds_off();
  /*
  
 Serial.print("Setting leds to R=");
 Serial.print(get_color(r));
 Serial.print(" G="); 
 Serial.print(get_color(g));
 Serial.print(" B=");
 Serial.println(get_color(b)); 
*/

   analogWriteFreq(200);
 
  analogWrite(R_PIN,get_color(r)); 
  analogWrite(G_PIN,get_color(g));
  analogWrite(B_PIN,get_color(b));  

}







// called everytime a MQTT action regarding leds is received

void led_handle_request(char* message)
{
 Serial.println("Some led action is going on!");

 
char led_message[100];
char delimiter[] = ":,";
char *ptr;

int counter=0;
 
 
 //Serial.println(message);


if(message[0] == 'P')
{

int preset = 0 ;

ptr = strtok(message, delimiter);

while(ptr != NULL) {
        Serial.print("found:");
        Serial.println(ptr);
  if(counter==1) preset= atoi(ptr);  
  counter++;
  ptr = strtok(NULL, delimiter);
}


  
 // PRESET
 Serial.print("(led) Switching to PRESET: ");
 Serial.println(preset);

 enabled_led_effect=preset; 


 return;
}






if(message[0] == 'R')
{
 // RAW
  Serial.println("(led) Switching to RAW");


 // disable an effect if one is running..
 enabled_led_effect=0;



  int r=0;
  int g=0;
  int b =0; 
  int timeout=0;


  // initialisieren und ersten Abschnitt erstellen
  ptr = strtok(message, delimiter);

  while(ptr != NULL) 
  {
    Serial.print("found:");
    Serial.println(ptr);
    if(counter==1) r= atoi(ptr);
    if(counter==2) g= atoi(ptr);
    if(counter==3) b= atoi(ptr);    
    if(counter==4) timeout= atoi(ptr);    
    counter++;
    ptr = strtok(NULL, delimiter);
  }

  // set the current color scheme to fall back to
  if(!timeout)
  {
   current_r=r;
   current_g=g;
   current_b=b;    
  }
  else
  {
    Serial.print("(leds) Callback timer set to: ");
    Serial.println(timeout);  
    // enable timer callback to reset colors after timeout
    ledticker.attach_ms(timeout, leds_ticker);
    
  }



  set_led(r,g,b);

 return;
}

 
  
}



int get_color(int val)
{


// needs substraction/inversion if we run directly on a led, or no substruction if we use a high power led siwth FETs
//  val=1023-val;  
  return  val;
}










////////// EFFECTS




//---------- RAINBOW EFFECT STUFF


int rainbowcounter =0;

//defined by Wheel();
int wR=0;
int wG=0;
int wB=0;

uint32_t last_call_effect_rainbow =0;

void effect_rainbow()
{

  //update this effect every ~10 milliseconds
  if(!last_call_effect_rainbow) last_call_effect_rainbow=millis();
  if((millis()-last_call_effect_rainbow)<10) return;
  last_call_effect_rainbow=0;
  


  rainbowcounter++;
  if(rainbowcounter==1023) rainbowcounter=0;

   // update the colors for a rainbowfade
  Wheel(rainbowcounter);



  (get_color(wR),get_color(wG),get_color(wB));




}



// Input a value 0 to 1023 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(int WheelPos) {
  WheelPos = 1023 - WheelPos;


  
  if(WheelPos < 341) {
    wR=1023 - WheelPos * 3;
    wG=0;
    wB= WheelPos * 3;

    return;
  }
  
  if(WheelPos < 682) {
    WheelPos -= 341;

    wB=1023 - WheelPos * 3;
    wR=0;
    wG= WheelPos * 3;


    
    return ;
  }


  WheelPos -= 682;
  
  wG=1023 - WheelPos * 3;
  wB=0;
  wR= WheelPos * 3;
  
  return ;
}




/*



//---------------------- FLASH TO COLOR EFFECT
// starts with a white flash fades to destination color over time
// ends in destination color being set.




uint32_t last_call_effect_flash =0;
int flashcounter =0;

//defined by effect_flash();
int fR=0;
int fG=0;
int fB=0;

void effect_flash(int target_r,int target_g,int target_b)
{

  //update this effect every ~10 milliseconds
  if(!last_call_effect_flash) last_call_effect_flash=millis();
  if((millis()-last_call_effect_flash)<10) return;
  last_call_effect_flash=0;
  




  if (fR>target_r)  fR=1023-flashcounter;
  if (fG>target_g)  fG=1023-flashcounter;
  if (fB>target_b)  fB=1023-flashcounter;    


  set_led(get_color(fR),get_color(fG),get_color(fB));

  flashcounter++;
  if(flashcounter==1024)
  { 
    flashcounter=0;
    enabled_led_effect=0;
  }


}

*/
