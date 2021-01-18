#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Fujitsu.h>


const uint16_t power = 1;
const uint16_t kIrLed = 4;  
IRsend ac(kIrLed);


void setup() {
  ac.begin();
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266
  Serial.begin(115200);
  delay(200);

  // Set up what we want to send. See ir_Fujitsu.cpp for all the options.
 // printState();
  
}

void loop() {
   acOn();
   acOff();
}






void acOn() {
  Serial.println("Ac On");
  ac.sendCOOLIX(0xB21FC8);
  delay(5000);
}


void acOff() {
  Serial.println("Ac Off");
  ac.sendCOOLIX(0XB27BE0);
  delay(5000);
}
