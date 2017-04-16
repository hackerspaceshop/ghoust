//TODO fill with motor actions

#define MOTORPIN 15 // GPIO15 D8

void motor_setup()
{
  
    Serial.println("motor_setup()");
  
  

 pinMode(MOTORPIN,OUTPUT);
 digitalWrite(MOTORPIN,LOW); 
 
 
  motor_raw(1023,200);
  
}




void motor_raw(int frequency, int duration)
{
   analogWrite(MOTORPIN,frequency); 
   delay(duration);
  analogWrite(MOTORPIN,0);
}











// called everytime a MQTT action regarding leds is received

void motor_handle_request(char* message)
{

char led_message[100];
char delimiter[] = ":,";
char *ptr;
int counter=0;

  
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
 Serial.print("(motor) Switching to PRESET: ");
 //int preset =  message.substring(7,8).toInt() ;
 Serial.println(preset);

  switch(preset)
  {
   case 1:
     motor_danger(); 
   break;
   
   case 2:
     motor_nope(); 
   break;
   
   case 3:
     motor_kitzikitzi(); 
   break;

   case 4:
     motor_die(); 
   break;
   
    
  }

  return;
 }



if(message[0] == 'R')
{


 int frequency=0;
 int duration=0;
 
 ptr = strtok(message, delimiter);

while(ptr != NULL) {
        Serial.print("found:");
        Serial.println(ptr);
  if(counter==1) frequency= atoi(ptr);  
  if(counter==2) duration= atoi(ptr);  
  
  counter++;
  ptr = strtok(NULL, delimiter);
}

 
 
 
 // RAW
  Serial.print("(motor) Switching to RAW freq: ");
  Serial.print(frequency);
  Serial.print(" duration:");
  Serial.println(duration);
  
    
//  int frequency =  message.substring(4,8).toInt() ;
//  int duration =  message.substring(9,13).toInt() ;

  motor_raw(frequency, duration);

 return;
}

 
  
}










// TODO:  ASYNCHRON MIT TIMER lib arbeiten, delay() weg..

/*
 * 
 * MOTOR PRESETS
 * 
 * 
*/


void  motor_danger(){

    for(int i=512;i<1023;i++)
  {
     analogWrite(MOTORPIN,i); 
     delay(2);
  }
  analogWrite(MOTORPIN,0);    
    

}


     
void motor_nope() {
  analogWrite(MOTORPIN,1023); 
 delay(500);
  analogWrite(MOTORPIN,128); 
 delay(500);
  analogWrite(MOTORPIN,0);
}


void motor_kitzikitzi() {
  for(int i=0;i<3;i++)
  {
     analogWrite(MOTORPIN,512); 
     delay(200);
     analogWrite(MOTORPIN,0);    
     delay(100);
  }
}


void     motor_die() {
  analogWrite(MOTORPIN,1023); 
  delay(2000);
  analogWrite(MOTORPIN,0);
}


