#pragma once

#include <stdint.h>
class IReciver
{
  public:
  virtual ~IReciver() {}
  virtual void init() = 0;
  virtual bool is_new_msg();
  virtual uint16_t get_command();
};
