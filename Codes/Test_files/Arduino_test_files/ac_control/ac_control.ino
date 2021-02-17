
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRutils.h>

const uint16_t kIrLed = D2;  // The ESP D2 pin to use that controls the IR LED.
IRac ac(kIrLed);  // Create a A/C object using GPIO to sending messages with.

void setup() {
  Serial.begin(115200);
  delay(200);

  ac.next.protocol = decode_type_t::TCL112AC;  
  ac.next.model = 1;  
  ac.next.mode = stdAc::opmode_t::kCool;
  ac.next.celsius = true;
  ac.next.degrees = 25;
  ac.next.light = false; 
  ac.next.power = false;

  
}

void loop() {
  Serial.println("power off");
  ac.next.power = false;
  ac.sendAc();
  delay(5000);
  Serial.println("power on");
  ac.next.power = true;
  ac.sendAc();
  delay(5000);
}
