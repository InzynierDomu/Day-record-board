#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <stdint.h>

namespace Memory
{
void save_counters(int16_t* table)
{
  for (int i = 0; i < 7; i++)
  {
    EEPROM.put<int16_t>(sizeof(int16_t) * i, table[i]);
  }
}

void load_counters(int16_t* table)
{
  for (int i = 0; i < 7; i++)
  {
    int16_t val;
    EEPROM.get<int16_t>(sizeof(int16_t) * i, val);
    table[i] = val;
  }
}
} // namespace Memory
