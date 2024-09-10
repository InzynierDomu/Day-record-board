#pragma once

#include "fonts/Arial14.h"
// #include "fonts/Arial_Black_16.h"
// #include "fonts/Droid_Sans_16.h"

#include <Arduino.h>
#include <stdint.h>

namespace Config
{
const unsigned long baudrate = 2400;

const uint8_t screens_count = 7;
const uint8_t* font = Arial14;
// options: Arial14, Arial_Black_16, Droid_Sans_16
} // namespace Config