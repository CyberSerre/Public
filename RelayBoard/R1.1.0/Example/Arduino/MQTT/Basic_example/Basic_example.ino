/*
 This example is derived from  PubSubClient librairy example ( mqtt_reconnect_nonBlocking)

*/

#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Config.h"


WiFiClient espClient;
PubSubClient client(espClient);
long lastReconnectAttempt = 0;




//this function is needed to connect or reconnect the wifi
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, SSID_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

Serial.println("WiFi connected");

/* for debuging purpose
  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  */
}

//This function is used to connect or reconnect to the MQTT server
boolean reconnect() {

  //Check if we have  wifi before connecting
  while (WiFi.status() != WL_CONNECTED)
  {
    setup_wifi() ;
  }

  //Connect to topic
  if (client.connect("arduinoClient"))
  {
    // MQTT client connected to the MQTT server
    client.subscribe(TOPIC_RELAY1);
    client.subscribe(TOPIC_RELAY2);
    client.subscribe(TOPIC_RELAY3);
    client.subscribe(TOPIC_RELAY4);
    
    //Subscribe to Relay topic
    client.publish("/newDeviceConnected","RelayBoard_R1.1.0");
    Serial.println(" COnnected to the MQTT Server");
  }
  return client.connected();
}


void SetupRelay()
{
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  setDefaultState();

}

//function to set default value. Used for the initial state and safety purpose
void setDefaultState()
{
  digitalWrite(RELAY1, 0);
  digitalWrite(RELAY2, 0);
  digitalWrite(RELAY3, 0);
  digitalWrite(RELAY4, 0);

}



//this function is executed for each recived topic
void callback(char* topic, byte* payload, unsigned int length)
{
  
    String MQTT_topic;
    String MQTT_payload;
  
  //Topic conversion
  MQTT_topic=topic;

  //payload conversion from Byte to Char
  for (int i = 0; i < length; i++) {
      MQTT_payload += (char)payload[i];
  }

  /* For debuging purpose
    Serial.println(topic);
    Serial.println( MQTT_payload);
  */

    //Chech for the new relay state
  int state=0;
  if(MQTT_payload=="1")
  { 
    state=1;
    //Serial.println("set to 1");
  }
 // else 
    //Serial.println("set to 0");

  //modification of the proper relay state
  if(MQTT_topic==TOPIC_RELAY1)
    digitalWrite(RELAY1, state);
  if(MQTT_topic==TOPIC_RELAY2)
    digitalWrite(RELAY2, state);
  if(MQTT_topic==TOPIC_RELAY3)
    digitalWrite(RELAY3, state);
  if(MQTT_topic==TOPIC_RELAY4)
    digitalWrite(RELAY4, state);    
}








//-------------------------------------------------------------------//
//Standard function in Arduino Sketch
void setup()
{
  //initiate the serial communication
  Serial.begin(115200);
  //Configure the MQTT parameter
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
  //Start the WIFI
  setup_wifi();
  //Setup the relay pin and default state
  SetupRelay();

}


void loop()
{

  //Check if the client is connected
  if (!client.connected())
  {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) 
    {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) 
      {
        lastReconnectAttempt = 0;
      }
    }
  } 
  else 
  {
    // Client connected
    client.loop();
  }

}
