#include "Config.h"
#include "IRremote.h"
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

enum command
{
  BTN_0 = 0,
  BTN_1,
  BTN_2,
  BTN_3,
  BTN_4,
  BTN_5,
  BTN_6,
  BTN_7,
  BTN_8,
  BTN_9,
  BTN_PALY = 10,
  BTN_NEXT = 11,
  BTN_PREVIOUS = 12
};

Device_state m_device_state = Device_state::startup; ///< actual device state

IRrecv irrecv(Config::pin_led_ir);

int16_t dataToSend[7] = {1234, 5678, 910, 1112, 1314, 1516, 1718};

const unsigned long m_refresh_time_ms = 30000;

RTC_DS3231 rtc;
void setup() {
  Serial.begin(9600);

   IrReceiver.begin(Config::pin_led_ir, ENABLE_LED_FEEDBACK);
    
   if(!rtc.begin()) {
        Serial.println("Couldn't find RTC!");
        Serial.flush();
        while (1) delay(10);
    }
    if(rtc.lostPower()) {
        
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    rtc.disable32K();

   m_device_state = Device_state::normal_cunting;
}

void button_action()
{
if (IrReceiver.decodedIRData.protocol == NEC) {
    auto command = IrReceiver.decodedIRData.command;
    IrReceiver.resume();
}
}
void loop() {
  if(IrReceiver.decode()){
    button_action();
  }

  static unsigned long last_loop_time = 0;
  unsigned long loop_time = millis();
  if (loop_time - last_loop_time > m_refresh_time_ms)
  {
    for (int i = 0; i < 7; i++) {
      Serial.write((uint8_t*)&dataToSend[i], sizeof(int16_t));
    }
    last_loop_time = millis();
  }
}
