#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <stdint.h>

namespace Memory
{
void save_counters(uint16_t* table)
{
  for (int i = 0; i < 7; i++)
  {
    EEPROM.put<uint16_t>(sizeof(uint16_t) * i, table[i]);
  }
}

void load_counters(uint16_t* table)
{
  for (int i = 0; i < 7; i++)
  {
    uint16_t val;
    EEPROM.get<uint16_t>(sizeof(uint16_t) * i, val);
    table[i] = val;
  }
}
} // namespace Memory
