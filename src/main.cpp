#include <Arduino.h>
#include "IRremote.h"

// Ustaw pin, do którego podłączona jest dioda IR
const int RECV_PIN = 11;

// Stwórz obiekt IRrecv do odbierania danych z diody IR
IRrecv irrecv(RECV_PIN);

// Zmienna do przechowywania odebranego sygnału IR
decode_results results;

void setup() {
  // Inicjalizacja komunikacji szeregowej
  Serial.begin(9600);
  
  // Uruchomienie odbiornika IR
  irrecv.enableIRIn(); 
}

void loop() {
  if(irrecv.decode()){
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
    irrecv.resume();
  }
}
