////////////// MQTT
//#define MQTT_VERSION  3
#define MQTT_VERSION  MQTT_VERSION_3_1

#include <PubSubClient.h>





WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;







void mqtt_setup()
{
  Serial.println("mqtt_setup()");
  Serial.print("Setting up server ");
  Serial.print(mqtt_server);
  Serial.print(":");   
  Serial.println(atoi(mqtt_port));
        
  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(callback); 
  
}



void mqtt_publish(char* topic,char* message)
{



  char path[100]; 
  sprintf (path, "GHOUST/clients/%s/%s", sysname , topic);

   Serial.print("PUBLISHING mqtt_publish('");
   Serial.print(path);
   Serial.print("','");
   Serial.print(message);
   Serial.println("')");

  client.publish(path, message);




}




void mqtt_publish_retained(char* topic,char* message)
{



  char path[100]; 
  sprintf (path, "GHOUST/clients/%s/%s", sysname , topic);

   Serial.print("PUBLISHING RETAINED mqtt_publish('");
   Serial.print(path);
   Serial.print("','");
   Serial.print(message);
   Serial.println("')");

  client.publish(path, message, true);




}










/*
 *  This is called every time something is RECEIVED from the mqtt broker
 */


//String current_message ="";


char current_message[100];




void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");



  //current_message='';


  for(int i=0;i<length;i++)
   current_message[i] = payload[i];

//strncpy(current_message, payload,length);
current_message[length] = '\0';


  
  /*
  for (int i = 0; i < length; i++) {
    current_message += (char)payload[i];
    
    Serial.print((char)payload[i]);
  }

  */

  
  Serial.println();



  if(strstr(topic,"/led"))
  {
   Serial.println("FOUND /led via strstr()"); 
   led_handle_request(current_message);
   return;    
  }


  if(strstr(topic,"/buzzer"))
  {
   Serial.println("FOUND /buzzer via strstr()"); 
   buzzer_handle_request(current_message);
   return;    
  }



  if(strstr(topic,"/motor"))
  {
   Serial.println("FOUND /motor via strstr()"); 
   motor_handle_request(current_message);
   return;    
  }


  if(strstr(topic,"/accel"))
  {

      if(strstr(topic,"warn"))
        accelerometer_threshold_handle_request(current_message,0);
      else
        accelerometer_threshold_handle_request(current_message,1);
   return; 
     
  }




  // if not the received message is not for us specifically but for all connected devices, something that will start or end a new game..


}










void mqtt_connect() {
  
  // Loop until we're reconnected
   while (!client.connected()) {
 
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect


/*
boolean connect (clientID, willTopic, willQoS, willRetain, willMessage)

Connects the client with a Will message specified.

Parameters
clientID : the client ID to use when connecting to the server.
willTopic : the topic to be used by the will message (const char[])
willQoS : the quality of service to be used by the will message (int : 0,1 or 2)
willRetain : whether the will should be published with the retain flag (boolean)
willMessage : the payload of the will message (const char[])
Returns
false - connection failed.
true - connection succeeded. 
 */



    char willTopic[] = "";

    //was ist das?
    int willQos=1;
    // den disconnect status NICHT retainen!! sonst blasen wir die DB auf.
    int willRetain=1;    


/*

    String mqtt_topic_base = "GHOUST/clients/";

    mqtt_topic_base = mqtt_topic_base + getUniqueSystemName();

    String topic = mqtt_topic_base;
    topic = topic + "/status";

    topic.toCharArray(willTopic,topic.length()+1);


    Serial.print("Trying to connect with: LWT(");
    Serial.print(willTopic);
    Serial.println(")");    

*/

    char topic[50]; 
    sprintf (topic, "GHOUST/clients/%s/status", sysname);



    
    if (client.connect(sysname,topic,willQos,willRetain,"DISCONNECTED")) 
    {
      

      
      Serial.println("connected");
      // Once connected, publish an announcement...

      mqtt_publish_retained("status","CONNECTED");


    // global events
     client.subscribe("GHOUST/game");

      // ... and subscribe to device specific events
    char device_subscriptions[50]; 
    sprintf (device_subscriptions, "GHOUST/clients/%s/config/#", sysname); 
    client.subscribe(device_subscriptions);


  
      
      
      
      
    } else {

      // TODO ERROR INDICATION WITH SOUND/LIGHT
      
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    
    
  }
}







void mqtt_work() // called from loop()
{
    if (!client.connected()) {
     mqtt_connect();
  }
  client.loop();

}



