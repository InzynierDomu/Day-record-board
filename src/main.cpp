
#include "Config.h"
#include "IRremote.h"
// #define IR_SMALLD_NEC
// #include "IRsmallDecoder.h"

// #include "DMD2.h"
#include "RTClib.h"
#include "DMDESP.h"

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
// IRsmallDecoder irDecoder(2); 
// irSmallD_t irData; 

RTC_DS3231 rtc;

// SPIDMD dmd(1,1);
// DMDESP dmd(1,1);
// SoftDMD dmd(1,1);
// DMD_TextBox box(dmd);

const unsigned long m_refresh_time_ms = 30;

void setup() {
  Serial.begin(115200);
  
  IrReceiver.begin(Config::pin_led_ir, ENABLE_LED_FEEDBACK);

  // dmd.beginNoTimer(); 
  // dmd.selectFont(Config::font);
  // dmd.setBrightness(255);
  // dmd.start();
  // dmd.setBrightness(255);
  // dmd.setFont(Config::font);
  
  //  if(!rtc.begin()) {
  //       Serial.println("Couldn't find RTC!");
  //       Serial.flush();
  //       while (1) delay(10);
  //   }

  //   if(rtc.lostPower()) {
  //       // this will adjust to the date and time at compilation
  //       rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //   }
    //we don't need the 32K Pin, so disable it
    // rtc.disable32K();
    // dmd.clearScreen();
    // dmd.drawString(0,0,"ID");

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
    auto command = IrReceiver.decodedIRData.command;
    Serial.println(command);
    // box.print(command);
    IrReceiver.resume();
}

void loop() {
  // dmd.loop();
  if(IrReceiver.decode()){
    button_action();
  }
  
   static unsigned long last_loop_time = 0;
   unsigned long loop_time = millis();
   if (loop_time - last_loop_time > m_refresh_time_ms)
   {
  
  // box.print("ID");
  // dmd.scanDisplay();
    last_loop_time = millis();
   }

  // if (irDecoder.dataAvailable(irData)) {
  //       Serial.print(irData.keyHeld, HEX);    //6th: do something with the data.
  //   Serial.print("\t ");
  //   Serial.print(irData.addr, HEX);
  //   Serial.print("\t ");
  //   Serial.println(irData.cmd, HEX);
  // }
  
}
