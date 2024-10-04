#pragma once

#include "ISerial_port.h"

class Serial_port : public ISerial_port
{
public:
    Serial_port();
    ~Serial_port () = default;
    void send(std::string& text) override;
};

