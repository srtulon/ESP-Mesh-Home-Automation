#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h> 

// Update these with values suitable for your network.
/*
const char* ssid = "iot_lab";
const char* password = "*abcd1234*";

const char* mqtt_server = "m24.cloudmqtt.com";
const int mqtt_port =    12947 ;
const char *mqtt_user = "vvjqiinu";
const char *mqtt_pass = "jemMAT3tJ8xf";
*/

const char* mqtt_server = "raspberrypi.local";
const int mqtt_port =    1883 ;
const char *mqtt_user = "pi";
const char *mqtt_pass = "raspberry";

String ser="";

WiFiClient espClient;
PubSubClient client(espClient);
int i=0;
char msg[13];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
//  Serial.println(ssid);

  WiFiManager wifiManager;
  WiFi.hostname("Espupdate");
  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    delay(1000);
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("");
  for (int i = 0; i < length; i++) {
    ser=ser+(char)payload[i];
  }
  if(ser[0]=='@'){
   Serial.printf("%s\n",ser.c_str());
   delay(2000);
  }
  //Serial.println(ser.length());
  ser="";
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
  //if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
  if (client.connect(clientId.c_str())) {
    Serial.println("connected");
    client.subscribe("device");
    /*
    if (Serial.available()){
      int i = 0;
      while (Serial.available() > 0)
        {
          msg[i] = Serial.read();
          i++;
        }
       delay(100);
          Serial.print("Sending1: ");
          Serial.println((char*)msg);
          client.publish("device", msg);
          // Once connected, publish an announcement...
    }
    */
  }
 
  else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      
      // Wait 5 seconds before retrying
      delay(5000);
      ESP.reset();
    }
  }
   Serial.println("");
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output  Serial.begin(115200);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  while (Serial.available() > 0)
    {
        msg[i] = Serial.read();
        i++;
    }
    i=0;
    delay(100);
    //if(msg[0]=='#' && msg[sizeof msg-1]=='$'){
    if(msg[0]=='#'){
      //Serial.print("Sending: ");
      //Serial.print((char*)msg);
      client.publish("device", msg);
      memset(msg, 0, sizeof msg);
    }
    
}
