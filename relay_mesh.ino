#include "painlessMesh.h"

#define   MESH_PREFIX     "brac"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555




Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

String msg = "";
int devtype=1;
int devstatus=0;


char charBuf[50];

int pin1=D4;



// Needed for painless library
void sendMessage() {
  //message format: #(id),(type),(status)$ 
  //send to certain amount of time for registering into database 
  if(millis()<100000){
    String id = "";
    id += mesh.getNodeId();
    String a= '#' + (String)id + ',' + devtype + ',' + devstatus + '$';
    mesh.sendBroadcast(a);
    taskSendMessage.setInterval(random( TASK_SECOND * 1, TASK_SECOND * 10));
  // a="";
  }
}

void receivedCallback( uint32_t from, String &msg ) {
  //message format: @(status)%
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  msg.trim();
  msg.toCharArray(charBuf, 13);
  
  //To convert msg into proper String
  String msg1=(String)msg;

  if(msg1.indexOf('@')>-1){
    Serial.println(msg1[1]);
    if(msg[1]=='1'){
      digitalWrite(pin1,LOW);
    }
    else{
      digitalWrite(pin1,HIGH);
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
  
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
