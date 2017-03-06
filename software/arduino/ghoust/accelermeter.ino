///TODO
// Move the whole motion detection stuff into interrupt routine and set tap events on accelerometer.

/*
 *  SCL = D1 GPIO5
 *  SDA = D2 GPIO4
 *  SA0 = 3.3V
 *  VIN = %V ?
 *  
 * 
 * 
 */

// needs include in joust file for arduino fubar.

#include <Wire.h> // Must include Wire library for I2C
#include <SparkFun_MMA8452Q.h> // Includes the SFE_MMA8452Q library


MMA8452Q accel;


void motion_setup()
{
  
    Serial.println("motion_setup()");
  
    Wire.begin();
    
   // Serial.println("Wire.begin() finihed");  

    // for library
    accel.init();
    
   // Serial.println("accel.init() finished");
}









// this are maximum rating that trigger the shock action.
//should be set remotly by master OTA
float max_cx =1.999;
float min_cx = -0.5;

float max_cy =0.7;
float min_cy = -0.7;

float max_cz = 0.7;
float min_cz= -0.7;







// lets player live or die.
// too fast a move return 1 and send to die() in loop();
int motion_checkPositionAdjustment()
{
  
 
// too much neigung on an important axis. needs to loose.  
 if (accel.cx>= max_cx || accel.cx< min_cx) return 1;
 if (accel.cy>= max_cy || accel.cy< min_cy) return 1;
 if (accel.cz>= max_cz || accel.cz< min_cz) return 1;
   
  
 // alles gut 
 return 0; 
}












///DEBUG STUFF



byte lastpos =0;


// This function demonstrates how to use the accel.readPL()
// function, which reads the portrait/landscape status of the
// sensor.
void printOrientation()
{


  
  // accel.readPL() will return a byte containing information
  // about the orientation of the sensor. It will be either
  // PORTRAIT_U, PORTRAIT_D, LANDSCAPE_R, LANDSCAPE_L, or
  // LOCKOUT.
  byte pl = accel.readPL();

  if(pl == lastpos)   return;

  lastpos=pl;
 
  switch (pl)
  {
  case PORTRAIT_U:
  //  Serial.println("ACCEL: Portrait Up");
    mqtt_publish("events/gestures","PORTRAIT_UP");
    break;
  case PORTRAIT_D:
  //  Serial.println("ACCEL: Portrait Down");
    mqtt_publish("events/gestures","PORTRAIT_DOWN");
    break;
  case LANDSCAPE_R:
  //  Serial.println("ACCEL: Landscape Right");
     mqtt_publish("events/gestures","LANDSCAPE_RIGHT");   
    break;
  case LANDSCAPE_L:
  //  Serial.println("ACCEL: Landscape Left");
    mqtt_publish("events/gestures","LANDSCAPE_LEFT");      
    break;
  case LOCKOUT:
  //  Serial.println("ACCEL: Flat");
     mqtt_publish("events/gestures","FLAT");       
    break;
  }
  
  
  
}







void printAcceleration()
{


  
  
    if (accel.available())
  {
    // First, use accel.read() to read the new variables:
    accel.read();
  
  
  Serial.print("X: ");
  Serial.print(accel.cx, 3);
//  Serial.print(" ");
//  Serial.print(accel.cx, 3);  
  Serial.print("\t");

  Serial.print("Y:" );
  Serial.print(accel.cy, 3);
//  Serial.print(" ");
//  Serial.print(accel.cy, 3);    
  Serial.print("\t");
  
  Serial.print("Z: ");  
  Serial.print(accel.cz, 3);
//    Serial.print(" ");
//  Serial.print(accel.cz, 3);  
  
  Serial.print("\n");
  
  
  
  
  }
  
  
}

















////////////
//native code for TAP below .. maybe we should integrate this.. lets see later on
/*



/*

// The SparkFun breakout board defaults to 1, set to 0 if SA0 jumper on the bottom of the board is set
#define MMA8452_ADDRESS 0x1D  // 0x1D if SA0 is high, 0x1C if low

//Define a few of the registers that we will be accessing on the MMA8452
#define OUT_X_MSB 0x01
#define XYZ_DATA_CFG  0x0E
#define WHO_AM_I   0x0D
#define CTRL_REG1  0x2A

#define GSCALE 2 // Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.



void readAccelData(int *destination)
{
  byte rawData[6];  // x/y/z accel register data stored here

  readRegisters(OUT_X_MSB, 6, rawData);  // Read the six raw data registers into data array

  // Loop to calculate 12-bit ADC and g value for each axis
  for(int i = 0; i < 3 ; i++)
  {
    int gCount = (rawData[i*2] << 8) | rawData[(i*2)+1];  //Combine the two 8 bit registers into one 12-bit number
    gCount >>= 4; //The registers are left align, here we right align the 12-bit integer

    // If the number is negative, we have to make it so manually (no 12-bit data type)
    if (rawData[i*2] > 0x7F)
    {  
      gCount -= 0x1000;
    }

    destination[i] = gCount; //Record this gCount into the 3 int array
  }
}

// Initialize the MMA8452 registers 
// See the many application notes for more info on setting all of these registers:
// http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=MMA8452Q
void initMMA8452()
{
  Serial.println("initMMA8452()");
  
  
  byte c = readRegister(WHO_AM_I);  // Read WHO_AM_I register
  
  
           Serial.println("balh");
  
  
  if (c == 0x2A) // WHO_AM_I should always be 0x2A
  {  
    Serial.println("MMA8452Q is online...");
         Serial.println("dort");
  }
  else
  {
    Serial.print("Could not connect to MMA8452Q: 0x");
    Serial.println(c, HEX);
   
   
     Serial.println("da");
   
    while(1) ; // Loop forever if communication doesn't happen
  }

  Serial.println("hier?");
  

  MMA8452Standby();  // Must be in standby to change registers

  // Set up the full scale range to 2, 4, or 8g.
  byte fsr = GSCALE;
  if(fsr > 8) fsr = 8; //Easy error check
  fsr >>= 2; // Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
  writeRegister(XYZ_DATA_CFG, fsr);

  //The default data rate is 800Hz and we don't modify it in this example code

  MMA8452Active();  // Set to active to start reading
}

// Sets the MMA8452 to standby mode. It must be in standby to change most register settings
void MMA8452Standby()
{
  byte c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c & ~(0x01)); //Clear the active bit to go into standby
}

// Sets the MMA8452 to active mode. Needs to be in this mode to output data
void MMA8452Active()
{
  byte c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c | 0x01); //Set the active bit to begin detection
}

// Read bytesToRead sequentially, starting at addressToRead into the dest byte array
void readRegisters(byte addressToRead, int bytesToRead, byte * dest)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  Wire.requestFrom(MMA8452_ADDRESS, bytesToRead); //Ask for bytes, once done, bus is released by default

  while(Wire.available() < bytesToRead); //Hang out until we get the # of bytes we expect

  for(int x = 0 ; x < bytesToRead ; x++)
    dest[x] = Wire.read();    
}

// Read a single byte from addressToRead and return it as a byte
byte readRegister(byte addressToRead)
{
  
  
       Serial.println("1");
  
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active
  
  
         Serial.println("2");
  
  Wire.requestFrom(MMA8452_ADDRESS, 1); //Ask for 1 byte, once done, bus is released by default


       Serial.println("3");

  while(!Wire.available()) ; //Wait for the data to come back
  return Wire.read(); //Return this one byte
}

// Writes a single byte (dataToWrite) into addressToWrite
void writeRegister(byte addressToWrite, byte dataToWrite)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToWrite);
  Wire.write(dataToWrite);
  Wire.endTransmission(); //Stop transmitting
}










*/
