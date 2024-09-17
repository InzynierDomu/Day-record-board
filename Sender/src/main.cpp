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
  reset_counter,
  set_counter
};

struct simple_time
{
  simple_time(uint8_t _hour, uint8_t _minutes)
  : hour(_hour)
  , minutes(_minutes){};
  uint8_t hour;
  uint8_t minutes;

  bool check_range()
  {
    if (hour > 23)
    {
      return false;
    }
    if (minutes > 59)
    {
      return false;
    }
    return true;
  }
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

void save_rtc(simple_time new_val)
{
  DateTime now = rtc.now();
  DateTime set_time(now.year(), now.month(), now.day(), new_val.hour, new_val.minutes);
  rtc.adjust(set_time);
}

void print_set_rtc(simple_time set_time, int8_t filed)
{
  static bool toggle;
  int16_t time[7] = {set_time.hour, set_time.minutes, 0, 0, 0, 0, 0};
  static unsigned long last_loop_time = 0;
  unsigned long loop_time = millis();
  if (loop_time - last_loop_time > Config::blink_time)
  {
    blink(time, filed, toggle);
    toggle = !toggle;
    last_loop_time = millis();
  }
}

int power_of_10(int exponent)
{
  int result = 1;
  for (int i = 0; i < exponent; i++)
  {
    result *= 10;
  }
  return result;
}
int get_digit_at_position(int number, int position)
{
  return (number / power_of_10(position)) % 10;
}
void set_rtc(IR_pilot::Button action, bool load = false)
{
  static DateTime set_time = rtc.now();
  static int8_t field_to_change = 0;
  static int8_t digit_to_change = 1;
  static simple_time new_val(set_time.hour(), set_time.minute());
  if (load)
  {
    set_time = rtc.now();
    new_val.minutes = set_time.minute();
    new_val.hour = set_time.hour();
  }

  if (static_cast<int>(action) < 10)
  {
    if (field_to_change == 0)
    {
      if (digit_to_change == 1)
      {
        int8_t val = get_digit_at_position(new_val.hour, 1);
        new_val.hour -= (val * 10);
        new_val.hour += static_cast<int>(action) * 10;
      }
      else
      {
        int8_t val = get_digit_at_position(new_val.hour, 0);
        new_val.hour -= val;
        new_val.hour += static_cast<int>(action);
      }
      digit_to_change = !digit_to_change;
    }
    else
    {
      if (digit_to_change == 1)
      {
        int8_t val = get_digit_at_position(new_val.minutes, 1);
        new_val.minutes -= (val * 10);
        new_val.minutes += static_cast<int>(action) * 10;
      }
      else
      {
        int8_t val = get_digit_at_position(new_val.minutes, 0);
        new_val.minutes -= val;
        new_val.minutes += static_cast<int>(action);
      }
      digit_to_change = !digit_to_change;
    }
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
        if (new_val.check_range())
        {
          save_rtc(new_val);
          digit_to_change = 1;
          m_device_state = Device_state::refersh;
        }
      }
      break;
      case IR_pilot::Button::BTN_BACK:
      {
        digit_to_change = 1;
        m_device_state = Device_state::refersh;
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

bool check_size(int16_t* table)
{
  for (size_t i = 0; i < 7; i++)
  {
    if (table[i] > 32767)
    {
      return false;
    }
  }
  return true;
}

void print_set_counter(int16_t* counters, int8_t actual_field)
{
  static bool toggle;
  static unsigned long last_loop_time = 0;
  unsigned long loop_time = millis();
  if (loop_time - last_loop_time > Config::blink_time)
  {
    blink(counters, actual_field, toggle);
    toggle = !toggle;
    last_loop_time = millis();
  }
}
void set_counter(IR_pilot::Button action, bool load = false)
{
  static int16_t new_days_counter[7] = {10, 11, 12, 13, 14, 15, 16};
  static int8_t actual_field = 0;
  static int8_t multiply = 0;

  if (load)
  {
    for (size_t i = 0; i < 7; i++)
    {
      new_days_counter[i] = days_counter[i];
    }
  }

  if (static_cast<int>(action) < 10)
  {
    int16_t val = get_digit_at_position(new_days_counter[actual_field], multiply);
    Serial.println(val);
    new_days_counter[actual_field] -= (val * power_of_10(multiply));
    Serial.println(new_days_counter[actual_field]);
    Serial.println(power_of_10(multiply));
    new_days_counter[actual_field] += (static_cast<int>(action) * power_of_10(multiply));
    Serial.println(new_days_counter[actual_field]);
    if (multiply < 5)
    {
      multiply++;
    }
  }
  else
  {
    switch (action)
    {
      case IR_pilot::Button::BTN_PLAY:
      {
        if (check_size(new_days_counter))
        {
          for (size_t i = 0; i < 7; i++)
          {
            days_counter[i] = new_days_counter[i];
          }
          multiply = 0;
          m_device_state = Device_state::refersh;
          refresh();
        }
      }
      break;
      case IR_pilot::Button::BTN_BACK:
      {
        m_device_state = Device_state::refersh;
        multiply = 0;
        refresh();
      }
      break;
      case IR_pilot::Button::BTN_NEXT:
        if (actual_field < 7)
        {
          actual_field++;
          multiply = 0;
        }
        break;
      case IR_pilot::Button::BTN_PREVIOUS:
        if (actual_field > 0)
        {
          actual_field--;
          multiply = 0;
        }
        break;
      default:
        print_set_counter(new_days_counter, actual_field);
        break;
    }
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
    {
      m_device_state = Device_state::reset_counter;
      reset_cunter(IR_pilot::Button::BTN_ERR);
    }
    break;
    case IR_pilot::Button::BTN_PLAY:
    {
      m_device_state = Device_state::set_rtc;
      set_rtc(IR_pilot::Button::BTN_ERR, true);
    }
    break;
    case IR_pilot::Button::BTN_NEXT:
    {
      m_device_state = Device_state::set_counter;
      set_counter(IR_pilot::Button::BTN_ERR, true);
    }
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
    case Device_state::set_counter:
      set_counter(action);
      break;
    default:
      break;
  }
}