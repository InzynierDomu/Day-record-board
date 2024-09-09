#pragma once

#include <stdint.h>

namespace IR_pilot
{

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
  BTN_PLAY = 10,
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
    for (size_t i = 0; i < 15; i++)
    {
      if (map[i].command == command)
      {
        button = map[i].button;
      }
    }
  }
};

Button_command buttons[15] = {Button_command(Button::BTN_0, 22),
                              Button_command(Button::BTN_1, 12),
                              Button_command(Button::BTN_2, 24),
                              Button_command(Button::BTN_3, 94),
                              Button_command(Button::BTN_4, 8),
                              Button_command(Button::BTN_5, 28),
                              Button_command(Button::BTN_6, 90),
                              Button_command(Button::BTN_7, 66),
                              Button_command(Button::BTN_8, 82),
                              Button_command(Button::BTN_9, 74),
                              Button_command(Button::BTN_PLAY, 21),
                              Button_command(Button::BTN_PREVIOUS, 7),
                              Button_command(Button::BTN_NEXT, 9),
                              Button_command(Button::BTN_BACK, 67),
                              Button_command(Button::BTN_ERR, 99)};
} // namespace IR_pilot