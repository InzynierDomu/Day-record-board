#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace Config
{
const uint8_t pin_led_ir = 11;
const unsigned long baudrate = 2400;
const unsigned long screen_refresh_time = 3600000; // in ms
const unsigned long blink_time = 1800; // in ms

const bool force_save_eeprom = false;
int16_t default_days_counter[7] = {10, 11, 12, 13, 14, 15, 16};
} // namespace Config