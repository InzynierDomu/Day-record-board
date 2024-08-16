
#include "Config.h"
#include "IRremote.h"
#include "DMD2.h"
#include "RTClib.h"

#include <SPI.h>
#include <Arduino.h>

IRrecv irrecv(Config::pin_led_ir);

RTC_DS3231 rtc;

const char *MESSAGE = "abcdefghijklmnopqrstuvwxyz";
SoftDMD dmd(Config::sceens_count,1);
DMD_TextBox box(dmd);

void setup() {
  Serial.begin(115200);
  
  irrecv.enableIRIn(); 

  dmd.setBrightness(255);
  dmd.selectFont(Config::font);
  dmd.begin();
  
   if(!rtc.begin()) {
        Serial.println("Couldn't find RTC!");
        Serial.flush();
        while (1) delay(10);
    }

    if(rtc.lostPower()) {
        // this will adjust to the date and time at compilation
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    //we don't need the 32K Pin, so disable it
    rtc.disable32K();
}

void loop() {
  if(irrecv.decode()){
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
    irrecv.resume();
  }
}
