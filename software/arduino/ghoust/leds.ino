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



Ticker ticker;


void leds_setup()
{
  
  
  Serial.println("leds_setup()");


  //analogWriteFreq(200);


  pinMode(R_PIN,OUTPUT);
  pinMode(G_PIN,OUTPUT);
  pinMode(B_PIN,OUTPUT);
  
  leds_off();

  
}



int ticker_called = 0;
// called from callback
void leds_ticker()
{
 ticker_called=1;
}




// handle led stuff periodically
void leds_work()
{
 // this is set via timer and resets colors after timeout 
 if(ticker_called)
 {
      Serial.println("(leds) ticker_called set!, resetting leds");
  
    leds_all(current_r,current_g,current_b);
    ticker.detach();
    ticker_called=0;
 }


}









void leds_off()
{
  analogWriteFreq(200);
  
  analogWrite(R_PIN,get_color(0)); 
  analogWrite(G_PIN,get_color(0));
  analogWrite(B_PIN,get_color(0));  
  
}


void leds_all(int r, int g, int b)
{
  
  
  leds_off();
  
  
 Serial.print("Setting leds to R=");
 Serial.print(get_color(r));
 Serial.print(" G="); 
 Serial.print(get_color(g));
 Serial.print(" B=");
 Serial.println(get_color(b)); 


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
 // TODO switch below here
 // switch(preset): ...
 return;
}






if(message[0] == 'R')
{
 // RAW
  Serial.println("(led) Switching to RAW");




  int r=0;
  int g=0;
  int b =0; 
  int timeout=0;


// initialisieren und ersten Abschnitt erstellen
ptr = strtok(message, delimiter);

while(ptr != NULL) {
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
 // enable timer callback to reset colors 
   ticker.attach_ms(timeout, leds_ticker);
  
}



  leds_all(r,g,b);

 return;
}

 
  
}



int get_color(int val)
{

//  val=1023-val;  
  return  val;
}



