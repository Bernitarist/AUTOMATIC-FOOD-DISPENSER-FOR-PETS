/* ***********************************************************
   *                      Global Constants                     *
   *                    Hardware Definitions                   *
   * ********************************************************* */

LiquidCrystal_I2C lcd(0x27, 16, 2);     // instantiates the LiquiCrystal
                                         // Object class to variable lcd
const byte RTC_addr = 0x68;                // I2C address of DS3231 RTC
const byte EEPROM_addr = 0x57;             // I2C address of AT24C32N EEPROM
const bool INTCN = true;                 // allows SQW pin to be monitored
  /* I2C address can be found in the datasheet Figure 1. Device
     Address ZS-040 module has pull-up resistors on these pins
     giving them a default value of 1.
     Shorting an individual pad results in different address:
         pads      Binary    Hex    Dec
     | Default  | b1101111 | 0x57 | 87 |
     | short A0 | b1101110 | 0x56 | 86 |
     | short A1 | b1101101 | 0x55 | 85 |
     | short A2 | b1101011 | 0x53 | 83 |
     | All Short| b1101000 | 0x50 | 80 |
     allowing up to eight combinations                      */
     // instantiate SimpleAlarmClock
SimpleAlarmClock Clock(RTC_addr, EEPROM_addr, INTCN);

const int Snooze_Pin = 11;
const int Lt_Pin = 9;
const int Rt_Pin = 10;
const int DebouceTime = 30;               // button debouce time in ms
Button SnoozeKey(Snooze_Pin, BUTTON_PULLUP_INTERNAL, true, DebouceTime);
Button LtKey(Lt_Pin, BUTTON_PULLUP_INTERNAL, true, DebouceTime);
Button RtKey(Rt_Pin, BUTTON_PULLUP_INTERNAL, true, DebouceTime);

const int LED_Pin = 13;                 // digital pin for LED
const int BUZZER_Pin = 12;              // digital pin for tone buzzer
const int SQW_Pin = 2;                  // Interrrupt pin
const int Button_Hold_Time = 3000;      // button hold length of time in ms
const int Alarm_View_Pause = 2000;      // View Alarm Length of time in ms
const byte SnoozePeriod = 9;            // Snooze value, in minutes
const int flashInterval = 1000;         // Alarm flashing interval
const int SkipClickTime = 60;           // Time in ms to ignore button click

//Alarm types:
const byte Daily = 0;
const byte Weekday = 1;
const byte Weekend = 2;
const byte Once = 3;
//Clock Modes:
const byte AMhr = 0;
const byte PMhr = 1;
const byte M24hr = 2;
//Clocks
const byte clock0 = 0;
const byte alarm1 = 1;
const byte alarm2 = 2;