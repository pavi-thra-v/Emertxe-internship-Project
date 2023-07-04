#include "ldr.h"
#include "Arduino.h"
#include "main.h"

void init_ldr(void)
{
   pinMode(GARDEN_LIGHT, OUTPUT);
   
}
void brightness_control(void)
{
  unsigned int LDR_val;
  unsigned int dt_cyc;
  /*to read the values from LDR sensor connected to A0 pin*/
  LDR_val = analogRead(LDR_SENSOR);
  LDR_val = 1023 - LDR_val; /*to Reverse Map*/
  dt_cyc = LDR_val/4;
  analogWrite(GARDEN_LIGHT,dt_cyc);
  delay(100);
}
