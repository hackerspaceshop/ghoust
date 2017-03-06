//uint8_t dataPin  = D3;    // Yellow wire on Adafruit Pixels
//uint8_t clockPin = D4;   // Green wire on Adafruit Pixels

// Don't forget to connect the ground wire to Arduino ground,
// and the +5V wire to a +5V supply

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
//Adafruit_WS2801 strip = Adafruit_WS2801(1, dataPin, clockPin);


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
    //high == off (common anode)
  digitalWrite(R_PIN,HIGH); 
  digitalWrite(G_PIN,HIGH);
  digitalWrite(B_PIN,HIGH);  
  
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
 
 

 analogWrite(R_PIN,1023-r);  
 analogWrite(G_PIN,1023-g);  
 analogWrite(B_PIN,1023-b);  

 
  
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



