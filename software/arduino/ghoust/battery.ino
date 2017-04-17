

// TODO: tst battery levels with powersupply to find correct values here.

int BATTERYLOW_THRESHOLD =500;


void battery_setup()
{
   
  
  Serial.println("battery_setup()");

  pinMode(A0,INPUT);

  battery_check();

  
}


void battery_check()
{
 int battery_level =0; 
 battery_level = analogRead(A0);

  
 //Serial.print("battery: ");
 //Serial.println(battery_level);
 
 if(battery_level<BATTERYLOW_THRESHOLD)
 {
   beep_fail();

   // stay in here forever
   while(true){
       set_led(10,0,0);
       delay(400);
          set_led(0,0,0);
       delay(200);   
    }
 }

  
}

