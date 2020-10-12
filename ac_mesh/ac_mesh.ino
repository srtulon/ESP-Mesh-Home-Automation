#include "painlessMesh.h"

#define   MESH_PREFIX     "mesh"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

String devtype="a"; //a=type ac
int devstatus=0;
//long id=ESP.getChipId();
//long id=mesh.getNodeId();

char charBuf[50];

int pin1=D4;
int pin2=D5;
int pin3=D6;

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
   //message format: @(relay number)(status)%
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  
  //To convert msg into proper String
  msg.trim();
  msg.toCharArray(charBuf, 13);
  String msg1=(String)msg;
  if(msg1.indexOf('@')>-1){
    Serial.println(msg1[1]); 
    if(msg[2]=='1'){
       temp=LOW;
    }
    else{
      temp=HIGH;
    }
    if(msg[1]=='0'){
      digitalWrite(pin1,temp);
      digitalWrite(pin2,temp);
      digitalWrite(pin3,temp);
      Serial.print(F("All relay status :"));
      Serial.println(temp); 
    }
    else if(msg[1]=='1'){
      digitalWrite(pin1,temp);
      Serial.print(F("Relay 1 status :"));
      Serial.println(temp); 
    }
    else if(msg[1]=='2'){
      digitalWrite(pin2,temp);
      Serial.print(F("Relay 2 status :"));
      Serial.println(temp); 
    }
    else if(msg[1]=='3'){
      digitalWrite(pin3,temp);
      Serial.print(F("Relay 3 status :"));
      Serial.println(temp); 
    }
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
  Serial.println("Start");

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  pinMode(pin1,OUTPUT);
  pinMode(pin2,OUTPUT);
  pinMode(pin3,OUTPUT);
  
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
