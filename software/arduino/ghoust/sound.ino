// TODO:  implement ASYNCHRIN sound handling without delay.
//i have no idea how that would work out.
// maybe we could create a list of sound events that are worked on in sounds_work or something like that.



#include "pitches.h"

#define SOUND_ACTIVE 1
#define SOUNDPIN 16  //D0 GPIO16


//Ticker soundticker;


void sound_setup()
{
  Serial.println("sound_setup()");
  pinMode(SOUNDPIN,OUTPUT); 
}







void beep(int frequency, int duration)
{ 
  if(!SOUND_ACTIVE) return;
  
  
   // at some point during development the tone() function broke. godess knows why.
   //- See more at: http://www.esp8266.com/viewtopic.php?f=29&t=7634#sthash.WRAtKbTR.dpuf
   
   // tone(SOUNDPIN, frequency, duration);
      
   
   pinMode(SOUNDPIN,OUTPUT);
   analogWriteFreq(frequency);
   analogWrite(SOUNDPIN,500);
   delay(duration);
   analogWrite(SOUNDPIN,0); 
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
  
    while(ptr != NULL) 
    {
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
      
     while(ptr != NULL) 
     {
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
 *   triggered from mqtt
 */


void beep_moep()
{
  if(!SOUND_ACTIVE) return;
  beep(NOTE_B3,500);
}

void beep_fail()
{
  if(!SOUND_ACTIVE) return;  
   beep(NOTE_E4,200);
   beep(NOTE_B3,500);
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

  beep(NOTE_E3,115);
  beep(NOTE_G4,115);
  beep(NOTE_E6,115);
  beep(NOTE_C5,115);
  beep(NOTE_D6,115);
  beep(NOTE_G7,140);
  
}




void beep_no()
{
  if(!SOUND_ACTIVE) return;
  beep(700,100);
  beep(600,100);
}









////////////  code below did not work out, left for future reference if i find time to migrate to interrupt/timer based sound playback.




/*
int soundticker_called=0;

void sound_work()
{
 if(soundticker_called)
 {
  soundticker_called=0;
 
  analogWrite(SOUNDPIN,0); //- See more at: http://www.esp8266.com/viewtopic.php?f=29&t=7634#sthash.WRAtKbTR.dpuf
  soundticker.detach();

  
 }

  
}


//called from timer
void soundreset()
{
  analogWrite(SOUNDPIN,0); //- See more at: http://www.esp8266.com/viewtopic.php?f=29&t=7634#sthash.WRAtKbTR.dpuf
  soundticker.detach();
}
*/
