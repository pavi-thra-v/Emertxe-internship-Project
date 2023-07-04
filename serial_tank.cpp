#include "serial_tank.h"
#include "Arduino.h"
#include "main.h"

unsigned int volume_value;
unsigned char valueh, valuel;

void init_serial_tank(void) 
{
  /*To begin the serial communication between serial tank and arduino board*/
    Serial.begin(19200);
    Serial.write(0xFF); //sincroniza comunicação
    Serial.write(0xFF);
    Serial.write(0xFF);   
}

unsigned int volume(void)
{
    /*to read the volume of water*/
  Serial.write(VOLUME);
  while(!Serial.available()); /*Wait for data*/
  /*reading lower byte*/
  valueh = Serial.read();
  while(!Serial.available()); /*Wait for data*/
  /*reading higher byte*/
  valuel = Serial.read();
  /*Combining Higher byte and Lower byte*/
  volume_value = (valueh << 8) | valuel;
  return volume_value;
}

void enable_inlet(void)
{
    Serial.write(INLET_VALVE);
    Serial.write(ENABLE);
}  
void disable_inlet(void)
{
    Serial.write(INLET_VALVE);
    Serial.write(DISABLE);
}  
void enable_outlet(void)
{  
    Serial.write(OUTLET_VALVE);
    Serial.write(ENABLE);
}
void disable_outlet(void)
{  
    Serial.write(OUTLET_VALVE);
    Serial.write(DISABLE);
}
