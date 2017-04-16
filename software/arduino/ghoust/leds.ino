// TODO:  ASYNCHRON MIT TIMER lib arbeiten, delay() weg..


#include <stdio.h>
#include <string.h>
const int R_PIN = 13;   // D7 GPIO 13
const int G_PIN = 12;   // D6 GPIO 12
const int B_PIN = 14;   // D5 GPIO 14





void leds_setup()
{
  
  
  Serial.println("leds_setup()");


  //analogWriteFreq(200);


  pinMode(R_PIN,OUTPUT);
  pinMode(G_PIN,OUTPUT);
  pinMode(B_PIN,OUTPUT);
  
  leds_off();
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


// initialisieren und ersten Abschnitt erstellen
ptr = strtok(message, delimiter);

while(ptr != NULL) {
        Serial.print("found:");
        Serial.println(ptr);
  if(counter==1) r= atoi(ptr);
  if(counter==2) g= atoi(ptr);
  if(counter==3) b= atoi(ptr);    
  counter++;
  ptr = strtok(NULL, delimiter);
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



