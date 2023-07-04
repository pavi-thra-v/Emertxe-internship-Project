/*************************************************************
Title         :   Home automation using blynk
Description   :   To control light's brigntness with brightness,monitor temperature , monitor water level in the tank through blynk app
Pheripherals  :   Arduino UNO , Temperature system, LED, LDR module, Serial Tank, Blynk cloud, Blynk App.
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPL3SjEkol8V"
#define BLYNK_DEVICE_NAME "Home Automation Device"
#define BLYNK_AUTH_TOKEN "ayYmd476zpY_I5dW3d4zGWWpODDkA27b"


// Comment this out to disable prints 
//#define BLYNK_PRINT Serial

/*Include library for blynk*/
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
/*include library for CLCD*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "main.h"
#include "temperature_system.h"
#include "ldr.h"
#include "serial_tank.h"

char auth[] = BLYNK_AUTH_TOKEN;
bool heater_sw, cooler_sw;
bool inlet_sw,outlet_sw;
unsigned int tank_volume;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// This function is called every time the Virtual Pin 0 state changes
/*To turn ON and OFF cooler based virtual PIN value*/
BLYNK_WRITE(COOLER_V_PIN)
{
  //To read the values on the virtual pin connected to cooler and print the notification on the CLCD
  cooler_sw = param.asInt();
  if (cooler_sw){
    cooler_control(ON); 
    lcd.setCursor(7,0);
    lcd.print("CO_LR ON ");
  }
  else{
    cooler_control(OFF);
    lcd.setCursor(7,0);
    lcd.print("CO_LR OFF");
  }
   
}

/*To turn ON and OFF heater based virtual PIN value*/
BLYNK_WRITE(HEATER_V_PIN )
{
  //To read the values on the virtual pin connected to heater and print notification on the CLCD
  heater_sw = param.asInt();
  if (heater_sw){
    heater_control(ON); 
    lcd.setCursor(7,0);
    lcd.print("HE_TR ON ");
  }
  else{
    heater_control(OFF);
    lcd.setCursor(7,0);
    lcd.print("HE_TR OFF");
  }
}
/*To turn ON and OFF inlet vale based virtual PIN value*/
BLYNK_WRITE(INLET_V_PIN)
{
  inlet_sw = param.asInt();
  if (inlet_sw){
    enable_inlet();
    lcd.setCursor(7,1);
    lcd.print("IN_FL_ON ");
  }
  else{
    disable_inlet();
    lcd.setCursor(7,1);
    lcd.print("IN_FL_OFF");
  }
}
/*To turn ON and OFF outlet value based virtual switch value*/
BLYNK_WRITE(OUTLET_V_PIN)
{
  outlet_sw = param.asInt();
  if (outlet_sw){
    enable_outlet();
    lcd.setCursor(7,1);
    lcd.print("OT_FL_ON ");
  }
  else{
    disable_outlet();
    lcd.setCursor(7,1);
    lcd.print("OT_FL_OFF");
  }
}

/* To display temperature and water volume as gauge on the Blynk App*/  
void update_temperature_reading()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.(use timer to send the temp at particular freq)
  Blynk.virtualWrite(TEMPERATURE_GAUGE, read_temperature());
  Blynk.virtualWrite(WATER_VOL_GAUGE, volume());
}

/*To turn off the heater if the temperature raises above 35 deg C*/
void handle_temp(void)
{
  /*read temperature and compare with 35 and also check if heater is ON*/
  if ((read_temperature()>float(35)) && heater_sw)
  {
    heater_sw = 0;
    heater_control(OFF); //to turn off the heater
    /*to print heater status on the dashboard-CLCDd*/
    lcd.setCursor(7,0);
    lcd.print("HE_TR OFF");
    /*to print notification on the blynk app*/
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Temperature is above 35 degree celcius\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning OFF the heater\n");
    /*to refresh the status on the button widget*/
    Blynk.virtualWrite(HEATER_V_PIN, 0);
  }
}

/*To control water volume above 2000ltrs*/
void handle_tank(void)
{
  /*to check if the volume of the water is less than 2000 litres then enable the inlet valve*/
  if ((tank_volume<2000) && (inlet_sw==0)){
    enable_inlet();
    inlet_sw=1;
    /*print status*/
    lcd.setCursor(7,1);
    lcd.print("IN_FL_ON ");
    /*to print notification on the blynk app*/
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Volume of water in the tank is less than 2000\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning ON the inlet valve\n");

    /*reflecting the status on the button widget*/
    Blynk.virtualWrite(INLET_V_PIN, 1);
  }  
  if ((tank_volume==3000) && (inlet_sw==1)){
    disable_inlet();
    inlet_sw=0;
    /*print status*/
    lcd.setCursor(7,1);
    lcd.print("IN_FL_OFF");
    /*to print notification on the blynk app*/
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Tank is Full \n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning OFF the inlet valve\n");

    /*reflecting the status on the button widget*/
    Blynk.virtualWrite(INLET_V_PIN, 0);
  }  
}


void setup(void)
{
  /*To config Garden Lights as output*/
  init_ldr();

  lcd.init(); //initialize the lcd
  lcd.backlight(); //To turn on the backlight
  lcd.clear(); //Clear the CLCD screen
  lcd.home(); //Set cursor to the first position on the CLCD

  /*Set cursor at particular position*/
  lcd.setCursor(0,0); // Character position=> Column0, Line number=> Row0
  lcd.print("T=");
  /*To set cursor at second line and first position*/
  lcd.setCursor(0,1);
  lcd.print("V=");

  /*To initialize temperature system*/
  init_temperature_system();

  /*To initialize the serial tank*/
  init_serial_tank();

  /*To connect arduino to the blynk cloud*/
  Blynk.begin(auth);

  /*To update temperature and water volume to blynk app for every 0.5 sec i.e two values sent in 1 sec*/
  timer.setInterval(500L, update_temperature_reading); //500ms
}

void loop(void) 
{
  Blynk.run();
  /*to keep timer run*/
  timer.run();
  /*to control the brightness of the LED*/
  brightness_control();

  /*to read temperature and display it on the CLCD*/
  String temp;
  temp = String(read_temperature(), 2); //upto 2 decimal values
  /* to set cursor at Line 2*/
  lcd.setCursor(2,0);
  lcd.print(temp);
  /*to maintain threshold temperature*/
  handle_temp();

  /*to maintain volume of water for 2000*/
  handle_tank();

  /*to read volume of water and display it on the CLCD*/
  tank_volume = volume();
  lcd.setCursor(2,1);
  lcd.print(String(tank_volume));

}
