#include <Wire.h>               
#include "SSD1306Wire.h"       

#include "painlessMesh.h"

#define   MESH_PREFIX     "brac"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555

SSD1306Wire display(0x3c, SDA, SCL);   
String a;

Scheduler userScheduler; 
painlessMesh  mesh;

void sendMessage() ;

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

String msg = "00000000";

void sendMessage() {
  //msg += mesh.getNodeId();
  if(a!=""){
    int i=0;
    while(i<1){
      //Serial.print(a[0]);
      mesh.sendBroadcast(a);
      taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 3 ));
      i++;
    }
  }
  a="";
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  if(msg!="" && msg!=NULL ){
    Serial.printf("%s",msg.c_str());
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

  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  delay(500);

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
  mesh.update();
  if(Serial.available()){
    while(Serial.available()){
      a= Serial.readString();
      if(a.charAt(0)>=48 && a.charAt(0)<=57){
        oledPrint(a);
      }
    }
  } 
}

void oledPrint(String s){
  display.clear();
  // draw the current demo method
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, s);
  display.display();
}
