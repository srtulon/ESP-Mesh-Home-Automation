#include "painlessMesh.h"

#define   MESH_PREFIX     "brac"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555


const int MOTION_PIN = D1; 
//const int LED_PIN = D4;

unsigned long previousMillis = 0;
const long interval = 5000;

String a="";
int devtype=2;
int devstatus=0;
long id=ESP.getChipId();

int pirstate=0;
long times=0;

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

String msg = "0";

// Needed for painless library
void sendMessage() {
  
  a= '#' + (String)id + ',' + devtype + ',' + devstatus + '$'; 
  mesh.sendBroadcast(a);
  Serial.println(a);
  //Serial.printf("#%lu,%s,%i\n", id,devtype,devstatus);
 // Serial.println(id);
  taskSendMessage.setInterval(random( TASK_SECOND * 1, TASK_SECOND * 3));
}


void receivedCallback( uint32_t from, String &msg ) {
  //Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  /*
  Serial.println(msg);
  if(msg.charAt(0)=='5'){
    pirstate=0;
  }
  else if (msg.charAt(0)=='6'){
    pirstate=1;
  }
  */
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
 // pinMode(LED_PIN, OUTPUT);
  //digitalWrite(LED_PIN,LOW);
//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
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
  if (proximity ==LOW) // If the sensor's output goes low, motion is detected
  {
    devstatus=1;
  }
  else 
  {
    //digitalWrite(LED_PIN, HIGH);
    //Serial.println(F("No motion detected!"));
    devstatus=0;
  }
}

/*
void stateChange(){
  if (devstatus==0){
    devstatus=1;
  }
  else if(devstatus==1){
    devstatus=0;
  }
}
*/
