// TODO:  ASYNCHRON MIT TIMER lib arbeiten, delay() weg..


#include "pitches.h"



#define SOUND_ACTIVE 1


#define SOUNDPIN 16  //D0 GPIO16




void sound_setup()
{
  
  
    Serial.println("sound_setup()");
    

 
 pinMode(SOUNDPIN,OUTPUT);

 
 
 
 //beep_coin(); 
// beep_oneup();
//beep_fail();



  
}



void beep(int frequency, int duration)
{ 

if(!SOUND_ACTIVE) return;

    
 
 pinMode(SOUNDPIN,OUTPUT);
analogWriteFreq(frequency);
analogWrite(SOUNDPIN,500);
delay(duration);
analogWrite(SOUNDPIN,0); //- See more at: http://www.esp8266.com/viewtopic.php?f=29&t=7634#sthash.WRAtKbTR.dpuf


 // at some point during development the tone() function broke. godess knows why.
 // simple wrapper, mabe we should add delay() in here according to duration .. not sure about this yet
 //tone(SOUNDPIN, frequency, duration);
}









// called everytime a MQTT action regarding leds is received

void buzzer_handle_request(char* message)
{
 Serial.println("Some buzzer action is going on!");
 Serial.println(message);


char led_message[100];
char delimiter[] = ":,";
char *ptr;

int counter=0;



 if(message[0] == 'P')
 {


  int preset=0;

ptr = strtok(message, delimiter);

while(ptr != NULL) {
        Serial.print("found:");
        Serial.println(ptr);
  if(counter==1) preset= atoi(ptr);  
  counter++;
  ptr = strtok(NULL, delimiter);
}



  
  // PRESET
  Serial.print("(buzzer) Switching to PRESET: ");
 // int preset =  message.substring(7,8).toInt() ;
  Serial.println(preset);

  
  switch(preset)
  {
   case 1:
     beep_coin(); 
   break;
   
   case 2:
     beep_oneup(); 
   break;
   
   case 3:
     beep_fail(); 
   break;

   case 4:
     beep_no(); 
   break;

   case 5:
     beep_moep(); 
   break;
    
  }

  return;
 }
 
 if(message[0] == 'R')
 {
  // RAW
  Serial.println("(buzzer) Switching to RAW");

  int frequency = 0; //  message.substring(4,8).toInt() ;
  int duration =  0; //message.substring(9,13).toInt() ;

 ptr = strtok(message, delimiter);

while(ptr != NULL) {
        Serial.print("found:");
        Serial.println(ptr);
  if(counter==1) frequency= atoi(ptr);  
  if(counter==2) duration= atoi(ptr);  
  
  counter++;
  ptr = strtok(NULL, delimiter);
}



  beep(frequency, duration);


  return;
 }

 
  
}





/*
 *   PRESET SOUNDS
 *   triggered from
 * 
 * 
 * 
 * 
 */


void beep_moep()
{
  
 if(!SOUND_ACTIVE) return;
 beep(NOTE_B3,500);
 //delay(500); 
  
}

void beep_fail()
{
  
 if(!SOUND_ACTIVE) return;
    
   beep(NOTE_E4,200);
 //delay(200);
 beep(NOTE_B3,500);
 //delay(500); 
  
}

void beep_coin()
{
 if(!SOUND_ACTIVE) return;
    
 beep(NOTE_B5,150);
 beep(NOTE_E6,500);
}



void beep_oneup()
{
 if(!SOUND_ACTIVE) return;
  

  
  int n1=120;

  
 beep(NOTE_E3,n1-5);
 //delay(n1);
 beep(NOTE_G4,n1-5);
// delay(n1);
  beep(NOTE_E6,n1-5);
// delay(n1);
  beep(NOTE_C5,n1-5);
// delay(n1);
  beep(NOTE_D6,n1-5);
// delay(n1);
  beep(NOTE_G7,n1+20);
// delay(n1+20);
 

  
}




void beep_no()
{
  
 if(!SOUND_ACTIVE) return;
  
 beep(700,100);
// delay(100);
 beep(600,100);
// delay(200);
  
}
