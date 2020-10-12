#include "painlessMesh.h"

#define   MESH_PREFIX     "mesh"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

const int MOTION_PIN = D1; 
const int LED_PIN = D4;



String a="";
int devtype=2;
int devstatus=0;


// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );


// Send message to mesh
void sendMessage() {
  //message format: #(id),(type),(status)$
  String id = "";
  id += mesh.getNodeId();
  a= '#' + id + ',' + devtype + ',' + devstatus + '$'; 
  mesh.sendBroadcast(a);
  Serial.println(a);
  taskSendMessage.setInterval(random( TASK_SECOND * 2, TASK_SECOND * 5));
}

//Receive message from mesh
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());

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
  Serial.begin(115200);
  Serial.println("Start");
  delay(1000);
  pinMode(MOTION_PIN,INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN,LOW);

  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
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
  } 
}


/////////////////// Test Part /////////////////

//This part is for testing whole system without PIR

/*
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
  }
  else if(devstatus==1){
    devstatus=0;
    digitalWrite(LED_PIN, HIGH);
  }
}

*/
