#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRutils.h>

#include "painlessMesh.h"

#define   MESH_PREFIX     "mesh"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555

const uint16_t kIrLed = 4;  // The ESP GPIO pin to use that controls the IR LED.
IRac ac(kIrLed);  // Create a A/C object using GPIO to sending messages with.

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

String devtype="a"; //a=type ac
int devstatus=011025; //01= protocol(first is AIRWELL) 1=first model, 0= power off(1=power on), 25=temperature
//long id=ESP.getChipId();
//long id=mesh.getNodeId();

char charBuf[50];


int temp;

//String a=temp.toString();\

// Send message to mesh
void sendMessage() {
  //message format: #(id),(type),(status)$ 
  //send to certain amount of time for registering into database
  if(millis()<100000){
    String id = "";
    id += mesh.getNodeId();
    String a= '#' + (String)id + ',' + devtype + ',' + devstatus + '$';
    mesh.sendBroadcast(a);
    taskSendMessage.setInterval(random( TASK_SECOND * 1, TASK_SECOND * 10));
  }
}

//Receive message from mesh
void receivedCallback( uint32_t from, String &msg ) {
   //message format: @(ac protocol)(model)(power)(temperature)%
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  
  //To convert msg into proper String
  msg.trim();
  msg.toCharArray(charBuf, 50);
  String msg1=(String)msg;
  
  if(msg1.indexOf('@')>-1){
    Serial.println(msg1); 
    if(msg1.substring(2,3)=="01"){
        Serial.print("Protocol: AIRWELL");
        ac.next.protocol = decode_type_t::AIRWELL;
     }
    else if(msg1.substring(2,3)=="02"){
        Serial.print("Protocol: AIWA_RC_T501");
        ac.next.protocol = decode_type_t::AIWA_RC_T501;
     }
  }
  if(msg1[4]=='0'){
    ac.next.power = false;
  }
  else if(msg1[4]=='1'){
    ac.next.power = false;  // Initially start with the unit off.
    ac.next.model = msg1[3];
    ac.next.degrees = msg1.substring(6,7);
  }

  
  
}


void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}


void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Start");

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes(ERROR | STARTUP | DEBUG);  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  
  // if you want your node to accept OTA firmware, simply include this line
  // with whatever role you want your hardware to be. For instance, a
  // mesh network may have a thermometer, rain detector, and bridge. Each of
  // those may require different firmware, so different roles are preferrable.
  //
  // MAKE SURE YOUR UPLOADED OTA FIRMWARE INCLUDES OTA SUPPORT OR YOU WILL LOSE
  // THE ABILITY TO UPLOAD MORE FIRMWARE OVER OTA. YOU ALSO WANT TO MAKE SURE
  // THE ROLES ARE CORRECT
  mesh.initOTAReceive(devtype);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  
  ac.next.protocol = decode_type_t::DAIKIN;  // Set a protocol to use.
  ac.next.model = 1;  // Some A/Cs have different models. Try just the first.
  ac.next.power = false;  // Initially start with the unit off.
  ac.next.mode = stdAc::opmode_t::kCool;  // Run in cool mode initially.
  ac.next.celsius = true;  // Use Celsius for temp units. False = Fahrenheit
  ac.next.degrees = 25;  // 25 degrees.
  ac.next.fanspeed = stdAc::fanspeed_t::kMedium;  // Start the fan at medium.

}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
