//************************************************************
// this is a simple example that uses the painlessMesh library to
// connect to a another network and relay messages from a MQTT broker to the nodes of the mesh network.
// To send a message to a mesh node, you can publish it to "device/to/12345678" where 12345678 equals the nodeId.
// To broadcast a message to all nodes in the mesh you can publish it to "device/to/broadcast".
// When you publish "getNodes" to "device/to/gateway" you receive the mesh topology as JSON
// Every message from the mesh which is send to the gateway node will be published to "device/from/12345678" where 12345678 
// is the nodeId from which the packet was send.
//************************************************************

#include <Arduino.h>
#include <painlessMesh.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

#define   MESH_PREFIX     "mesh"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555

#define   STATION_SSID     "xaneon"
#define   STATION_PASSWORD "bmwm3gtr"

//#define   STATION_SSID     "IOI wifi"
//#define   STATION_PASSWORD "IoI2021@wifi"

#define HOSTNAME "MQTT_Bridge"

#define light 2


// Prototypes
void receivedCallback( const uint32_t &from, const String &msg );
void mqttCallback(char* topic, byte* payload, unsigned int length);

IPAddress getlocalIP();

IPAddress myIP(0,0,0,0);


int a=192;
int b=168;
int c=0;
int d=102;
IPAddress mqttBroker(a, b, c, d);
int mqttport=1883;

//const char* mqttBroker = "raspberrypi.local";
/*
const char* mqttuser = "vvjqiinu";
const char* mqttpassword = "jemMAT3tJ8xf";
*/



painlessMesh  mesh;


WiFiClient wifiClient;
PubSubClient mqttClient(mqttBroker, 1883, mqttCallback, wifiClient);
//PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(115200);
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);

  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);

  // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/device/device/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setRoot(true);
  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);
  mqttClient.setServer(mqttBroker, mqttport);
  mqttClient.setCallback(mqttCallback);
}

void loop() {
  
  pinMode(light,OUTPUT);
  digitalWrite(light,LOW);
  
  mesh.update();
  mqttClient.loop();

  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }
  if (!mqttClient.connected() && myIP == getlocalIP()){
      //Serial.println("Attempting MQTT connection...");
      if(mqttClient.connect("painlessMeshClient")){
        Serial.println("connected");
        digitalWrite(light,HIGH);
        mqttClient.publish("device/from/gateway","Ready!");
        mqttClient.subscribe("device/to/#");
    }
  }
}


void receivedCallback( const uint32_t &from, const String &msg ) {
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
  String topic = "device/from/" + String(from);
  mqttClient.publish(topic.c_str(), msg.c_str());
}

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  char* cleanPayload = (char*)malloc(length+1);
  payload[length] = '\0';
  memcpy(cleanPayload, payload, length+1);
  String msg = String(cleanPayload);
  free(cleanPayload);

  String targetStr = String(topic).substring(10);
  Serial.println(msg);
  Serial.println(targetStr);
  
  if(targetStr == "gateway")
  {
    if(msg == "getNodes")
    {
      auto nodes = mesh.getNodeList(true);
      String str;
      for (auto &&id : nodes)
        str += String(id) + String(" ");
      mqttClient.publish("device/from/gateway", str.c_str());
    }
  }
  else if(targetStr == "broadcast") 
  {
    Serial.println("Broadcast");
    mesh.sendBroadcast(msg);
  }
  else
  {
    uint32_t target = strtoul(targetStr.c_str(), NULL, 10);
    Serial.printf("Target:");
    Serial.println(target);
    mesh.sendSingle(target, msg);

    //removed client checking because it is not working
    /*
    if(mesh.isConnected(target))
    {
      mesh.sendSingle(target, msg);
    }
    else
    {
      mqttClient.publish("device/from/gateway", "Client not connected!");
    }
    */
  }
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}
