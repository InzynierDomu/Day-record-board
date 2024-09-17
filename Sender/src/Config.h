#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace Config
{
const uint8_t pin_led_ir = 11;
const unsigned long baudrate = 2400;
const unsigned long screen_refresh_time = 30000; // in ms
const unsigned long blink_time = 1800; // in ms
} // namespace Config