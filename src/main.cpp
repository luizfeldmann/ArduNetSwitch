#include "Arduino.h"

extern const char* _binary_public_der_start;
extern const char* _binary_public_der_end;

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}