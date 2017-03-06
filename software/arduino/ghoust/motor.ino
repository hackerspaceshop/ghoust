//TODO fill with motor actions

#define MOTORPIN 15 // GPIO15 D8

void motor_setup()
{
  
    Serial.println("motor_setup()");
  
  

 pinMode(MOTORPIN,OUTPUT);
 digitalWrite(MOTORPIN,LOW); 
 
 
// rumbleMotor();
  
}


/*
void rumbleMotor()
{
 // TODO fill me. 
  analogWrite(MOTORPIN,1023); 
 delay(500);
  analogWrite(MOTORPIN,0);
}

*/


void motor_raw(int frequency, int duration)
{
   analogWrite(MOTORPIN,frequency); 
   delay(duration);
  analogWrite(MOTORPIN,0);
}











// called everytime a MQTT action regarding leds is received

void motor_handle_request(String message)
{
if(message[0] == 'P')
{
 // PRESET
 Serial.print("(motor) Switching to PRESET");
 int preset =  message.substring(7,8).toInt() ;
 Serial.print(preset);

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
 // RAW
  Serial.println("(motor) Switching to RAW");
  int frequency =  message.substring(4,8).toInt() ;
  int duration =  message.substring(9,13).toInt() ;

  motor_raw(frequency, duration);

 return;
}

 
  
}











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


