
#include "Config.h"
#include "IRremote.h"

#include <Arduino.h>

IRrecv irrecv(Config::pin_led_ir);

void setup() {
  Serial.begin(115200);
  
  irrecv.enableIRIn(); 
}

void loop() {
  if(irrecv.decode()){
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
    irrecv.resume();
  }
}
