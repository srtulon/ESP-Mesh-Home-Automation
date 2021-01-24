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
    Serial.println(id);
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
    Serial.println(msg1.substring(1,3));
    Serial.println(msg1[3]);
    Serial.println(msg1[4]);
    Serial.println(msg1.substring(5,7));
    

    
    if(msg1.substring(1,3)=="01"){
        Serial.print("Protocol: AIRWELL");
        ac.next.protocol = decode_type_t::AIRWELL;
     }
    else if(msg1.substring(1,3)=="02"){
        Serial.print("Protocol: AIWA_RC_T501");
        ac.next.protocol = decode_type_t::AIWA_RC_T501;
     }
    else if(msg1.substring(1,3)=="03"){
        Serial.print("Protocol: AMCOR");
        ac.next.protocol = decode_type_t::AMCOR;
     }
    else if(msg1.substring(1,3)=="04"){
        Serial.print("Protocol: ARGO");
        ac.next.protocol = decode_type_t::ARGO;
     }
    else if(msg1.substring(1,3)=="05"){
        Serial.print("Protocol: CARRIER_AC");
        ac.next.protocol = decode_type_t::CARRIER_AC;
     }
    else if(msg1.substring(1,3)=="06"){
        Serial.print("Protocol: CARRIER_AC40");
        ac.next.protocol = decode_type_t::CARRIER_AC40;
     }
    else if(msg1.substring(1,3)=="07"){
        Serial.print("Protocol: CARRIER_AC64");
        ac.next.protocol = decode_type_t::CARRIER_AC64;
     }
    else if(msg1.substring(1,3)=="08"){
        Serial.print("Protocol: COOLIX");
        ac.next.protocol = decode_type_t::COOLIX;
     }
    else if(msg1.substring(1,3)=="09"){
        Serial.print("Protocol: CORONA_AC");
        ac.next.protocol = decode_type_t::CORONA_AC;
     }
    else if(msg1.substring(1,3)=="10"){
        Serial.print("Protocol: DAIKIN");
        ac.next.protocol = decode_type_t::DAIKIN;
     }
    else if(msg1.substring(1,3)=="11"){
        Serial.print("Protocol: DAIKIN128");
        ac.next.protocol = decode_type_t::DAIKIN128;
     }
    else if(msg1.substring(1,3)=="12"){
        Serial.print("Protocol: DAIKIN152");
        ac.next.protocol = decode_type_t::DAIKIN152;
     }
    else if(msg1.substring(1,3)=="13"){
        Serial.print("Protocol: DAIKIN160");
        ac.next.protocol = decode_type_t::DAIKIN160;
     }
    else if(msg1.substring(1,3)=="14"){
        Serial.print("Protocol: DAIKIN176");
        ac.next.protocol = decode_type_t::DAIKIN176;
     }
    else if(msg1.substring(1,3)=="15"){
        Serial.print("Protocol: DAIKIN2");
        ac.next.protocol = decode_type_t::DAIKIN2;
     }
    else if(msg1.substring(1,3)=="16"){
        Serial.print("Protocol: DAIKIN216");
        ac.next.protocol = decode_type_t::DAIKIN216;
     }
    else if(msg1.substring(1,3)=="17"){
        Serial.print("Protocol: DAIKIN64");
        ac.next.protocol = decode_type_t::DAIKIN64;
     }
    else if(msg1.substring(1,3)=="18"){
        Serial.print("Protocol: DELONGHI_AC");
        ac.next.protocol = decode_type_t::DELONGHI_AC;
     }
    else if(msg1.substring(1,3)=="19"){
        Serial.print("Protocol: DENON");
        ac.next.protocol = decode_type_t::DENON;
     }
    else if(msg1.substring(1,3)=="20"){
        Serial.print("Protocol: DISH");
        ac.next.protocol = decode_type_t::DISH;
     }
    else if(msg1.substring(1,3)=="21"){
        Serial.print("Protocol: DOSHISHA");
        ac.next.protocol = decode_type_t::DOSHISHA;
     }
    else if(msg1.substring(1,3)=="22"){
        Serial.print("Protocol: ELECTRA_AC");
        ac.next.protocol = decode_type_t::ELECTRA_AC;
     }
    else if(msg1.substring(1,3)=="23"){
        Serial.print("Protocol: EPSON");
        ac.next.protocol = decode_type_t::EPSON;
     }
    else if(msg1.substring(1,3)=="24"){
        Serial.print("Protocol: FUJITSU_AC");
        ac.next.protocol = decode_type_t::FUJITSU_AC;
     }
    else if(msg1.substring(1,3)=="25"){
        Serial.print("Protocol: GICABLE");
        ac.next.protocol = decode_type_t::GICABLE;
     }
    else if(msg1.substring(1,3)=="26"){
        Serial.print("Protocol: GLOBALCACHE");
        ac.next.protocol = decode_type_t::GLOBALCACHE;
     }
    else if(msg1.substring(1,3)=="27"){
        Serial.print("Protocol: GOODWEATHER");
        ac.next.protocol = decode_type_t::GOODWEATHER;
     }
    else if(msg1.substring(1,3)=="28"){
        Serial.print("Protocol: GREE");
        ac.next.protocol = decode_type_t::GREE;
     }
    else if(msg1.substring(1,3)=="29"){
        Serial.print("Protocol: HAIER_AC");
        ac.next.protocol = decode_type_t::HAIER_AC;
     }
    else if(msg1.substring(1,3)=="30"){
        Serial.print("Protocol: HAIER_AC_YRW02");
        ac.next.protocol = decode_type_t::HAIER_AC_YRW02;
     }
    else if(msg1.substring(1,3)=="31"){
        Serial.print("Protocol: HITACHI_AC");
        ac.next.protocol = decode_type_t::HITACHI_AC;
     }
    else if(msg1.substring(1,3)=="32"){
        Serial.print("Protocol: HITACHI_AC1");
        ac.next.protocol = decode_type_t::HITACHI_AC1;
     }
    else if(msg1.substring(1,3)=="33"){
        Serial.print("Protocol: HITACHI_AC2");
        ac.next.protocol = decode_type_t::HITACHI_AC2;
     }
    else if(msg1.substring(1,3)=="34"){
        Serial.print("Protocol: HITACHI_AC3");
        ac.next.protocol = decode_type_t::HITACHI_AC3;
     }
    else if(msg1.substring(1,3)=="35"){
        Serial.print("Protocol: HITACHI_AC344");
        ac.next.protocol = decode_type_t::HITACHI_AC344;
     }
    else if(msg1.substring(1,3)=="36"){
        Serial.print("Protocol: HITACHI_AC424");
        ac.next.protocol = decode_type_t::HITACHI_AC424;
     }
    else if(msg1.substring(1,3)=="37"){
        Serial.print("Protocol: INAX");
        ac.next.protocol = decode_type_t::INAX;
     }
    else if(msg1.substring(1,3)=="38"){
        Serial.print("Protocol: JVC");
        ac.next.protocol = decode_type_t::JVC;
     }
    else if(msg1.substring(1,3)=="39"){
        Serial.print("Protocol: KELVINATOR");
        ac.next.protocol = decode_type_t::KELVINATOR;
     }
    else if(msg1.substring(1,3)=="40"){
        Serial.print("Protocol: LASERTAG");
        ac.next.protocol = decode_type_t::LASERTAG;
     }
    else if(msg1.substring(1,3)=="41"){
        Serial.print("Protocol: LEGOPF");
        ac.next.protocol = decode_type_t::LEGOPF;
     }
    else if(msg1.substring(1,3)=="42"){
        Serial.print("Protocol: LG");
        ac.next.protocol = decode_type_t::LG;
     }
    else if(msg1.substring(1,3)=="43"){
        Serial.print("Protocol: LG2");
        ac.next.protocol = decode_type_t::LG2;
     }
    else if(msg1.substring(1,3)=="44"){
        Serial.print("Protocol: LUTRON");
        ac.next.protocol = decode_type_t::LUTRON;
     }
    else if(msg1.substring(1,3)=="45"){
        Serial.print("Protocol: MAGIQUEST");
        ac.next.protocol = decode_type_t::MAGIQUEST;
     }
    else if(msg1.substring(1,3)=="46"){
        Serial.print("Protocol: METZ");
        ac.next.protocol = decode_type_t::METZ;
     }
    else if(msg1.substring(1,3)=="47"){
        Serial.print("Protocol: MIDEA");
        ac.next.protocol = decode_type_t::MIDEA;
     }
    else if(msg1.substring(1,3)=="48"){
        Serial.print("Protocol: MIDEA24");
        ac.next.protocol = decode_type_t::MIDEA24;
     }
    else if(msg1.substring(1,3)=="49"){
        Serial.print("Protocol: MITSUBISHI");
        ac.next.protocol = decode_type_t::MITSUBISHI;
     }
    else if(msg1.substring(1,3)=="50"){
        Serial.print("Protocol: MITSUBISHI112");
        ac.next.protocol = decode_type_t::MITSUBISHI112;
     }
    else if(msg1.substring(1,3)=="51"){
        Serial.print("Protocol: MITSUBISHI136");
        ac.next.protocol = decode_type_t::MITSUBISHI136;
     }
    else if(msg1.substring(1,3)=="52"){
        Serial.print("Protocol: MITSUBISHI2");
        ac.next.protocol = decode_type_t::MITSUBISHI2;
     }
    else if(msg1.substring(1,3)=="53"){
        Serial.print("Protocol: MITSUBISHI_AC");
        ac.next.protocol = decode_type_t::MITSUBISHI_AC;
     }
    else if(msg1.substring(1,3)=="54"){
        Serial.print("Protocol: MITSUBISHI_HEAVY_152");
        ac.next.protocol = decode_type_t::MITSUBISHI_HEAVY_152;
     }
    else if(msg1.substring(1,3)=="55"){
        Serial.print("Protocol: MITSUBISHI_HEAVY_88");
        ac.next.protocol = decode_type_t::MITSUBISHI_HEAVY_88;
     }
    else if(msg1.substring(1,3)=="56"){
        Serial.print("Protocol: MULTIBRACKETS");
        ac.next.protocol = decode_type_t::MULTIBRACKETS;
     }
    else if(msg1.substring(1,3)=="57"){
        Serial.print("Protocol: MWM");
        ac.next.protocol = decode_type_t::MWM;
     }
    else if(msg1.substring(1,3)=="58"){
        Serial.print("Protocol: NEC");
        ac.next.protocol = decode_type_t::NEC;
     }
    else if(msg1.substring(1,3)=="59"){
        Serial.print("Protocol: NEC_LIKE");
        ac.next.protocol = decode_type_t::NEC_LIKE;
     }
    else if(msg1.substring(1,3)=="60"){
        Serial.print("Protocol: NEOCLIMA");
        ac.next.protocol = decode_type_t::NEOCLIMA;
     }
    else if(msg1.substring(1,3)=="61"){
        Serial.print("Protocol: NIKAI");
        ac.next.protocol = decode_type_t::NIKAI;
     }
    else if(msg1.substring(1,3)=="62"){
        Serial.print("Protocol: PANASONIC");
        ac.next.protocol = decode_type_t::PANASONIC;
     }
    else if(msg1.substring(1,3)=="63"){
        Serial.print("Protocol: PANASONIC_AC");
        ac.next.protocol = decode_type_t::PANASONIC_AC;
     }
    else if(msg1.substring(1,3)=="64"){
        Serial.print("Protocol: PIONEER");
        ac.next.protocol = decode_type_t::PIONEER;
     }
    else if(msg1.substring(1,3)=="65"){
        Serial.print("Protocol: PRONTO");
        ac.next.protocol = decode_type_t::PRONTO;
     }
    else if(msg1.substring(1,3)=="66"){
        Serial.print("Protocol: RC5");
        ac.next.protocol = decode_type_t::RC5;
     }
    else if(msg1.substring(1,3)=="67"){
        Serial.print("Protocol: RC5X");
        ac.next.protocol = decode_type_t::RC5X;
     }
    else if(msg1.substring(1,3)=="68"){
        Serial.print("Protocol: RC6");
        ac.next.protocol = decode_type_t::RC6;
     }
    else if(msg1.substring(1,3)=="69"){
        Serial.print("Protocol: RCMM");
        ac.next.protocol = decode_type_t::RCMM;
     }
    else if(msg1.substring(1,3)=="70"){
        Serial.print("Protocol: SAMSUNG");
        ac.next.protocol = decode_type_t::SAMSUNG;
     }
    else if(msg1.substring(1,3)=="71"){
        Serial.print("Protocol: SAMSUNG36");
        ac.next.protocol = decode_type_t::SAMSUNG36;
     }
    else if(msg1.substring(1,3)=="72"){
        Serial.print("Protocol: SAMSUNG_AC");
        ac.next.protocol = decode_type_t::SAMSUNG_AC;
     }
    else if(msg1.substring(1,3)=="73"){
        Serial.print("Protocol: SANYO");
        ac.next.protocol = decode_type_t::SANYO;
     }
    else if(msg1.substring(1,3)=="74"){
        Serial.print("Protocol: SANYO_AC");
        ac.next.protocol = decode_type_t::SANYO_AC;
     }
    else if(msg1.substring(1,3)=="75"){
        Serial.print("Protocol: SANYO_LC7461");
        ac.next.protocol = decode_type_t::SANYO_LC7461;
     }
    else if(msg1.substring(1,3)=="76"){
        Serial.print("Protocol: SHARP");
        ac.next.protocol = decode_type_t::SHARP;
     }
    else if(msg1.substring(1,3)=="77"){
        Serial.print("Protocol: SHARP_AC");
        ac.next.protocol = decode_type_t::SHARP_AC;
     }
    else if(msg1.substring(1,3)=="78"){
        Serial.print("Protocol: SHERWOOD");
        ac.next.protocol = decode_type_t::SHERWOOD;
     }
    else if(msg1.substring(1,3)=="79"){
        Serial.print("Protocol: SONY");
        ac.next.protocol = decode_type_t::SONY;
     }
    else if(msg1.substring(1,3)=="80"){
        Serial.print("Protocol: SONY_38K");
        ac.next.protocol = decode_type_t::SONY_38K;
     }
    else if(msg1.substring(1,3)=="81"){
        Serial.print("Protocol: SYMPHONY");
        ac.next.protocol = decode_type_t::SYMPHONY;
     }
    else if(msg1.substring(1,3)=="82"){
        Serial.print("Protocol: TCL112AC");
        ac.next.protocol = decode_type_t::TCL112AC;
     }
    else if(msg1.substring(1,3)=="83"){
        Serial.print("Protocol: TECHNIBEL_AC");
        ac.next.protocol = decode_type_t::TECHNIBEL_AC;
     }
    else if(msg1.substring(1,3)=="84"){
        Serial.print("Protocol: TECO");
        ac.next.protocol = decode_type_t::TECO;
     }
    else if(msg1.substring(1,3)=="85"){
        Serial.print("Protocol: TOSHIBA_AC");
        ac.next.protocol = decode_type_t::TOSHIBA_AC;
     }
    else if(msg1.substring(1,3)=="86"){
        Serial.print("Protocol: TRANSCOLD");
        ac.next.protocol = decode_type_t::TRANSCOLD;
     }
    else if(msg1.substring(1,3)=="87"){
        Serial.print("Protocol: TROTEC");
        ac.next.protocol = decode_type_t::TROTEC;
     }
    else if(msg1.substring(1,3)=="88"){
        Serial.print("Protocol: VESTEL_AC");
        ac.next.protocol = decode_type_t::VESTEL_AC;
     }
    else if(msg1.substring(1,3)=="89"){
        Serial.print("Protocol: VOLTAS");
        ac.next.protocol = decode_type_t::VOLTAS;
     }
    else if(msg1.substring(1,3)=="90"){
        Serial.print("Protocol: WHIRLPOOL_AC");
        ac.next.protocol = decode_type_t::WHIRLPOOL_AC;
     }
    else if(msg1.substring(1,3)=="91"){
        Serial.print("Protocol: WHYNTER");
        ac.next.protocol = decode_type_t::WHYNTER;
     }
    else if(msg1.substring(1,3)=="92"){
        Serial.print("Protocol: ZEPEAL");
        ac.next.protocol = decode_type_t::ZEPEAL;
     }
  }
  Serial.println();
  if(msg1[4]=='0'){
    ac.next.model = msg1[3];
    ac.next.power = false;
  }
  else if(msg1[4]=='1'){
    ac.next.power = true;  // Initially start with the unit off.
    ac.next.model = msg1[3];
    ac.next.degrees = msg1.substring(5,7).toInt();
  }
  ac.sendAc();
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
