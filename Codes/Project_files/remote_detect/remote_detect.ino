#include <Arduino.h>
#include <assert.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>

#include "painlessMesh.h"



#define   MESH_PREFIX     "mesh"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

char charBuf[50];

int light=D4;
String a="";
//String b="";

// Send message to mesh
void sendMessage() {
  //send for certain amount of time for registering into database
  if(a.length()>1){
    mesh.sendBroadcast(a);
    taskSendMessage.setInterval(random( TASK_SECOND * 1, TASK_SECOND * 5));
    //mesh.sendBroadcast(b);
    //taskSendMessage.setInterval(random( TASK_SECOND * 1, TASK_SECOND * 5));


  }
}


//Receive message from mesh
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  //To convert msg into proper String
  msg.trim();
  msg.toCharArray(charBuf, 13);
  String msg1=(String)msg;
  if(msg1.indexOf('&')>-1){
    if(msg[2]=='1'){
       taskSendMessage.disable();
       a="";
    }
  }
}


void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
    digitalWrite(light,LOW);

}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}


// ==================== start of TUNEABLE PARAMETERS ====================
// An IR detector/demodulator is connected to GPIO pin 14
// e.g. D5 on a NodeMCU board.
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
const uint16_t kRecvPin = 14;

const uint32_t kBaudRate = 115200;

const uint16_t kCaptureBufferSize = 1024;

const uint16_t kMinUnknownSize = 12;

const uint8_t kTolerancePercentage = kTolerance;  // kTolerance is normally 25%

#if DECODE_AC
// Some A/C units have gaps in their protocols of ~40ms. e.g. Kelvinator
// A value this large may swallow repeats of some protocols
const uint8_t kTimeout = 50;
#else   // DECODE_AC
// Suits most messages, while not swallowing many repeats.
const uint8_t kTimeout = 15;
#endif  // DECODE_AC

#define LEGACY_TIMING_INFO false
// ==================== end of TUNEABLE PARAMETERS ====================

// Use turn on the save buffer feature for more complete capture coverage.
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;  // Somewhere to store the results

// This section of code runs only once at start-up.
void setup() {
  pinMode(light,OUTPUT);
  digitalWrite(light,HIGH);

  mesh.setDebugMsgTypes(ERROR | STARTUP | DEBUG); // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  
  #if defined(ESP8266)
    Serial.begin(kBaudRate, SERIAL_8N1, SERIAL_TX_ONLY);
  #else  // ESP8266
    Serial.begin(kBaudRate, SERIAL_8N1);
  #endif  // ESP8266
    while (!Serial)  // Wait for the serial connection to be establised.
      delay(50);
    // Perform a low level sanity checks that the compiler performs bit field
    // packing as we expect and Endianness is as we expect.
    assert(irutils::lowLevelSanityCheck() == 0);
  
    Serial.printf("\n" D_STR_IRRECVDUMP_STARTUP "\n", kRecvPin);
  
  #if DECODE_HASH
    // Ignore messages with less than minimum on or off pulses.
    irrecv.setUnknownThreshold(kMinUnknownSize);
  #endif  // DECODE_HASH
    irrecv.setTolerance(kTolerancePercentage);  // Override the default tolerance.
    irrecv.enableIRIn();  // Start the receiver
}

// The repeating section of the code
void loop() {
  // Check if the IR code has been received.
  if (irrecv.decode(&results)) {
    // Display a crude timestamp.
    uint32_t now = millis();
    //Serial.printf(D_STR_TIMESTAMP " : %06u.%03u\n", now / 1000, now % 1000);
    // Check if we got an IR message that was to big for our capture buffer.
    if (results.overflow)
      //Serial.printf(D_WARN_BUFFERFULL "\n", kCaptureBufferSize);
    // Display the library version the message was captured with.
    //Serial.println(D_STR_LIBRARY "   : v" _IRREMOTEESP8266_VERSION_ "\n");
    // Display the tolerance percentage if it has been change from the default.
    if (kTolerancePercentage != kTolerance)
      Serial.printf(D_STR_TOLERANCE " : %d%%\n", kTolerancePercentage);
    // Display the basic output of what we found.
    a=resultToHumanReadableBasic(&results);
    Serial.println(a);
    // Display any extra A/C info if we have it.
    String description = IRAcUtils::resultAcToString(&results);
    //if (description.length()) Serial.println(D_STR_MESGDESC ": " + description);
    //String b="  " + description;
    //Serial.println(b);


    yield();  // Feed the WDT as the text output can take a while to print.
#if LEGACY_TIMING_INFO
    // Output legacy RAW timing info of the result.
    //Serial.println(resultToTimingInfo(&results));
    yield();  // Feed the WDT (again)
#endif  // LEGACY_TIMING_INFO
    // Output the results as source code
    //Serial.println(resultToSourceCode(&results));
    //Serial.println();    // Blank line between entries
    yield();             // Feed the WDT (again)

   taskSendMessage.enable();
    
  }
  mesh.update();
}
