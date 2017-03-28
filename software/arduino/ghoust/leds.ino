const int R_PIN = 13;   // D7 GPIO 13
const int G_PIN = 12;   // D6 GPIO 12
const int B_PIN = 14;   // D5 GPIO 14





void leds_setup()
{
  
  
  Serial.println("leds_setup()");


  analogWriteFreq(200);


  pinMode(R_PIN,OUTPUT);
  pinMode(G_PIN,OUTPUT);
  pinMode(B_PIN,OUTPUT);
  
  leds_off();
}



void leds_off()
{
  
  digitalWrite(R_PIN,get_color(0)); 
  digitalWrite(G_PIN,get_color(0));
  digitalWrite(B_PIN,get_color(0));  
  
}


void leds_all(int r, int g, int b)
{
  
  
  leds_off();
  
  
 Serial.print("Setting leds to R=");
 Serial.print(r);
 Serial.print(" G="); 
 Serial.print(g);
 Serial.print(" B=");
 Serial.println(b); 
 
 

 analogWrite(R_PIN,get_color(r));  
 analogWrite(G_PIN,get_color(g));  
 analogWrite(B_PIN,get_color(b));  

 
  
}



// called everytime a MQTT action regarding leds is received

void led_handle_request(String message)
{
 Serial.println("Some led action is going on!");
 Serial.println(message);
if(message[0] == 'P')
{
 // PRESET
 Serial.print("(led) Switching to PRESET");
 int preset =  message.substring(7,8).toInt() ;
 Serial.print(preset);
 // TODO switch below here
 // switch(preset): ...
 return;
}



if(message[0] == 'R')
{
 // RAW
  Serial.println("(led) Switching to RAW");
  int r =  message.substring(4,8).toInt() ;
  int g =  message.substring(9,13).toInt() ;
  int b =  message.substring(14,18).toInt() ; 

  leds_all(r,g,b);

 return;
}

 
  
}



int get_color(int val)
{

  val=1023-val;  
  return val;
}



