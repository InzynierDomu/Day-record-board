
#include "Config.h"
#include "DMD2.h"

#include <Arduino.h>

SoftDMD dmd(1, 2);
// DMD_TextBox box(dmd);

const int arraySize = 7;
// int16_t receivedData[arraySize];
// const int numOfTokens = 7;
String tokens[arraySize];

void setup()
{
  Serial.begin(9600);

  dmd.setBrightness(255);
  dmd.selectFont(Config::font);
  dmd.begin();

  // box.print("ID");
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
    while (token != NULL && index < arraySize)
    {
      tokens[index] = String(token); // Zapisujemy token jako String do tablicy
      token = strtok(NULL, ",");
      index++;
    }

    dmd.clearScreen();
    for (int i = 0; i < 2; i++)
    {
      String numString = String(tokens[i]);
      dmd.drawString(0, i * 16, numString.c_str(), GRAPHICS_ON, Config::font);
    }
  }
}