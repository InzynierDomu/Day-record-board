#include "Config.h"
#include "IR_piolt.h"
#include "IRremote.h"
#include "Memory.h"
#include "RTClib.h"

#include <Arduino.h>
#include <SPI.h>

///< possible device state
enum class Device_state
{
  startup,
  idle,
  refersh,
  reset,
  set_rtc,
  reset_counter
};

Device_state m_device_state = Device_state::startup; ///< actual device state

IRrecv irrecv(Config::pin_led_ir);

int16_t days_counter[7] = {10, 11, 12, 13, 14, 15, 16};

RTC_DS3231 rtc;

long days;

long get_days_from_start()
{
  auto now = rtc.now();
  DateTime startDate(2024, 1, 1, 0, 0, 0);
  TimeSpan timeSpan = now - startDate;
  return timeSpan.days();
}

// void save_counters()
// {
//   for (int i = 0; i < 7; i++)
//   {
//     EEPROM.put<int16_t>(sizeof(int16_t) * i, days_counter[i]);
//   }
// }

// void load_counters()
// {
//   for (int i = 0; i < 7; i++)
//   {
//     int16_t val;
//     EEPROM.get<int16_t>(sizeof(int16_t) * i, val);
//     days_counter[i] = val;
//   }
// }
void setup()
{
  Serial.begin(Config::baudrate);

  IrReceiver.begin(Config::pin_led_ir, ENABLE_LED_FEEDBACK);

  if (!rtc.begin())
  {
    while (1)
      delay(10);
  }
  if (rtc.lostPower())
  {

    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.disable32K();
  days = get_days_from_start();

  Memory::load_counters(days_counter);

  delay(100);
  m_device_state = Device_state::refersh;
}

IR_pilot::Button button_action()
{
  if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)
  {
    IrReceiver.resume();
    return (IR_pilot::BTN_ERR);
  }

  IR_pilot::Button_command command(IR_pilot::Button::BTN_ERR, 99);
  if (IrReceiver.decodedIRData.protocol == NEC)
  {
    auto recive_command = IrReceiver.decodedIRData.command;
    IrReceiver.resume();
    command.check_button(recive_command, IR_pilot::buttons);
    return (command.button);
  }
  IrReceiver.resume();
  return (IR_pilot::BTN_ERR);
}
void show(int16_t* table)
{
  String dataString = "";

  for (int i = 0; i < 7; i++)
  {
    dataString += String(table[i]);
    if (i < 6)
    {
      dataString += ",";
    }
  }

  Serial.println(dataString);
}
void blink(int16_t* table, int8_t blink_screen, bool toggle)
{
  if (toggle)
  {
    String dataString = "";

    for (int i = 0; i < 7; i++)
    {
      if (i == blink_screen)
      {
        dataString += " ";
      }
      else
      {
        dataString += String(table[i]);
      }
      if (i < 6)
      {
        dataString += ",";
      }
    }
    Serial.println(dataString);
  }
  else
  {
    show(table);
  }
}
void refresh()
{
  show(days_counter);

  m_device_state = Device_state::idle;
}

void save_rtc(int8_t* new_val)
{
  DateTime now = rtc.now();
  DateTime set_time(now.year(), now.month(), now.day(), new_val[0], new_val[1]);
  rtc.adjust(set_time);
}

void print_set_rtc(int8_t* set_time, int8_t filed)
{
  static bool toggle;
  int16_t time[7] = {set_time[0], set_time[1], 0, 0, 0, 0, 0};
  static unsigned long last_loop_time = 0;
  unsigned long loop_time = millis();
  if (loop_time - last_loop_time > Config::blink_time)
  {
    blink(time, filed, toggle);
    toggle = !toggle;
    last_loop_time = millis();
  }
}

int get_units_digit(int number)
{
  return number % 10;
}

int get_tens_digit(int number)
{
  return (number / 10) % 10;
}
void set_rtc(IR_pilot::Button action)
{
  static DateTime set_time = rtc.now();
  static int8_t field_to_change = 0;
  static int8_t digit_to_change = 1;
  static int8_t new_val[2]{set_time.hour(), set_time.minute()};
  if (static_cast<int>(action) < 10)
  {
    if (digit_to_change == 1)
    {
      int8_t val = get_tens_digit(new_val[field_to_change]);
      new_val[field_to_change] -= (val * 10);
      new_val[field_to_change] += static_cast<int>(action) * 10;
    }
    else
    {
      int8_t val = get_units_digit(new_val[field_to_change]);
      new_val[field_to_change] -= val;
      new_val[field_to_change] += static_cast<int>(action);
    }
    digit_to_change = !digit_to_change;
  }
  else
  {
    switch (action)
    {
      case IR_pilot::Button::BTN_NEXT:
      {
        field_to_change = !field_to_change;
        digit_to_change = 1;
      }
      break;
      case IR_pilot::Button::BTN_PLAY:
      {
        save_rtc(new_val);
        m_device_state = Device_state::idle;
      }
      break;
      default:
      {
        print_set_rtc(new_val, field_to_change);
      }
      break;
    }
  }
}

void increment_counter(long days_count)
{
  for (int i = 0; i < 6; i++)
  {
    days_counter[i] += days_count;
  }
}

void check_record()
{
  for (int i = 0; i < 6; i++)
  {
    auto check_counter = days_counter[i];
    if (check_counter > days_counter[6])
    {
      days_counter[6] = check_counter;
    }
  }
}

void check_day()
{
  auto now = get_days_from_start();
  if (days < now)
  {
    auto new_days = get_days_from_start() - days;
    increment_counter(new_days);
    check_record();
    Memory::save_counters(days_counter);
    days = now;
  }
}
void idle(IR_pilot::Button action)
{
  switch (action)
  {
    case IR_pilot::Button::BTN_BACK:
      m_device_state = Device_state::reset_counter;
      break;
    case IR_pilot::Button::BTN_PLAY:
      m_device_state = Device_state::set_rtc;
      break;
    default:
    {
      static unsigned long last_loop_time = 0;
      unsigned long loop_time = millis();
      if (loop_time - last_loop_time > Config::screen_refresh_time)
      {
        check_day();
        m_device_state = Device_state::refersh;
        last_loop_time = millis();
      }
    }
    break;
  }
}

void print_reset_counter(int counter_to_reset, bool force = false)
{
  static unsigned long last_loop_time = 0;
  String dataString = "";

  for (int i = 0; i < 7; i++)
  {
    if (i < 6)
    {
      dataString += String(days_counter[i]);
      dataString += ",";
    }
    else
    {
      dataString += String(counter_to_reset);
    }
  }

  if (force)
  {
    Serial.println(dataString);
  }
  else
  {
    unsigned long loop_time = millis();
    if (loop_time - last_loop_time > Config::screen_refresh_time)
    {
      Serial.println(dataString);
      last_loop_time = millis();
    }
  }
}
void reset_cunter(IR_pilot::Button action)
{
  static int counter_to_reset = 1;

  if ((static_cast<int>(action) < 8) && (static_cast<int>(action) > 0))
  {
    counter_to_reset = static_cast<int>(action);
    print_reset_counter(counter_to_reset, true);
  }
  else
  {
    switch (action)
    {
      case IR_pilot::Button::BTN_PLAY:
      {
        days_counter[counter_to_reset - 1] = 0;
        Memory::save_counters(days_counter);
        m_device_state = Device_state::refersh;
      }
      break;
      case IR_pilot::Button::BTN_BACK:
      {
        m_device_state = Device_state::refersh;
      }
      break;
      default:
        print_reset_counter(counter_to_reset);
        break;
    }
  }
}
void loop()
{
  IR_pilot::Button action = IR_pilot::Button::BTN_ERR;
  if (IrReceiver.decode())
  {
    action = button_action();
  }

  switch (m_device_state)
  {
    case Device_state::idle:
    {
      idle(action);
    }
    break;
    case Device_state::refersh:
    {
      refresh();
      m_device_state = Device_state::idle;
    }
    break;
    case Device_state::reset_counter:
      reset_cunter(action);
      break;
    case Device_state::set_rtc:
      set_rtc(action);
      break;
    default:
      break;
  }
}