                                     /*************
                                      * LIBRARIES
                                     **************/
#include <LiquidCrystal_I2C.h>
#include <SimpleAlarmClock.h>
#include <Button.h> 


                                      /************
                                       * CONSTANTS
                                      *************/
 const int redLedPin = 13; 
 const int greenLedPin = 7;
 const int buzzerPin = 12;

 const byte RTC_addr = 0x68;      // I2C address of DS3231 RTC
 const byte EEPROM_addr = 0x57;  // I2C address of AT24C32N EEPROM
 const bool INTCN = true;       // allows SQW pin to be monitored

 LiquidCrystal_I2C lcd(0x27, 16, 2);                     //lcd object created
 SimpleAlarmClock Clock(RTC_addr, EEPROM_addr, INTCN);  //clock object

 const int CtrlPin = 11;
 const int Lt_Pin = 9;
 const int Rt_Pin = 10;
 const int DebouceTime = 30;  // button debouce time in ms
 Button CtrlKey(CtrlPin, BUTTON_PULLUP_INTERNAL, true, DebouceTime);  //button pin, button mode, debounce mode(bool), debounce time
 Button LtKey(Lt_Pin, BUTTON_PULLUP_INTERNAL, true, DebouceTime);
 Button RtKey(Rt_Pin, BUTTON_PULLUP_INTERNAL, true, DebouceTime);

const byte clock0 = 0;
const byte FeedTime1 = 1;  //all clock Modes
const byte FeedTime2 = 2;

const byte AMhr = 0;
const byte PMhr = 1;  //all clock modes
const byte M24hr = 2;

const byte Daily = 0;
const byte Weekday = 1;  //all alarm modes
const byte Weekend = 2;
const byte Once = 3;

                                      /*******************
                                      * GLOBAL VARIABLES
                                      ********************/
 unsigned long RunTime;  // Used to track time between get temperature value
 unsigned long AlarmRunTime;
 float CurrentTemperature;
 bool Fahrenheit = true;  // true for Farenheit and false for celcius
 bool bHoldButtonFlag = false;  // used to prevent holdButton also activating clickButton

 byte cpIndex = 0;  // Cursor Position Index 
 
 enum States {
    PowerLoss,
    ShowClock,
    ShowFeedTime1,
    ShowFeedTime2,  //all existing feeder modes
    Feeding,
    EditClock,
    EditFeedTime1,
    EditFeedTime2
};
States FeederState = ShowClock;

byte upArrow[8] = {
              0b00100,
              0b01110,
              0b11111,
              0b00000,  //Up arrow
              0b00000,
              0b00000,
              0b00000,
              0b00000 };
byte downArrow[8] = {
              0b00000,
              0b00000,
              0b00000,
              0b00000,  //Down arrow
              0b00000,
              0b11111,
              0b01110,
              0b00100 };
byte bothArrows[8] = {
              0b00100,
              0b01110,
              0b11111,
              0b00000,  //Both arrows
              0b00000,
              0b11111,
              0b01110,
              0b00100 };

byte FeedLetter[8] = {
              0b00000,
              0b00000,
              0b00111,
              0b00100,  //Skinny letter F
              0b00111,
              0b00100,
              0b00100,
              0b00100 };


void setup() {       
    RunTime = millis();   // Get the start time
           
           // Pin Modes      
    pinMode(redLedPin, OUTPUT);     
    digitalWrite(redLedPin, LOW);
    pinMode(greenLedPin, OUTPUT);
    digitalWrite(greenLedPin, HIGH);
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);
         
          //LCD Stuff       
    lcd.init();  
    lcd.clear();
    lcd.backlight();
    
         //Create custom lcd characters
    lcd.createChar(1, upArrow);   
    lcd.createChar(2, downArrow);
    lcd.createChar(3, bothArrows);
    lcd.createChar(4, FeedLetter);

         //Clock Stuff        
    Clock.begin();
    if (Clock.getOSFStatus() == true) {
        FeederState = PowerLoss;
    }
    CurrentTemperature = getTemperatureValue();

         //Button callback functions 
    LtKey.clickHandler(ButtonClick);
    //LtKey.holdHandler(ButtonHold, Button_Hold_Time);
    RtKey.clickHandler(ButtonClick);
    //RtKey.holdHandler(ButtonHold, Button_Hold_Time);
    CtrlKey.clickHandler(ButtonClick);
    //CtrlKey.holdHandler(ButtonHold, Button_Hold_Time);

    //Display the clock
    //displayClock(true);

}


void loop() {
  

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


void ButtonClick(Button& b) {
    if (bHoldButtonFlag == true) {       
        bHoldButtonFlag = false;  // After a hold button is released, a button click is also registered
    }
    else {
        //PowerLoss,ShowClock, Feeding, EditClock, EditFeedTime1, EditFeedTime2
        switch (FeederState) {
        case PowerLoss:
            //any clickbutton and return to ShowClock
            FeederState = ShowClock;
            Clock.clearOSFStatus();
            break;
        case ShowClock:
            switch (b.pinValue()) {
            case CtrlPin:
                //Do Nothing
                break;
            case Lt_Pin:
                showFeedTime(FeedTime1);
                break;
            case Rt_Pin:
                showFeedTime(FeedTime2);
                break;
            default:
                //do nothing
                break;
            }
            break;
            //ShowAlarm1 or ShowAlarm2 does nothing
        case Feeding:
            switch (b.pinValue()) {
            case CtrlPin:
                FeederState = ShowClock;
                break;
            case Lt_Pin:
            case Rt_Pin:
                //turn off alarms
                clearAlarms();
                FeederState = ShowClock;
                break;
            default:
                //do nothing
                break;
            }
            break;
        case EditClock:
            //Edit Clock Mode
            switch (b.pinValue()) {
            case CtrlPin:
                //Increments cursor position
                //cpIndex += 1 % 5;
                cpIndex += 1;
                cpIndex %= 5;
                break;
            case Lt_Pin:
                // Decrements value
                // First Row  hh:mm AM ###.#°F
                //             0  1  2       3
                // Second Row          ##.#Kg
                //                        4
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
                case 4:
                    changeAmtOfFood(false);
                    break; 
                default:
                    //do nothing
                    break;
                }
                break;
            case Rt_Pin:
                // Increments value
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
                case 4:
                    changeAmtOfFood(true);
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
            //End EditClock
            break;
        case EditFeedTime1:
            switch (b.pinValue()) {
            case CtrlPin:
                //Increments cursor position
                cpIndex += 1;
                cpIndex %= 4;
                break;
            case Lt_Pin:
                // Decrements value      hh:mm AM Weekday
                
                switch (cpIndex) {
                case 0:
                    changeHour(FeedTime1, false);
                    break;
                case 1:
                    changeMinute(FeedTime1, false);
                    break;
                case 2:
                    changeClockMode(FeedTime1, false);
                    break;
                case 3:
                    changeAlarmMode(FeedTime1, false);
                    break;
                default:
                    //do nothing
                    break;
                }
                break;
            case Rt_Pin:
                // Increments value
                switch (cpIndex) {
                case 0:
                    changeHour(FeedTime1, true);
                    break;
                case 1:
                    changeMinute(FeedTime1, true);
                    break;
                case 2:
                    changeClockMode(FeedTime1, true);
                    break;
                case 3:
                    changeAlarmMode(FeedTime1, true);
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
        case EditFeedTime2:
            switch (b.pinValue()) {
            case CtrlPin:
                //Increments cursor position
                cpIndex += 1;
                cpIndex %= 4;
                break;
            case Lt_Pin:
                // Decrements value      hh:mm AM Weekday
                
                switch (cpIndex) {
                case 0:
                    changeHour(FeedTime2, false);
                    break;
                case 1:
                    changeMinute(FeedTime2, false);
                    break;
                case 2:
                    changeClockMode(FeedTime2, false);
                    break;
                case 3:
                    changeAlarmMode(FeedTime2, false);
                    break;
                default:
                    //do nothing
                    break;
                }
                break;
            case Rt_Pin:
                // Increments value

                switch (cpIndex) {
                case 0:
                    changeHour(FeedTime2, true);
                    break;
                case 1:
                    changeMinute(FeedTime2, true);
                    break;
                case 2:
                    changeClockMode(FeedTime2, true);
                    break;
                case 3:
                    changeAlarmMode(FeedTime2, true);
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
        default:
            //todo
            break;
        }
    }
}


void showFeedTime(byte i) {
        if (i == 2) {
            FeederState = ShowFeedTime2;
        }
        else {
            FeederState = ShowFeedTime1;
        }
        AlarmTime feed;
        feed = Clock.readAlarm(i);
        feed.Enabled = !feed.Enabled;
        Clock.setAlarm(feed, i);
        AlarmRunTime = millis();
        displayFeedTime(i, true);
    
    //otherwise do nothing
}


void clearAlarms() {  
    Clock.clearAlarms();  //Clear alarm flags
    toggleLED(false);
    lcd.display();  // Just in case it was off
}


void changeHour(byte i, bool increment) {
    AlarmTime feed;
    DateTime NowTime;  //create DateTime struct from Library
    int Hour;
    byte ClockMode;

   switch (i) {
    case clock0:
        NowTime = Clock.read();  // get the latest clock values
        Hour = NowTime.Hour;
        ClockMode = NowTime.ClockMode;
        break;
    case FeedTime1:
        feed = Clock.readAlarm(FeedTime1);
        Hour = feed.Hour;
        ClockMode = feed.ClockMode;
        break;
    case FeedTime2:
        feed = Clock.readAlarm(FeedTime2);
        Hour = feed.Hour;
        ClockMode = feed.ClockMode;
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
        break;
    default:
        //do nothing
        break;
    }
    switch (i) {
    case clock0:
        NowTime.Hour = byte(Hour);
        Clock.write(NowTime);
        break;
    case FeedTime1:
        feed.Hour = byte(Hour);
        Clock.setAlarm(feed, 1);
        break;
    case FeedTime2:
        feed.Hour = byte(Hour);
        Clock.setAlarm(feed, 2);
        break;
    default:
        NowTime.Hour = byte(Hour);
        Clock.write(NowTime);
        break;
    }

}


void changeMinute(byte i, bool increment) {
    AlarmTime feed;
    DateTime NowTime;            //create DateTime struct from Library
    int Minute;

   switch (i) {
    case clock0:
        NowTime = Clock.read();        // get the latest clock values
        Minute = NowTime.Minute;
        break;
    case FeedTime1:
        feed = Clock.readAlarm(FeedTime1);
        Minute = feed.Minute;
        break;
    case FeedTime2:
        feed = Clock.readAlarm(FeedTime2);
        Minute = feed.Minute;
        break;
    default:
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

    if (Minute < 0) { Minute = 59; }
    switch (i) {
    case clock0:
        NowTime.Minute = byte(Minute);
        Clock.write(NowTime);
        break;
    case FeedTime1:
        feed.Minute = byte(Minute);
        Clock.setAlarm(feed, 1);
        break;
    case FeedTime2:
        feed.Minute = byte(Minute);
        Clock.setAlarm(feed, 2);
        break;
    default:
        NowTime.Minute = byte(Minute);
        Clock.write(NowTime);
        break;
    }

}


void changeClockMode(byte i, bool increment) {
    AlarmTime feed;
    DateTime NowTime = Clock.read();     //create DateTime struct from Library
    int ClockMode = NowTime.ClockMode;   //int is able to be negative

    switch (i) {
    case clock0:
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
        fixAlarmClockMode(FeedTime1, NowTime.ClockMode);
        fixAlarmClockMode(FeedTime2, NowTime.ClockMode);
        break;
    case FeedTime1:
    case FeedTime2:
        if (ClockMode != M24hr) {
            feed = Clock.readAlarm(i);
            if (feed.ClockMode == AMhr) {
                feed.ClockMode = PMhr;
            }
            else {
                feed.ClockMode = AMhr;
            }
            Clock.setAlarm(feed, i);
        } //else do nothing
        break;
    default:
        //do nothing
        break;
    }
}


void changeTemp() {
    Fahrenheit = !Fahrenheit;  //change the temperature to F or C
    CurrentTemperature = getTemperatureValue();
    RunTime = millis();
    displayClock(true);
}


void changeAmtOfFood(bool increment) {
    float weight = 0.5;
      if(increment == true){
          weight += 0.5;
        }
      else {weight -= 0.5}
}


void changeAlarmMode(byte i, bool increment) {
    // Change AlarmMode to 0=Daily, 1=Weekday, 2=Weekend, 3=Once

        AlarmTime feed = Clock.readAlarm(i);
        int AlarmMode = feed.AlarmMode;

        if (increment == true) {
            AlarmMode += 1;
            AlarmMode %= 4;
        }
        else {
            AlarmMode -= 1;
            AlarmMode %= 4;
        }

        if (AlarmMode < 0) { AlarmMode = 3; }
        feed.AlarmMode = byte(AlarmMode);
        Clock.setAlarm(feed, i);
}


void displayFeedTime(byte index, bool changeFlag) {
    AlarmTime feed;            

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
