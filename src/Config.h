#pragma once

#include "fonts/Arial14.h"

#include <Arduino.h>
#include <stdint.h>

namespace Config
{
    const uint8_t pin_led_ir = 11;
    const uint8_t sceens_count = 7;
    const uint8_t *font = Arial14;

} // namespace Config