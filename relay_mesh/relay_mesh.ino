#include "painlessMesh.h"

#define   MESH_PREFIX     "mesh"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

String devtype="r4"; //r=type relay, 4= 4 relay
//int devstatus="0000"; //0= Off , 1= On
int ack=1;  // Acknowledgement 1 = recieved/not needed , 0= pending  
//long id=ESP.getChipId();
//long id=mesh.getNodeId();

char charBuf[50];

int pin1=5;
int pin2=6;
int pin3=7;
int pin4=8;

int sw1=22;
int sw2=21;
int sw3=19;
int sw4=18;

int s1=0;
int s2=0;
int s3=0;
int s4=0;

int temp;

//String a=temp.toString();\

// Send message to mesh
void sendMessage() {
  //message format: #(id),(type),(status)$ 
  //send for certain amount of time for registering into database
  if((millis()<100000) || (ack=0)){
    String id = "";
    id += mesh.getNodeId();
    String a= '#' + (String)id + ',' + devtype + ',' + s1 + s2 + s3 + s4 + '$';
    mesh.sendBroadcast(a);
    taskSendMessage.setInterval(random( TASK_SECOND * 1, TASK_SECOND * 10));
    ack=0; 
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
    else if(msg[1]=='4'){
      digitalWrite(pin4,temp);
      Serial.print(F("Relay 4 status :"));
      Serial.println(temp); 
    }
  }
  else if(msg1.indexOf('&')>-1){
    if(msg[2]=='1'){
       ack=1;
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
  mesh.setDebugMsgTypes(ERROR | STARTUP | DEBUG); // set before init() so that you can see startup messages

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

  pinMode(pin1,OUTPUT);
  pinMode(pin2,OUTPUT);
  pinMode(pin3,OUTPUT);
  pinMode(pin4,OUTPUT);

  pinMode(sw1,INPUT);
  pinMode(sw2,INPUT);
  pinMode(sw3,INPUT);
  pinMode(sw4,INPUT);
  
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  if(digitalRead(sw1)!=s1){
    s1=digitalRead(sw1);
  }
  if(digitalRead(sw2)!=s2){
    s2=digitalRead(sw2);
  }
  if(digitalRead(sw3)!=s3){
    s3=digitalRead(sw3);
  }
  if(digitalRead(sw4)!=s4){
    s4=digitalRead(sw4);
  }
}
