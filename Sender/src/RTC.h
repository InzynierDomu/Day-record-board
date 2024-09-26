#pragma once

#include "IRTC.h"

class RTC : public IRTC
{
  public:
  RTC();
  ~RTC() = default;
  bool init() override;
  void adjust(simple_time time) override;
  DateTime get_date() override;
};
