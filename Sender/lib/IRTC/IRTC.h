#pragma once

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
class IRTC
{
  public:
  virtual ~IRTC() {}
  virtual init() = 0;
  virtual adjust() = 0;
};
