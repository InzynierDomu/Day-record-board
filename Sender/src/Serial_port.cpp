#include "Arduino.h"
#include "Config.h"
#include "Serial_port.h"


Serial_port::Serial_port() {}

Serial_port::init()
{
  Serial.begin(Config::baudrate);
}

void Serial_port::send(std::string& text) {}