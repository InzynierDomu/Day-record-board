
#include "Config.h"
#include "IRremote.h"
#include "DMD2.h"
#include "RTClib.h"

#include <SPI.h>
#include <Arduino.h>

///< possible device state
enum class Device_state
{
  startup,
  normal_cunting,
  reset,
  set_rtc
};

Device_state m_device_state = Device_state::startup; ///< actual device state

IRrecv irrecv(Config::pin_led_ir);

RTC_DS3231 rtc;

SoftDMD dmd(Config::screens_count,1);
DMD_TextBox box(dmd);

void setup() {
  Serial.begin(115200);
  
  IrReceiver.begin(Config::pin_led_ir, ENABLE_LED_FEEDBACK);

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

    m_device_state = Device_state::normal_cunting;
}

void button_action()
{
if (IrReceiver.decodedIRData.protocol == NEC) {
      Serial.println("ok protocol");
    }
    else
    {
      Serial.println("wrong protocol");
    }
    Serial.print("command");
    Serial.println(IrReceiver.decodedIRData.command);
    IrReceiver.resume();
}

void loop() {

  if(IrReceiver.decode()){
    button_action();
  }
}
