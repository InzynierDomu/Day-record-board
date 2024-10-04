#pragma once

#include <string>

class ISerial_port
{
  public:
  virtual ~ISerial_port() {}
  virtual void init() = 0;
  virtual void send(std::string& text) = 0;
};