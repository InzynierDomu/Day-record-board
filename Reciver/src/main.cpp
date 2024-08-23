
#include "DMD2.h"
#include "Config.h"

#include <Arduino.h>

SoftDMD dmd(1,1);
// DMD_TextBox box(dmd);

const int arraySize = 7;
int16_t receivedData[arraySize];

void setup() {
  Serial.begin(9600);

  dmd.setBrightness(255);
  dmd.selectFont(Config::font);
  dmd.begin();

  // box.print("ID");
}

void loop() {
    if (Serial.available() >= arraySize * sizeof(int16_t)) {
    for (int i = 0; i < arraySize; i++) {
      Serial.readBytes((char*)&receivedData[i], sizeof(int16_t));      
    }

    for (int i = 0; i < arraySize; i++)
    {
      String numString = String(receivedData[i]);
      dmd.drawString(i, 0, numString.c_str(), GRAPHICS_ON, Config::font);
    }
    }
}