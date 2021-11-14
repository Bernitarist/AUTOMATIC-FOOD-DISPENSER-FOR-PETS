 /* ***********************************************************
  *                         Libraries                         *
  * ********************************************************* */
  
#include <SimpleAlarmClock.h>         
#include <LiquidCrystal_I2C.h>
#include <Button.h>
#include <Servo.h>   
#include "HX711.h"                 
#include "pitches.h"

#define calibFacPet 216.0 
#define doutPet 6
#define clkPet 5

#define calibFacPlate 218.5 
#define doutPlate 4
#define clkPlate 3

  /* ***********************************************************
   *                      Global Constants                     *
   *                    Hardware Definitions                   *
   * ********************************************************* */



const byte RTC_addr = 0x68;      // I2C address of DS3231 RTC
const byte EEPROM_addr = 0x57;  // I2C address of AT24C32N EEPROM
const bool INTCN = true;       // allows SQW pin to be monitored

const int Snooze_Pin = 11;
const int Lt_Pin = 9;
const int Rt_Pin = 10;
const int doorPin = 12;
     
SimpleAlarmClock Clock(RTC_addr, EEPROM_addr, INTCN);  // SimpleAlarmClock object
LiquidCrystal_I2C lcd(0x27, 16, 2);                   // LiquiCrystal object
HX711 scale;
HX711 pWeight;
Servo door;

const int DebouceTime = 30;    // button debouce time in ms
Button SnoozeKey(Snooze_Pin, BUTTON_PULLUP_INTERNAL, true, DebouceTime);
Button LtKey(Lt_Pin, BUTTON_PULLUP_INTERNAL, true, DebouceTime);
Button RtKey(Rt_Pin, BUTTON_PULLUP_INTERNAL, true, DebouceTime);

const int LED_Pin = 13;      // digital pin for LED
const int greenLed = 8;     // digital pin for LED
const int BUZZER_Pin = 7;  // digital pin for tone buzzer
const int SQW_Pin = 2;    // Interrrupt pin

const int Button_Hold_Time = 2000;   // button hold length of time in ms
const int Alarm_View_Pause = 2000;  // View Alarm Length of time in ms
const int FeedAmtPauseTime = 3000;
const byte SnoozePeriod = 9;      // Snooze value, in minutes
const int flashInterval = 1000;  // Alarm flashing interval
const int SkipClickTime = 60;   // Time in ms to ignore button click

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


/* ***********************************************************
 *                      Global Variables                     *
 * ********************************************************* */
 
int melody[] = {

    NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6,  // notes in the melody:
};
int noteDurations[] = { 16, 16, 16, 16, 16 };  // note durations: 4 = quarter note, 8 = eighth note, etc.

enum States {
    PowerLoss,
    ShowClock,
    ShowAlarm1,
    ShowAlarm2,
    ShowFeedAmt,
    Alarm,
    DoorOpening,
    EditClock,
    EditAlarm1,
    EditAlarm2,
    EditFeedAmt,
};

States ClockState = ShowClock;
States PrevState = EditAlarm2;  

byte HourType = 0;        // 0=AM/PM, 1=24hour - used in display alarm - to be deleted
byte EditHourType = 0;   // 0=AM, 1=PM, 2=24hour - used for edit only
byte ActiveAlarms = 0;  // used to store active alarms (not enabled alarms)
byte cpIndex = 0;      // Cursor Position Index - used for edit mode

bool Fahrenheit = true;             // Or Celsius=false
bool PrevFahrenheit = Fahrenheit;  // Capture previous Fahrenheit
bool bHoldButtonFlag = false;     // used to prevent holdButton also activating clickButton
bool bDisplayStatus = true;      // used to track the lcd display on status
bool animalPresent;             // check if pet is present

float CurrentTemperature;  // Maybe move as static variable under displayClock function
float feedRem;
float foodBackup;

int minAngle = 0;
int maxAngle = 180;

unsigned int MaxAmtfood = 5000;
unsigned long RunTime;                    // Used to track time between get temperature value
unsigned long buttonHoldPrevTime = 0.0;  // Used to track button hold times 
unsigned long AlarmRunTime;
unsigned long doorStartTime = 0;
unsigned long doorInterval = 500;
unsigned long plateStartTime = 0;
unsigned long plateInterval = 1000;

DateTime PreviousTime;     // Maybe move as static variable under displayClock function
AlarmTime PreviousAlarm;  // Maybe move as static variable under displayAlarm function

//custom LCD characters
byte cA1[8] = {
              0b00100,
              0b01110,
              0b11111,  //Up arrow
              0b00000,
              0b00000,
              0b00000,
              0b00000,
              0b00000 };
byte cA2[8] = {
              0b00000,
              0b00000,
              0b00000,  //Down arrow
              0b00000,
              0b00000,
              0b11111,
              0b01110,
              0b00100 };
byte cBA[8] = {
              0b00100,
              0b01110,
              0b11111,  //Both arrows
              0b00000,
              0b00000,
              0b11111,
              0b01110,
              0b00100 };


/* ***********************************************************
 *                         Functions                         *
 * ********************************************************* */

void displayClock(bool changeFlag = false) {
    /* ***************************************************** *
     *   changeFlag - true forces display refresh
     *              - false does nothing
     * ***************************************************** */
    
    digitalWrite(greenLed, HIGH);
    DateTime NowTime;            //create DateTime struct from Library
    NowTime = Clock.read();      // get the latest clock values

    // Check the temperature every 65 seconds OR if Fahrenheit changes
    unsigned long uL = millis() - RunTime;
    if ((uL >= 65000) || (Fahrenheit != PrevFahrenheit)) {
        float PreviousTemperature = CurrentTemperature;
        CurrentTemperature = getTemperatureValue();
        RunTime = millis();
        PrevFahrenheit = Fahrenheit;
        if (CurrentTemperature != PreviousTemperature) { changeFlag = true; }
    }

    // Check for Time change
    if (NowTime.Hour != PreviousTime.Hour) { changeFlag = true; }
    if (NowTime.Minute != PreviousTime.Minute) { changeFlag = true; }
    if (NowTime.ClockMode != PreviousTime.ClockMode) { changeFlag = true; }
    if (NowTime.Day != PreviousTime.Day) { changeFlag = true; }
    if (NowTime.Month != PreviousTime.Month) { changeFlag = true; }
    if (NowTime.Year != PreviousTime.Year) { changeFlag = true; }

    //Update Display - Only change display if change is detected
    if (changeFlag == true) {
        lcd.backlight();
        lcd.clear();

        //First Row  hh:mm AM ###.#°F
        lcd.setCursor(0, 0);                       //Column, Row
        lcd.print(p2Digits(NowTime.Hour));
        lcd.print(":");
        lcd.print(p2Digits(NowTime.Minute));
        switch (NowTime.ClockMode) {
        case AMhr:
            lcd.print(" AM ");
            break;
        case PMhr:
            lcd.print(" PM ");
            break;
        case M24hr:
            lcd.print("  H ");
            break;
        default:
            //do nothing
            break;
        }
        if (CurrentTemperature < 100.0) { lcd.print(" "); }
        lcd.print(String(CurrentTemperature, 1));  // converts float to string
                                                  // with 1 decimal place
        lcd.print((char)223);                     // prints the degree symbol
        if (Fahrenheit) { lcd.print("F"); }
        else { lcd.print("C"); }
                
        lcd.setCursor(0, 1);                       // Column, Row
        lcd.print("Nxt Feed ");
               
        DisplayNextFeed();

        lcd.setCursor(15, 1); 
        lcdAlarmIndicator();                      //lcd.print A1, A2, BA, or -

        PreviousTime = Clock.read();
}
}

void DisplayNextFeed(){
  
  unsigned int timeRem;
  byte smallh[8] = {
              0b00000,
              0b10000,
              0b10000,
              0b10000,
              0b11100,
              0b10100,
              0b10100,
              0b00000 };

  lcd.createChar(0, smallh);

    DateTime NowTime;            
    NowTime = Clock.read(); 
    AlarmTime alarm;

  struct TIME
{
    int minutes;
    int hours;
};
    struct TIME alm1, alm2, difference;
   
          //alarm1 values
     alarm = Clock.readAlarm(alarm1);
     alm1.hours = alarm.Hour;
     alm1.minutes = alarm.Minute;
  
          //alarm2 values
    alarm = Clock.readAlarm(alarm2);
     alm2.hours = alarm.Hour;
     alm2.minutes = alarm.Minute;

            //clock values
    int clockHr = NowTime.Hour;
    int clockMin = NowTime.Minute;
    
          
          //time difference calculation
   if((NowTime.ClockMode == AMhr && clockHr > alm1.hours) || (NowTime.ClockMode == AMhr && clockHr == alm1.hours && clockMin > alm1.minutes) || (NowTime.ClockMode == PMhr && clockHr < alm2.hours) 
          || (clockHr == alm2.hours && NowTime.ClockMode == PMhr && clockMin < alm2.minutes) ||(NowTime.ClockMode == PMhr && clockHr == 12)){
            
              if(alm2.minutes < clockMin){
                --alm2.hours;
                alm2.minutes += 60;
                difference.minutes = alm2.minutes - clockMin; 
              }
              else{difference.minutes = alm2.minutes - clockMin;}
              if(alm2.hours < clockHr){
                alm2.hours += 12;
                difference.hours = alm2.hours- clockHr; 
              }
              else{difference.hours = alm2.hours - clockHr;}

                  lcd.setCursor(9, 1);
                  lcd.print(p2Digits(difference.hours));
                  lcd.write(byte(0));
                  lcd.print(p2Digits(difference.minutes));
                  lcd.print("m");

                 
     }
      if(NowTime.ClockMode == AMhr && clockHr < alm1.hours || clockHr == 12 && NowTime.ClockMode == AMhr || (NowTime.ClockMode == AMhr && clockHr == alm1.hours && clockMin < alm1.minutes)){
              if(alm1.minutes < clockMin){
                --alm1.hours;
                alm1.minutes += 60;
                difference.minutes = alm1.minutes - clockMin; 
              }
              else{difference.minutes = alm1.minutes - clockMin;}
              if(alm1.hours < clockHr){
                alm1.hours += 12;
                difference.hours = alm1.hours - clockHr;
              }
                else{difference.hours = alm1.hours - clockHr;} 

                  lcd.setCursor(9, 1);
                  lcd.print(p2Digits(difference.hours));
                  lcd.write(byte(0));
                  lcd.print(p2Digits(difference.minutes));
                  lcd.print("m");

                  
     }
     if(NowTime.ClockMode == PMhr && clockHr > alm2.hours || (clockHr == alm2.hours && NowTime.ClockMode == PMhr && clockMin > alm2.minutes) ){
            alm1.hours += 24;
            clockHr += 12;
                if(alm1.minutes < clockMin){
                      --alm1.hours;
                      alm1.minutes += 60;
                      difference.minutes = alm1.minutes - clockMin;
                }
                else{difference.minutes = alm1.minutes - clockMin;}

                difference.hours = alm1.hours - clockHr;
                  
                  lcd.setCursor(9, 1);
                  lcd.print(p2Digits(difference.hours));
                  lcd.write(byte(0));
                  lcd.print(p2Digits(difference.minutes));
                  lcd.print("m");
                 
     }
}

void displayAlarm(byte index = 1, bool changeFlag = false) {
    
    digitalWrite(greenLed, LOW);
    AlarmTime alarm;            //create AlarmTime struct from Library

    if (index == alarm2) {
        alarm = Clock.readAlarm(alarm2);      // get the latest alarm2 values
    }
    else {
        alarm = Clock.readAlarm(alarm1);      // get the latest alarm1 values
    }

    // Check for Alarm change
    if (alarm.Hour != PreviousAlarm.Hour) { changeFlag = true; }
    if (alarm.Minute != PreviousAlarm.Minute) { changeFlag = true; }
    if (alarm.ClockMode != PreviousAlarm.ClockMode) { changeFlag = true; }
    if (alarm.AlarmMode != PreviousAlarm.AlarmMode) { changeFlag = true; }

    //Update Display - Only change display if change is detected
    if (changeFlag == true) {
        lcd.clear();

        // First row
        lcd.setCursor(0, 0);
        if (index == alarm2) {
            lcd.print("Alarm 2");
        }
        else {
            lcd.print("Alarm 1");
        }
        lcd.setCursor(13, 0);
        if (alarm.Enabled == true) {
            lcd.print("ON");
        }
        else {
            lcd.print("OFF");
        }

        //Second row
        lcd.setCursor(0, 1);
        lcd.print(p2Digits(alarm.Hour));
        lcd.print(":");
        lcd.print(p2Digits(alarm.Minute));
        switch (alarm.ClockMode) {
        case AMhr:
            //AM
            lcd.print(" AM");
            break;
        case PMhr:
            //PM
            lcd.print(" PM");
            break;
        case M24hr:
            //24hr
            lcd.print("  M");
            break;
        default:
            lcd.print("  M");
            break;
        }
        switch (alarm.AlarmMode) {
            //0=Daily, 1=Weekday, 2=Weekend, 3=Once
        case 0:
            //Daily
            lcd.print(" Daily");
            break;
        case 1:
            //Weekday
            lcd.print(" Weekday");
            break;
        case 2:
            //Weekend
            lcd.print(" Weekend");
            break;
        case 3:
            //Once
            lcd.print(" Once");
            break;
        default:
            //do nothing
            break;
        }
        PreviousAlarm = alarm;
    }
}

void changeHour(byte i = clock0, bool increment = true) {
    /*  Increments or decrements the hour by one
     *    i = 0 Clock
     *      = 1 Alarm1
     *      = 2 Alarm2
     */
    AlarmTime alarm;
    DateTime NowTime;                  //create DateTime struct from Library
    int Hour;
    byte ClockMode;

    switch (i) {
    case clock0:
        NowTime = Clock.read();        // get the latest clock values
        Hour = NowTime.Hour;
        ClockMode = NowTime.ClockMode;
        break;
    case alarm1:
        //alarm1
        alarm = Clock.readAlarm(alarm1);
        Hour = alarm.Hour;
        ClockMode = alarm.ClockMode;
        break;
    case alarm2:
        //alarm2
        alarm = Clock.readAlarm(alarm2);
        Hour = alarm.Hour;
        ClockMode = alarm.ClockMode;
        break;
    default:
        //Clock
        NowTime = Clock.read();      // get the latest clock values
        Hour = NowTime.Hour;
        ClockMode = NowTime.ClockMode;
        break;
    }
    switch (ClockMode) {
    case AMhr:
    case PMhr:
        if (increment == true) {
            Hour += 1;
            Hour %= 12;
        }
        else {
            Hour -= 1;
            Hour %= 12;
        }
        if (Hour <= 0) { Hour = 12; }
        break;
    case M24hr:
        if (increment == true) {
            Hour += 1;
            Hour %= 24;
        }
        else {
            Hour -= 1;
            Hour %= 24;
        }
        if (Hour < 0) { Hour = 23; }
        //Serial.print("24Hour = ");Serial.println(Hour);
        break;
    default:
        //do nothing
        break;
    }
    switch (i) {
    case clock0:
        //Clock
        NowTime.Hour = byte(Hour);
        Clock.write(NowTime);
        break;
    case alarm1:
        //alarm1
        Serial.println("Setting Alarm1");
        alarm.Hour = byte(Hour);
        Serial.print("alarm.Hour = "); Serial.println(alarm.Hour);
        Clock.setAlarm(alarm, 1);
        break;
    case alarm2:
        //alarm2
        alarm.Hour = Hour;
        Clock.setAlarm(alarm, 2);
        break;
    default:
        //Clock
        NowTime.Hour = Hour;
        Clock.write(NowTime);
        break;
    }
  
}

void changeMinute(byte i = 0, bool increment = true) {
    /*  Increments or decrements the minute by one
     *    i = 0 Clock
     *      = 1 Alarm1
     *      = 2 Alarm2
     */
    AlarmTime alarm;
    DateTime NowTime;            //create DateTime struct from Library
    int Minute;

    switch (i) {
    case clock0:
        //Clock
        NowTime = Clock.read();        // get the latest clock values
        Minute = NowTime.Minute;
        break;
    case alarm1:
        //alarm1
        alarm = Clock.readAlarm(alarm1);
        Minute = alarm.Minute;
        break;
    case alarm2:
        //alarm2
        alarm = Clock.readAlarm(alarm2);
        Minute = alarm.Minute;
        break;
    default:
        //Clock
        NowTime = Clock.read();        // get the latest clock values
        Minute = NowTime.Minute;
        break;
    }
    if (increment == true) {
        Minute += 1;
        Minute %= 60;
    }
    else {
        Minute -= 1;
        Minute %= 60;
    }
    // Note a byte is from 0-255, no negative number
    // that's why we need an int here
    if (Minute < 0) { Minute = 59; }
    switch (i) {
    case clock0:
        //Clock
        NowTime.Minute = byte(Minute);
        Clock.write(NowTime);
        break;
    case alarm1:
        //alarm1
        alarm.Minute = byte(Minute);
        Clock.setAlarm(alarm, 1);
        break;
    case alarm2:
        //alarm2
        alarm.Minute = byte(Minute);
        Clock.setAlarm(alarm, 2);
        break;
    default:
        //Clock
        NowTime.Minute = byte(Minute);
        Clock.write(NowTime);
        break;
    }
    //TODO: Error checking. Would return 0 for fail and 1 for OK
}

void changeClockMode(byte i = 0, bool increment = true) {
    /*  Change Clock's ClockMode to AM=0, PM=1 or 24=2
     *  Limited change of Alarm's ClockMode to AM or PM
     *  or no change if 24hr
     *    i = 0 Clock0
     *      = 1 Alarm1
     *      = 2 Alarm2
     */
    AlarmTime alarm;
    DateTime NowTime = Clock.read();     //create DateTime struct from Library
    int ClockMode = NowTime.ClockMode;   //int is able to be negative

    switch (i) {
    case clock0:
        //Clock
        if (increment == true) {
            ClockMode += 1;
            ClockMode %= 3;
        }
        else {
            ClockMode -= 1;
            ClockMode %= 3;
        }
        if (ClockMode < 0) { ClockMode = 2; }
        NowTime.ClockMode = byte(ClockMode);
        Clock.write(NowTime);
        fixAlarmClockMode(alarm1, NowTime.ClockMode);
        fixAlarmClockMode(alarm2, NowTime.ClockMode);
        break;
    case alarm1:
    case alarm2:
        //alarm1 or alarm2
        if (ClockMode != M24hr) {
            alarm = Clock.readAlarm(i);
            if (alarm.ClockMode == AMhr) {
                alarm.ClockMode = PMhr;
            }
            else {
                alarm.ClockMode = AMhr;
            }
            Clock.setAlarm(alarm, i);
        } //else do nothing
        break;
    default:
        //do nothing
        break;
    }
    //TODO: Error checking. Would return 0 for fail and 1 for OK
}

void changeAlarmMode(byte i = 1, bool increment = true) {
    /*  Change AlarmMode to 0=Daily, 1=Weekday, 2=Weekend, 3=Once
     *    i = 1 Alarm1
     *      = 2 Alarm2
     */
    if ((i == 1) || (i = 2)) {
        // Instantiates object as struct AlarmTIme
        AlarmTime alarm = Clock.readAlarm(i);
        int AlarmMode = alarm.AlarmMode;;

        if (increment == true) {
            AlarmMode += 1;
            AlarmMode %= 4;
        }
        else {
            AlarmMode -= 1;
            AlarmMode %= 4;
        }

        if (AlarmMode < 0) { AlarmMode = 3; }
        alarm.AlarmMode = byte(AlarmMode);
        Clock.setAlarm(alarm, i);
    }//TODO: Error checking. Would return 0 for fail and 1 for OK
}

void changeTemp() {
    //change the temperature to F or C
    Fahrenheit = !Fahrenheit;
    CurrentTemperature = getTemperatureValue();
    RunTime = millis();
    displayClock(true);
}

void changeMonth(byte i = 0, bool increment = true) {
    DateTime NowTime;
    NowTime = Clock.read();
    int Month = NowTime.Month;
    if (increment == true) {
        Month += 1;
    }
    else {
        Month -= 1;
    }
    if (Month > 12) { Month = 1; }
    if (Month < 1) { Month = 12; }
    NowTime.Month = byte(Month);
    Clock.write(NowTime);
}

void changeDay(byte i = 0, bool increment = true) {
    DateTime NowTime;
    NowTime = Clock.read();
    int Day = NowTime.Day;
    byte Month = NowTime.Month;
    byte Year = NowTime.Year + 2000;
    byte DaysMax = 31;
    switch (Month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        DaysMax = 31;
        break;
    case 2:
        DaysMax = 28;
        if ((Year % 4 == 0) && (Year % 100 != 0) || (Year % 400 == 0)) {
            //those are the conditions to have a leap year
            DaysMax = 29;
        }
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        DaysMax = 30;
        break;
    default:
        break;
    }
    if (increment == true) { Day += 1; }
    else { Day -= 1; }
    if (Day < 1) { Day = DaysMax; }
    if (Day > DaysMax) { Day = 1; }
    NowTime.Day = byte(Day);
    Clock.write(NowTime);
}

void changeYear(byte i = 0, bool increment = true) {
    DateTime NowTime;
    NowTime = Clock.read();
    int Year = NowTime.Year;
    if (increment == true) {
        Year += 1;
    }
    else {
        Year -= 1;
    }
    if (Year < 18) { Year = 199; }
    if (Year > 199) { Year = 18; }
    NowTime.Year = byte(Year);
    Clock.write(NowTime);
}

void changeFeedAmt(bool increment = true){

      float maxAmt = 5000;
      float minAmt = 1000;
 
    if (increment == true) { MaxAmtfood += 1000; }
    else { MaxAmtfood -= 1000; }

      if (MaxAmtfood < 1000) { MaxAmtfood = maxAmt; }
    if (MaxAmtfood > 5000) { MaxAmtfood = minAmt; }

    lcd.setCursor(0,1);
    lcd.print(MaxAmtfood);

}

void foodParam(bool change = false){
     
  if (change == true){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set Max Feed Amt ");
  lcd.setCursor(0,1);
  lcd.print(Amt2Digits(MaxAmtfood));
  lcd.print("g");
  }
  
}

void fixAlarmClockMode(byte alarmIndex, byte NewClockMode) {
    /** ********************************************************
     * Fixes alarm clockmode if clock.clockmode is switch
     * between 12hr and 24hr clockmodes
     ********************************************************* */
    AlarmTime alarm = Clock.readAlarm(alarmIndex);
    //Clock Modes:
    //const byte AMhr=0;
    //const byte PMhr=1;
    //const byte M24hr=2;

    //TODO: check alarmIndex, check NewClockMode
    if ((NewClockMode == AMhr) || (NewClockMode == PMhr)) {
        if (alarm.Hour > 12) {
            alarm.ClockMode = PMhr;
        }
        else {
            alarm.ClockMode = AMhr;
        }
        alarm.Hour %= 12;
        if (alarm.Hour == 0) { alarm.Hour = 12; }
    }
    else if (NewClockMode == M24hr) {
        //Convert to 24hr
        alarm.Hour %= 12;
        alarm.Hour += (12 * alarm.ClockMode);
        alarm.ClockMode = M24hr;
    }
    Clock.setAlarm(alarm, alarmIndex);

}

void toggleShowAlarm(byte i = 1) {
    if ((i == 1) || (i == 2)) {
        if (i == 2) {
            ClockState = ShowAlarm2;
        }
        else {
            ClockState = ShowAlarm1;
        }
        AlarmTime alarm;
        alarm = Clock.readAlarm(i);
        alarm.Enabled = !alarm.Enabled;
        Clock.setAlarm(alarm, i);
        AlarmRunTime = millis();
        displayAlarm(i, true);
    }
    //otherwise do nothing
}

void toggleLED(bool ledON = true) {
     bool ledState;

    ledState = digitalRead(LED_Pin);                //get the state of LED
    if (ledON == true) {
        digitalWrite(LED_Pin, !ledState);           //do the opposite
    }
    else {
        digitalWrite(LED_Pin, LOW);
    }
    
}

void toggleBuzzer() {
  
      /* Plays alarm beeps and ends               */
    /* Code found from arduino web site         */
    for (int thisNote = 0; thisNote < 5; thisNote++) {
        // to calculate the note duration, take one second divided by the note type.
        // e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(BUZZER_Pin, melody[thisNote], noteDuration);

        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(BUZZER_Pin);
    }
}

bool checkPlate(){
  if((scale.get_units(5)) <= 5){
    return true;
  }
  else{return false;}
}

void openDoor(){

    bool plateEmpty;
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Feeding...");

    if((scale.get_units(5)) <= 5){
      door.write(maxAngle);
    }
    else { 
      if ((millis() - plateStartTime) >= plateInterval) { 
        plateStartTime = millis();
        plateEmpty = checkPlate();
         }       
      }
        if (plateEmpty == true){
          if((pWeight.get_units(5)) > 8 && (pWeight.get_units(5)) <= 18){
           if ((millis() - doorStartTime) >= doorInterval){
                doorStartTime = millis();
                closeDoor(0);
            }           
        }
        if((pWeight.get_units(5)) > 18 && (pWeight.get_units(5)) <= 23){
           if ((millis() - doorStartTime) >= doorInterval){
                doorStartTime = millis();
                closeDoor(1);
            }           
        }
        if((pWeight.get_units(5)) > 23 && (pWeight.get_units(5)) <= 29){
           if ((millis() - doorStartTime) >= doorInterval){
                doorStartTime = millis();
                closeDoor(2);
            }           
        }
        if((pWeight.get_units(5)) > 29 && (pWeight.get_units(5)) <= 36){
           if ((millis() - doorStartTime) >= doorInterval){
                doorStartTime = millis();
                closeDoor(3);
            }           
        }
        }
}

void closeDoor(const byte animalWeight){

    switch (animalWeight){
        case 0:
            if((scale.get_units(5)) >= 17){
                  door.write(0);
                  ClockState = ShowClock;                  
            }          
        break;
        case 1:
            if((scale.get_units(5)) >= 26){
                  door.write(0);
                  ClockState = ShowClock;                  
            }          
        break;
        case 2:
            if((scale.get_units(5)) >= 35){
                  door.write(0);
                  ClockState = ShowClock;                  
            }          
        break;
        case 3:
            if((scale.get_units(5)) >= 45){
                  door.write(0);
                  ClockState = ShowClock;                  
            }          
        break;
        default:
        
        break;
    }
     
}

void Snooze() {
  ClockState = ShowClock;
    switch (ActiveAlarms) {
    case 0:
        //No flagged alarms
        break;
    case alarm1:
        //alarm 1
        Clock.snoozeAlarm(1,SnoozePeriod);
        break;
    case alarm2:
        //alarm 2
        Clock.snoozeAlarm(2,SnoozePeriod);
        break;
    case 3:
        //both alarms
        Clock.snoozeAlarm(1,SnoozePeriod);
        Clock.snoozeAlarm(2,SnoozePeriod);
        break;
    default:
        //do nothing
        break;
    }
    toggleLED(false);  // Confirm LED turned off
    lcd.display();    // Just in case it was off

}

void clearAlarms() {
    //Clear alarm flags
    Clock.clearAlarms();
    //toggleBuzzer();
    toggleLED(false);
    lcd.display();                     // Just in case it was off
}

void editClock(byte i = 0) {
    //First Row  hh:mm AM ###.#°F
    //Second Row dow mm/dd/yyyyA^
    //                             hh    mm    AM     F     mm    dd   yyyy
    byte cursorPositions[][2] = { {1,0},{4,0},{7,0},{15,0},{5,1},{8,1},{13,1} };

    lcd.setCursor(cursorPositions[i][0], cursorPositions[i][1]);
    lcd.cursor();
    lcd.blink();
}

void editAlarm(byte i = 0) {
    /* Alarm 1      ON
     hh:mm AM Weekday            hh    mm    AM   Weekday */
     //Note valid i values are 0-3
     //lcd.setCursor(Column, Row);
    byte cursorPositions[][2] = { {1,1},{4,1},{7,1},{9,1} };

    lcd.setCursor(cursorPositions[i][0], cursorPositions[i][1]);
    lcd.cursor();
    lcd.blink();
}

void showAmtOfFood(bool changeFlag = false){

        float prevFeed;
        DateTime NowTime;   
        NowTime = Clock.read(); 
        
        foodBackup = MaxAmtfood;
       // feedRem = foodBackup - scale.get_units(10);   

    if (NowTime.Day != PreviousTime.Day) { changeFlag = true; }
    if (NowTime.Month != PreviousTime.Month) { changeFlag = true; }
    if (NowTime.Year != PreviousTime.Year) { changeFlag = true; }

            if (changeFlag == true){
        lcd.clear(); 
        lcd.setCursor(0, 0);                       // Column, Row
        lcd.print(dow2Str(NowTime.Dow));          // Integer Day of the week
                                                  // convert to String with
                                                  // an optional leading zero
        lcd.print(" ");
        lcd.print(p2Digits(NowTime.Day));
        lcd.print("/");
        lcd.print(p2Digits(NowTime.Month));
        lcd.print("/");
        int i = 2000 + NowTime.Year;
        lcd.print(i);

          if(feedRem < 0){ feedRem = 0;}
          if(feedRem >= MaxAmtfood){ feedRem = MaxAmtfood;}
         if((scale.get_units(5)) > 7){
            feedRem = foodBackup - scale.get_units(5);
            lcd.setCursor(0, 1); 
            lcd.print("Feed Amt ");  
            lcd.print(Amt2Digits(feedRem)); 
            lcd.print("g"); 
            
           
         }
         foodBackup = feedRem; 
          prevFeed = feedRem;
        if((scale.get_units(5)) < 7){
            lcd.setCursor(0, 1); 
            lcd.print("Feed Amt ");  
            lcd.print(Amt2Digits(prevFeed)); 
            lcd.print("g");
        }
         

       // 
            }
}

void editFeedAmt(byte i = 0) {

    byte cursorPositions[][2] = { {3,1} };

    lcd.setCursor(cursorPositions[i][0], cursorPositions[i][1]);
    lcd.cursor();
    lcd.blink();
}

void toggleShowAmtOfFood(){
       ClockState = ShowFeedAmt;

        AlarmRunTime = millis();
        showAmtOfFood(true);
}

void ButtonClick(Button& b) {

    if (bHoldButtonFlag == true) {
        bHoldButtonFlag = false;
    }
    else {
        switch (ClockState) {
        case PowerLoss:
            ClockState = ShowClock;
            Clock.clearOSFStatus();
        break;
        case ShowClock:
                switch (b.pinValue()) {
                    case Snooze_Pin: 
                        toggleShowAmtOfFood();               
                    break;               
                    case Lt_Pin:
                        toggleShowAlarm(alarm1);                
                    break;               
                    case Rt_Pin:
                        toggleShowAlarm(alarm2);              
                    break;               
                    default:
                             //do nothing
                    break;
            }
        break;
        case Alarm:
            //Alarm Mode
            switch (b.pinValue()) {
            case Snooze_Pin:
                //Snooze alarm for 9 minutes
                Snooze();
                ClockState = ShowClock;
                break;
            case Lt_Pin:
            case Rt_Pin:
                //turn off alarms
                clearAlarms();
                ClockState = ShowClock;
                break;
            default:
                //do nothing
                break;
            }
        break;
        case EditClock:
            switch (b.pinValue()) {
            case Snooze_Pin:
                //Increments cursor position
                cpIndex += 1;
                cpIndex %= 4;
            break;
            case Lt_Pin:
                // Decrements value
                // First Row  hh:mm AM ###.#°F
                //             0  1  2       3
                switch (cpIndex) {
                case 0:
                    changeHour(clock0, false);
                break;
                case 1:
                    changeMinute(clock0, false);
                break;
                case 2:
                    changeClockMode(clock0, false);
                break;
                case 3:
                    changeTemp();
                break;
                default:
                    //do nothing
                break;
                }
            break;
            case Rt_Pin:
                switch (cpIndex) {
                case 0:
                    changeHour(clock0, true);
                break;
                case 1:
                    changeMinute(clock0, true);
                break;
                case 2:
                    changeClockMode(clock0, true);
                break;
                case 3:
                    changeTemp();
                break;
                default:
                    //do nothing
                break;
                }
            break;
            default:
                //do nothing
            break;
            }

        break;
        case EditAlarm1:
            //Edit Alarm1 Mode
            switch (b.pinValue()) {
            case Snooze_Pin:
                //Increments cursor position
                cpIndex += 1;
                cpIndex %= 4;
                //cpIndex += 1 % 4; didn't work
                break;
            case Lt_Pin:
                // Decrements value      hh:mm AM Weekday
                //cpIndex is global
                switch (cpIndex) {
                case 0:
                    //edit Hours
                    changeHour(alarm1, false);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(alarm1, false);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(alarm1, false);
                    break;
                case 3:
                    //AlarmMode
                    changeAlarmMode(alarm1, false);
                    break;
                default:
                    //do nothing
                    break;
                }
                break;
            case Rt_Pin:
                // Increments value
                //cpIndex is global
                switch (cpIndex) {
                case 0:
                    //edit Hours
                    changeHour(alarm1, true);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(alarm1, true);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(alarm1, true);
                    break;
                case 3:
                    //AlarmMode
                    changeAlarmMode(alarm1, true);
                    break;
                default:
                    //do nothing
                    break;
                }
                break;
            default:
                //do nothing
                break;
            }
        break;
        case EditAlarm2:
            //Edit Alarm2 Mode
            switch (b.pinValue()) {
            case Snooze_Pin:
                //Increments cursor position
                cpIndex += 1;
                cpIndex %= 4;
                break;
            case Lt_Pin:
                // Decrements value      hh:mm AM Weekday
                //cpIndex is global
                switch (cpIndex) {
                case 0:
                    //edit Hours
                    changeHour(alarm2, false);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(alarm2, false);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(alarm2, false);
                    break;
                case 3:
                    //AlarmMode
                    changeAlarmMode(alarm2, false);
                    break;
                default:
                    //do nothing
                    break;
                }
                break;
            case Rt_Pin:
                // Increments value
                //cpIndex is global
                switch (cpIndex) {
                case 0:
                    //edit Hours
                    changeHour(alarm2, true);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(alarm2, true);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(alarm2, true);
                    break;
                case 3:
                    //AlarmMode
                    changeAlarmMode(alarm2, true);
                    break;
                default:
                    //do nothing
                    break;
                }
                break;
            default:
                //do nothing
                break;
            }
        break;
        case EditFeedAmt:
              switch (b.pinValue()){
                case Snooze_Pin:
                break;
                case Lt_Pin:    
                      changeFeedAmt(false);
                 break;
                 case Rt_Pin:
                      changeFeedAmt(true);
                 break;                
                    }
        break;
        default://clockstates           
        break;
        }
    }
}

void ButtonHold(Button& b) {
    
    if ((millis() - buttonHoldPrevTime) > 2000) {
        switch (ClockState) {
        case PowerLoss:
            ClockState = EditClock;
            cpIndex = 0;
            buttonHoldPrevTime = millis();
            bHoldButtonFlag = true;
            Clock.clearOSFStatus();
            break;
        case ShowClock:
            switch (b.pinValue()) {
            case Snooze_Pin:
                ClockState = EditClock;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                break;
            case Lt_Pin:
                //Edit Alarm1
                ClockState = EditAlarm1;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayAlarm(1, true);
                break;
            case Rt_Pin:
                //Edit Alarm2
                ClockState = EditAlarm2;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayAlarm(2, true);
                break;
            default:
                break;
            }
            break;
        case ShowAlarm1:
            switch (b.pinValue()) {
            case Snooze_Pin:
                break;
            case Lt_Pin:
                ClockState = EditAlarm1;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayAlarm(1, true);
                //Switch to edit mode
                break;
            case Rt_Pin:
                //Do Nothing
                break;
            default:
                break;
            }
            break;
        case ShowAlarm2:
            switch (b.pinValue()) {
            case Snooze_Pin:
                break;
            case Lt_Pin:
                break;
            case Rt_Pin:
                //Edit Alarm2
                ClockState = EditAlarm2;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayAlarm(2, true);
                break;
            default:
                break;
            }
            break;
        case Alarm:
            //Alarm Mode
            switch (b.pinValue()) {
            case Snooze_Pin:
                Snooze();             //Snooze alarm for 9 minutes
                ClockState = ShowClock;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayClock(true);
                break;
            case Lt_Pin:
            case Rt_Pin:
                //turn off alarms
                clearAlarms();
                ClockState = ShowClock;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayClock(true);
                break;
            default:
                //do nothing
                break;
            }
            break;
        case EditClock:  //Edit Clock
                  switch (b.pinValue()) {
                      case Snooze_Pin:
                          ClockState = EditFeedAmt;
                          cpIndex = 0;
                          buttonHoldPrevTime = millis();
                          bHoldButtonFlag = true;
                          foodParam(true);
                      break;
                      case Lt_Pin:                      
                      case Rt_Pin:                 
                      default:
                      break;
            }
              
        break;
        case EditFeedAmt: 
            switch (b.pinValue()) {
            case Snooze_Pin:
                lcd.noBlink();
                lcd.noCursor();
                ClockState = ShowClock;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayClock(true);
                break;
            case Lt_Pin:
            case Rt_Pin:
            default:
                break;
            }
            break;
        case EditAlarm1:  //Edit Alarm1
            switch (b.pinValue()) {
            case Snooze_Pin:
                lcd.noBlink();
                lcd.noCursor();
                ClockState = ShowClock;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayClock(true);
                break;
            case Lt_Pin:
            case Rt_Pin:
            default:
                break;
            }
            break;
            
        case EditAlarm2:  //Edit Alarm1
            switch (b.pinValue()) {
                case Snooze_Pin:
                    lcd.noBlink();
                    lcd.noCursor();
                    ClockState = ShowClock;
                    buttonHoldPrevTime = millis();
                    bHoldButtonFlag = true;
                    displayClock(true);
                break;
                case Lt_Pin:
                case Rt_Pin:
                default:
                break;
                }
            break;
        default:
            //todo
        break;
        }
    }

}

String dow2Str(byte bDow) {
    // Day of week to string or char array. DOW 1=Sunday, 0 is undefined
    static const char* str[] = { "---", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    if (bDow > 7) bDow = 0;
    return(str[bDow]);
}

String p2Digits(int numValue) {
    String str;

    if (numValue < 10) {
        str = "0" + String(numValue);
    }
    else {
        str = String(numValue);
    }
    return str;
}

String Amt2Digits(int numValue) {
    String str;

    if (numValue < 1000 && numValue >= 100) {
        str = "0" + String(numValue);
    }
    else if (numValue < 100 && numValue >= 10) {
        str = "00" + String(numValue);
    }
    else if (numValue < 10) {
        str = "000" + String(numValue);
    }
    else {
        str = String(numValue);
    }
    return str;
}

float getTemperatureValue() {
   Fahrenheit = false;
    float floatTemperature;
    floatTemperature = Clock.getTemperatureFloat();
    if (Fahrenheit == true) {
        floatTemperature = floatTemperature * 9.0 / 5.0 + 32.0;
    }
    return floatTemperature;
}

byte CheckAlarmStatus() {
    bool AlarmStatus = digitalRead(SQW_Pin);
    byte flaggedAlarms = Clock.flaggedAlarms();

    if (AlarmStatus == LOW) {
        //Alarm detected
        ClockState = Alarm;
    }
    return flaggedAlarms;
}

void lcdAlarmIndicator() {
    byte alarmEnabledStatus;

    alarmEnabledStatus = Clock.alarmStatus();
    /* Returns:
       0 - No alarms
       1 - Alarm 1 enabled
       2 - Alarm 2 enabled
       3 - Both alarms enabled
     */
    switch (alarmEnabledStatus) {
    case 0:
        //No alarms
        lcd.print("-");
        break;
    case 1:
        //alarm 1 enabled
        lcd.write(1); //cA1
        break;
    case 2:
        //alarm 2 enabled
        lcd.write(2); //cA2
        break;
    case 3:
        //both alarms enabled
        lcd.write(3); //cBA
        break;
    default:
        break;
    }
}

void welcome() {
    byte zero[8] =
    {
    0b00000,
    0b11000,
    0b11000,
    0b10100,
    0b10011,
    0b10000,
    0b10010,
    0b10000
    };
    byte one[8] =
    {
    0b00000,
    0b00110,
    0b00110,
    0b01010,
    0b10010,
    0b00010,
    0b10010,
    0b00010
    };
    byte two[8] =
    {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00001,
    0b00010,
    0b00011
    };
    byte three[8] =
    {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11000,
    0b00100,
    0b10010
    };
    byte four[8] =
    {
    0b10011,
    0b01000,
    0b00111,
    0b00010,
    0b00111,
    0b01000,
    0b01000,
    0b00111
    };
    byte five[8] =
    {
    0b10010,
    0b00100,
    0b11011,
    0b00000,
    0b00000,
    0b00001,
    0b00010,
    0b11111
    };
    byte six[8] =
    {
    0b00000,
    0b00000,
    0b11100,
    0b00011,
    0b10001,
    0b00001,
    0b00001,
    0b11110
    };
    byte seven[8] =
    {
    0b00101,
    0b00101,
    0b00101,
    0b11001,
    0b00010,
    0b11100,
    0b00000,
    0b00000
    };


    lcd.createChar(0, zero);
    lcd.createChar(1, one);
    lcd.createChar(2, two);
    lcd.createChar(3, three);
    lcd.createChar(4, four);
    lcd.createChar(5, five);
    lcd.createChar(6, six);
    lcd.createChar(7, seven);

    lcd.setCursor(0, 0);
    lcd.print("WELCOME!!  ");

    lcd.setCursor(10, 0);
    lcd.write(byte(0));
    lcd.setCursor(11, 0);
    lcd.write(byte(1));
    lcd.setCursor(12, 0);
    lcd.write(byte(2));
    lcd.setCursor(13, 0);
    lcd.write(byte(3));
    lcd.setCursor(10, 1);
    lcd.write(byte(4));
    lcd.setCursor(11, 1);
    lcd.write(byte(5));
    lcd.setCursor(12, 1);
    lcd.write(byte(6));
    lcd.setCursor(13, 1);
    lcd.write(byte(7));
}

void checkAnimal(){
  
   if((pWeight.get_units(5)) > 9){animalPresent = true;}

    if(animalPresent == true){
          Snooze();
          ClockState = DoorOpening;
        }

}

/* ***********************************************************
 *                         Void Setup                        *
 * ********************************************************* */
void setup() {
       // Get the start time
    RunTime = millis(); 

    Serial.begin(9600);  //for debugging

        // Pin Modes 
    pinMode(LED_Pin, OUTPUT);
    digitalWrite(LED_Pin, LOW);
    pinMode(greenLed, OUTPUT);
    digitalWrite(greenLed, LOW);
    pinMode(BUZZER_Pin, OUTPUT);
    digitalWrite(BUZZER_Pin, LOW);

       //LCD Stuff and welcome screen 
    lcd.init();
    lcd.clear();
    lcd.backlight();    
    welcome();
    delay(4000);
    lcd.clear();
    
       //Create custom lcd characters
    lcd.createChar(1, cA1);
    lcd.createChar(2, cA2);
    lcd.createChar(3, cBA);
    

      //Clock Stuff  
    Clock.begin();
    if (Clock.getOSFStatus() == true) { ClockState = PowerLoss;}

      //check temperature
   CurrentTemperature = getTemperatureValue();

      //weight sensors
   pWeight.begin(doutPet, clkPet);
   pWeight.set_scale(calibFacPet); 
   pWeight.tare(); 
   scale.begin(doutPlate, clkPlate);
   scale.set_scale(calibFacPlate); 
   scale.tare();

      //feeder door
   door.attach(doorPin);
   door.write(minAngle);
     
       //Button callback functions 
    LtKey.clickHandler(ButtonClick);
    LtKey.holdHandler(ButtonHold, Button_Hold_Time);
    RtKey.clickHandler(ButtonClick);
    RtKey.holdHandler(ButtonHold, Button_Hold_Time);
    SnoozeKey.clickHandler(ButtonClick);
    SnoozeKey.holdHandler(ButtonHold, Button_Hold_Time);

        //Display the clock
    displayClock(true);

}

/* ***********************************************************
 *                         Void Loop                         *
 * ********************************************************* */
void loop() {
    static long previousMillis = 0;

//    Serial.print(pWeight.get_units(5),1);
//    Serial.print(" grams"); 
//    Serial.print("                  ");
//    Serial.print(scale.get_units(5),1);
//    Serial.println(" grams");
    

    switch (ClockState) {
    case PowerLoss:
        displayClock();
        
        //Flash Clock
        if ((millis() - previousMillis) >= flashInterval) {
            previousMillis = millis();
            if (bDisplayStatus == true) {
                lcd.noDisplay();
            }
            else {
                lcd.display();
            }
            bDisplayStatus = !bDisplayStatus;
        }
        break;
    case ShowClock:
        displayClock();
        break;
    case ShowAlarm1:
        //AlarmRunTime is defined by toggleShowAlarm
        if ((millis() - AlarmRunTime) <= Alarm_View_Pause) {
            displayAlarm(alarm1);
        }
        else {
            ClockState = ShowClock;
            displayClock(true);
        }
        break;
    case ShowAlarm2:
        //AlarmRunTime is defined by toggleShowAlarm
        if ((millis() - AlarmRunTime) <= Alarm_View_Pause) {
            displayAlarm(alarm2);
        }
        else {
            ClockState = ShowClock;
            displayClock(true);
        }
        break;
    case ShowFeedAmt:
         if ((millis() - AlarmRunTime) <= FeedAmtPauseTime) {
            showAmtOfFood();
        }
        else {
            ClockState = ShowClock;
            displayClock(true);
        }
    break;

    case Alarm:
            displayClock();
        
        //Flash Clock
        if ((millis() - previousMillis) >= flashInterval) {
            previousMillis = millis();
            if (bDisplayStatus == true) {
                lcd.noDisplay();
            }
            else {
                lcd.display();
            }
            bDisplayStatus = !bDisplayStatus;
            digitalWrite(greenLed, LOW);
            toggleLED();
            toggleBuzzer();
            checkAnimal();
        }        
        break;
    case EditClock:
        editClock(cpIndex);
        displayClock();
        break;
    case EditAlarm1:
        editAlarm(cpIndex);
        displayAlarm(alarm1);
        break;
    case EditAlarm2:
        editAlarm(cpIndex);
        displayAlarm(alarm2);
        break;
    case EditFeedAmt:
         editFeedAmt(cpIndex);
         foodParam();
        break;
    case DoorOpening:
        openDoor();
    break;
    default:
        displayClock();
        break;
   }
    LtKey.process();
    RtKey.process();
    SnoozeKey.process();
    ActiveAlarms = CheckAlarmStatus();  //Returns which alarms are activated
}
