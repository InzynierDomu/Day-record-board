
#include "Config.h"
#include "DMD2.h"

#include <Arduino.h>

SoftDMD dmd(1, Config::screens_count);

String tokens[Config::screens_count];

void setup()
{
  Serial.begin(2400);

  dmd.setBrightness(255);
  dmd.selectFont(Config::font);
  dmd.begin();
}

void loop()
{
  if (Serial.available() > 0)
  {
    String receivedMessage = Serial.readStringUntil('\n');

    char receivedChars[receivedMessage.length() + 1];
    receivedMessage.toCharArray(receivedChars, receivedMessage.length() + 1);

    char* token = strtok(receivedChars, ",");
    int index = 0;

    // Zapisujemy tokeny do tablicy String
    while (token != NULL && index < Config::screens_count)
    {
      tokens[index] = String(token); // Zapisujemy token jako String do tablicy
      token = strtok(NULL, ",");
      index++;
    }

    dmd.clearScreen();
    for (int i = 0; i < Config::screens_count; i++)
    {
      String numString = tokens[i];
      dmd.drawString(0, i * 16, numString.c_str(), GRAPHICS_ON, Config::font);
      Serial.println(numString);
    }
  }
}