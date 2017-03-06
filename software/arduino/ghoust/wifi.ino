
// TODO switch to wifiManager



const char *ssid =      "ghoust";      
const char *pass =      "scaryghoust";




void wifi_setup()
{
  
    Serial.println("wifi_setup()");
  
  
  
    WiFi.begin(ssid, pass);


  
  
    //Wifi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected, using IP address: ");
  Serial.println(WiFi.localIP());
  
  
  
  
  // all green!
  //leds_all(0,1023,0);

   // make sound 
 beep_coin();
  

  
}









#define WL_MAC_ADDR_LENGTH 6


//creates the string that shows if the device goes into accces pint mode
void createUniqueSystemName()
{



 
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  sprintf(sysname,"GHOUST_%X-%X",mac[WL_MAC_ADDR_LENGTH - 2],mac[WL_MAC_ADDR_LENGTH - 1]);



/*
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + String("-") + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);

  macID.toUpperCase();
  String UniqueSystemName = String("GHOUST_") + macID;

  return UniqueSystemName;
*/
}



