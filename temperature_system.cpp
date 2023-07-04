#include "temperature_system.h"
#include "Arduino.h"
#include "main.h"


void init_temperature_system(void)
{
  /*to config Heater and Cooler pins output*/
    pinMode(HEATER, OUTPUT);
    pinMode(COOLER, OUTPUT);
  /*to turn off heater and cooler initially*/
    digitalWrite(HEATER, LOW);
    digitalWrite(COOLER, LOW);
}

float read_temperature(void)
{
  /*To convert adc value to voltage and then convert to temperature in degree celsius*/
  unsigned int adc_val = analogRead(TEMPERATURE_SENSOR);
   float temperature = ((adc_val*(float)5/1023)/(float)0.01);
   return temperature;
}

/*function to control the cooler*/
void cooler_control(bool control)
{
   if (control){
     digitalWrite(COOLER, HIGH);
   }
   else{
     digitalWrite(COOLER, LOW);
   }
}
/*function to control the cooler*/
void heater_control(bool control)
{
    if (control){
      digitalWrite(HEATER, HIGH);
    }
    else{
      digitalWrite(HEATER, LOW);
    }
}
