#include "painlessMesh.h"

#define   MESH_PREFIX     "mesh"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

//const int MOTION_PIN = D1; 
const int LED_PIN = 2;



char charBuf[50];
String a=""; 
String devtype="p"; //p=type pir sensor
int devstatus=0; // 0 =Off , 1= On
int prevstatus=0; // Previous status
int ack=1; // Acknowledgement 1 = recieved/not needed , 0= pending  

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );


// Send message to mesh
void sendMessage() {
  //message format: #(id),(type),(status)$
  //if status changes, send message
  if((devstatus != prevstatus) || (ack=0)){
    String id = "";
    id += mesh.getNodeId();
    a= '#' + id + ',' + devtype + ',' + devstatus + '$'; 
    mesh.sendBroadcast(a);
    Serial.println(a);
    taskSendMessage.setInterval(random( TASK_SECOND * 1, TASK_SECOND * 3));
    prevstatus=devstatus;
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
  if(msg1.indexOf('&')>-1){
    if(msg[2]=='1'){
       ack=1;
    }
  }
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("%u , relay", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}


void setup() {
  SPIFFS.begin(true);
  Serial.begin(115200);
  Serial.println("Start");
  delay(1000);
  //pinMode(MOTION_PIN,INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN,LOW);

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

}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  test();
  /*
  int proximity = digitalRead(MOTION_PIN);  
  if (proximity == LOW ){ // If the sensor's output goes low, motion is detected
    digitalWrite(LED_PIN, LOW);
    //Serial.println(F("Motion detected!"));
    devstatus=1;
  }
  else {
    digitalWrite(LED_PIN, HIGH);
    //Serial.println(F("No motion detected!"));
    devstatus=0;
  } */
}


/////////////////// Test Part /////////////////

//This part is for testing whole system without PIR


unsigned long previousMillis = 0;
const long interval = 20000;

void test(){
  if(millis() - previousMillis >= interval){
    previousMillis=millis();
    stateChange();
  }
}
void stateChange(){
  if (devstatus==0){
    devstatus=1;
    digitalWrite(LED_PIN, LOW);
    ack=0;
  }
  else if(devstatus==1){
    devstatus=0;
    digitalWrite(LED_PIN, HIGH);
    ack=0;
  }
}
