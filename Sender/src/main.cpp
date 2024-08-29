#include "Config.h"
#include "IRremote.h"
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

enum Button
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
  BTN_PREVIOUS = 12,
  BTN_BACK = 13,
  BTN_ERR = 99
};

struct Button_command
{
  Button button;
  int16_t command;

  Button_command(Button button_, int16_t command_)
  : button(button_)
  , command(command_)
  {}

  void check_button(int16_t command, Button_command* map)
  {
    for (size_t i = 0; i < 14; i++)
    {
      if (map[i].command == command)
      {
        button = map[i].button;
      }
    }
  }
};

Button_command buttons[14] = {Button_command(Button::BTN_0, 22),
                              Button_command(Button::BTN_1, 12),
                              Button_command(Button::BTN_2, 24),
                              Button_command(Button::BTN_3, 94),
                              Button_command(Button::BTN_4, 8),
                              Button_command(Button::BTN_5, 28),
                              Button_command(Button::BTN_6, 90),
                              Button_command(Button::BTN_7, 66),
                              Button_command(Button::BTN_8, 82),
                              Button_command(Button::BTN_9, 74),
                              Button_command(Button::BTN_PALY, 21),
                              Button_command(Button::BTN_PREVIOUS, 7),
                              Button_command(Button::BTN_NEXT, 9),
                              Button_command(Button::BTN_BACK, 67)};

Device_state m_device_state = Device_state::startup; ///< actual device state

IRrecv irrecv(Config::pin_led_ir);

int16_t days_counter[7] = {1234, 5678, 910, 1112, 1314, 1516, 1718};

const unsigned long m_refresh_time_ms = 30000;

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
  Serial.begin(9600);

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

  m_device_state = Device_state::refersh;
}

Button button_action()
{
  Button_command command(Button::BTN_ERR, 0);
  if (IrReceiver.decodedIRData.protocol == NEC)
  {
    auto recive_command = IrReceiver.decodedIRData.command;
    IrReceiver.resume();
    command.check_button(recive_command, buttons);
    return (command.button);
  }
  return (BTN_ERR);
}

void blink(int16_t* table, int8_t blink_screen)
{
  long loop_time = millis();
  static long time;
  static bool toggle;

  if (loop_time - time > 100)
  {
    time = millis();
    toggle = !toggle;
  }

  if (toggle)
  {}
  else
  {}
}
void show(int16_t* table)
{
  String dataString = "";

  for (int i = 0; i < 7; i++)
  {
    dataString += String(table[i]); // Konwersja liczby na String i dodanie do ciągu
    if (i < 6)
    {
      dataString += ","; // Dodanie przecinka i spacji po każdej liczbie, z wyjątkiem ostatniej
    }
  }

  Serial.println(dataString);
}
void refresh()
{
  show(days_counter);

  m_device_state = Device_state::idle;
}

void set_rtc(Button action)
{
  switch (action)
  {
    case Button::BTN_NEXT:
      break;
    case Button::BTN_PREVIOUS:
      break;
    default:
      break;
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
    days = now;
  }
}
void idle(Button action)
{
  switch (action)
  {
    case Button::BTN_BACK:
      m_device_state = Device_state::reset_counter;
      break;
    default:
    {
      check_day();
      static unsigned long last_loop_time = 0;
      unsigned long loop_time = millis();
      if (loop_time - last_loop_time > m_refresh_time_ms)
      {
        m_device_state = Device_state::refersh;
        last_loop_time = millis();
      }
    }
    break;
  }
}

void reset_cunter(Button action)
{
  days_counter[0] = 0;
  if (action == Button::BTN_PALY)
  {
    m_device_state = Device_state::refersh;
  }
}
void loop()
{
  Button action = Button::BTN_ERR;
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
    default:
      break;
  }
}
