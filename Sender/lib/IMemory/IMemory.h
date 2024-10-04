#pragma once

#include <stdint.h>
class IMemory
{
  public:
  ~IMemory() {}
  virtual void save_counters(uint16_t* table) = 0;
  virtual void load_counters(uint16_t* table) = 0;
};