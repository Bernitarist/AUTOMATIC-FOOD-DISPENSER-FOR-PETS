                                     /*************
                                      * LIBRARIES
                                     **************/
#include <LiquidCrystal_I2C.h>
#include <SimpleAlarmClock.h>
#include <Button.h> 
#include "pitches.h"
//#include "NewPing.h"


                                      /************
                                       * CONSTANTS
                                      *************/
// #define trigPin 6
// #define echoPin 6
// #define MAXDistance 200                                      
 const int redLedPin = 13; 
 const int greenLedPin = 8;
 const int buzzerPin = 7;
 const int SQW_Pin = 2;
// const int ServoPin = 12;

 const byte RTC_addr = 0x68;      // I2C address of DS3231 RTC
 const byte EEPROM_addr = 0x57;  // I2C address of AT24C32N EEPROM
 const bool INTCN = true;       // allows SQW pin to be monitored

 LiquidCrystal_I2C lcd(0x27, 16, 2);                     //lcd object created
 SimpleAlarmClock Clock(RTC_addr, EEPROM_addr, INTCN);  //clock object
// NewPing sonar(trigPin, echoPin, MAXDistance);         //ultra sonic sensor object

 const int CtrlPin = 11;
 const int Lt_Pin = 9;
 const int Rt_Pin = 10;
 const int DebouceTime = 30;  // button debouce time in ms
 
 Button CtrlKey(CtrlPin, BUTTON_PULLUP_INTERNAL, true, DebouceTime);  //button pin, button mode, debounce mode(bool), debounce time
 Button LtKey(Lt_Pin, BUTTON_PULLUP_INTERNAL, true, DebouceTime);
 Button RtKey(Rt_Pin, BUTTON_PULLUP_INTERNAL, true, DebouceTime);
 
 unsigned long buttonHoldPrevTime = 0.0;  // Used to track button hold times 
 const int Button_Hold_Time = 3000;      // button hold length of time in ms
 const int flashInterval = 1000;         // Alarm flashing interval
 const int Alarm_View_Pause = 2000;      // View FeedTime Length of time in ms

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
 bool PrevFahrenheit = Fahrenheit;  // Capture previous Fahrenheit
 bool bHoldButtonFlag = false;  // used to prevent holdButton also activating clickButton
 
 AlarmTime PreviousFeedTime;
 DateTime PreviousTime;  
 bool bDisplayStatus = true;  // used to track the lcd display ON status
 byte cpIndex = 0;  // Cursor Position Index 
 byte ActiveFeedTimes = 0;  // used to store active alarms (not enabled alarms)

 unsigned int maxDistance = 20;  //max dist measured by sensor

 int melody[] = { 
    NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6,  //notes to determine pitch of buzzer sound
};
int noteDurations[] = { 16, 16, 16, 16, 16 };  // note durations: 4 = quarter note, 8 = eighth note, etc.:

int angle = 0;
 
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
              0b00111,
              0b00100,
              0b00100,  //Skinny letter F
              0b00111,
              0b00100,
              0b00100,
              0b00100 };


                                      /*********************
                                      * FUNCTION PROTOTYPES
                                      **********************/
//bool checkDistance();                                     
void displayClock(bool changeFlag = false);
float getTemperatureValue();
void ButtonClick(Button& b);
void showFeedTime(byte i);
void clearAlarms();
void changeHour(byte i, bool increment);
void changeMinute(byte i, bool increment);
void changeClockMode(byte i, bool increment);
void changeTemp();
//void changeAmtOfFood(bool increment);
void changeAlarmMode(byte i, bool increment);
void ButtonHold(Button& b);
void displayFeedTime(byte index = 1, bool changeFlag = false);
String p2Digits(int numValue);
void lcdAlarmIndicator();
void toggleLEDred(bool ledON = true);
void toggleBuzzer();
void editClock(byte i);
void editFeedTime(byte i);
byte CheckFeedTimeStatus();
void fixFeedTimeClockMode(byte FeedTimeIndex, byte NewClockMode);
//void displayNextFeed();
//void showAmtOfFood();
//void feederDoor();
void welcome();

//bool distConfirmed = checkDistance();  //check the sensor dist and the floor

void setup() {       
    RunTime = millis();   // Get the start time
    Serial.begin(9600);
    
           
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
    welcome();
    delay(4000);
    lcd.clear();
    
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
    LtKey.holdHandler(ButtonHold, Button_Hold_Time);
    RtKey.clickHandler(ButtonClick);
    RtKey.holdHandler(ButtonHold, Button_Hold_Time);
    CtrlKey.clickHandler(ButtonClick);
    CtrlKey.holdHandler(ButtonHold, Button_Hold_Time);

    displayClock(true);

}


void loop() {
  static long previousMillis = 0;

    switch (FeederState) {
    case PowerLoss:
        displayClock();
        digitalWrite(greenLedPin, LOW);
        
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
        digitalWrite(greenLedPin, HIGH);
        break;

        //for both, AlarmRunTime is defined by showFeedTime
    case ShowFeedTime1:
        if ((millis() - AlarmRunTime) <= Alarm_View_Pause) {
            displayFeedTime(FeedTime1);
        }
        else {
            FeederState = ShowClock;
            digitalWrite(greenLedPin, HIGH);
            displayClock(true);
        }
        break;
    case ShowFeedTime2:
        if ((millis() - AlarmRunTime) <= Alarm_View_Pause) {
            displayFeedTime(FeedTime2);
        }
        else {
            FeederState = ShowClock;
            digitalWrite(greenLedPin, HIGH);
            displayClock(true);
        }
        break;
    case Feeding:
        displayClock();
        digitalWrite(greenLedPin, LOW);
        
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
            toggleLEDred();
            toggleBuzzer();
            //feederDoor();
        }
        break;
        
    case EditClock:
        digitalWrite(greenLedPin, LOW);
        editClock(cpIndex);
        displayClock();       
        break;
        
    case EditFeedTime1:
        digitalWrite(greenLedPin, LOW);
        editFeedTime(cpIndex);
        displayFeedTime(FeedTime1);
        break;
    case EditFeedTime2:
        digitalWrite(greenLedPin, LOW);
        editFeedTime(cpIndex);
        displayFeedTime(FeedTime2);
        break;
    default:
        displayClock();
        break;
    }
    LtKey.process();
    RtKey.process();
    CtrlKey.process();
    ActiveFeedTimes = CheckFeedTimeStatus();  //Returns which FeedTime are active

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
                Serial.println("left button clicked");
                showFeedTime(FeedTime1);
                break;
            case Rt_Pin:
                Serial.println("right button clicked");
                showFeedTime(FeedTime2);
                break;
            default:
                //do nothing
                break;
            }
            break;
            //ShowAlarm1 or ShowFeedTime2 does nothing
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
                    //changeAmtOfFood(false);
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
                    //changeAmtOfFood(true);
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
            //do nothing
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
    toggleLEDred();
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
        fixFeedTimeClockMode(FeedTime1, NowTime.ClockMode);
        fixFeedTimeClockMode(FeedTime2, NowTime.ClockMode);
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


//void changeAmtOfFood(bool increment) {
//    float weight = 0.5;
//      if(increment == true){
//          weight += 0.5;
//        }
//      else {weight -= 0.5;}
//}


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


void ButtonHold(Button& b) {
    // PowerLoss, ShowClock, ShowFeedTime1, ShowFeedTime2, Feeding, EditClock, EditFeedTime1, EditFeedTime2

    // To ignore back to back button hold? 
    if ((millis() - buttonHoldPrevTime) > 2000) {
        switch (FeederState) {
        case PowerLoss:
            FeederState = EditClock;
            cpIndex = 0;
            buttonHoldPrevTime = millis();
            bHoldButtonFlag = true;
            Clock.clearOSFStatus();
            break;
        case ShowClock:
            switch (b.pinValue()) {
            case CtrlPin:
                FeederState = EditClock;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                break;
            case Lt_Pin:
                FeederState = EditFeedTime1;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayFeedTime(1, true);
                break;
            case Rt_Pin:
                FeederState = EditFeedTime2;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayFeedTime(2, true);
                break;
            default:
                break;
            }
            break;
        case ShowFeedTime1:
            switch (b.pinValue()) {
            case CtrlPin:
                break;
            case Lt_Pin:
                FeederState = EditFeedTime1;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayFeedTime(1, true);
                //Switch to edit mode
                break;
            case Rt_Pin:
                break;
            default:
                break;
            }
            break;
        case ShowFeedTime2:
            switch (b.pinValue()) {
            case CtrlPin:
                break;
            case Lt_Pin:
                break;
            case Rt_Pin:
                FeederState = EditFeedTime2;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayFeedTime(2, true);
                break;
            default:
                break;
            }
            break;
        case Feeding:
            switch (b.pinValue()) {
            case CtrlPin:
                FeederState = ShowClock;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayClock(true);
                break;
            case Lt_Pin:
            case Rt_Pin:
                clearAlarms();
                FeederState = ShowClock;
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
            case CtrlPin:
                lcd.noBlink();
                lcd.noCursor();
                FeederState = ShowClock;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                break;
            case Lt_Pin:
            case Rt_Pin:
            default:
                break;
            }
            break;
        case EditFeedTime1: 
            switch (b.pinValue()) {
            case CtrlPin:
                lcd.noBlink();
                lcd.noCursor();
                FeederState = ShowClock;
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
        case EditFeedTime2: 
            switch (b.pinValue()) {
            case CtrlPin:
                lcd.noBlink();
                lcd.noCursor();
                FeederState = ShowClock;
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


void displayFeedTime(byte index = 1, bool changeFlag = false) {                                       
    AlarmTime feed;            //create AlarmTime struct from Library

    if (index == FeedTime2) {
        feed = Clock.readAlarm(FeedTime2);      // get the latest FeedTime2 values
    }
    else {
        feed = Clock.readAlarm(FeedTime1);      // get the latest alarm1 values
    }

    // Check for Alarm change
    if (feed.Hour != PreviousFeedTime.Hour) { changeFlag = true; }
    if (feed.Minute != PreviousFeedTime.Minute) { changeFlag = true; }
    if (feed.ClockMode != PreviousFeedTime.ClockMode) { changeFlag = true; }
    if (feed.AlarmMode != PreviousFeedTime.AlarmMode) { changeFlag = true; }

    //Update Display - Only change display if change is detected
    if (changeFlag == true) {
      
      lcd.init();  
      lcd.clear();
      lcd.backlight();

        // First row
        lcd.setCursor(0, 0);
        if (index == FeedTime2) {
            lcd.print("Alarm 2");
        }
        else {
            lcd.print("Alarm 1");
        }
        lcd.setCursor(13, 0);
        if (feed.Enabled == true) {
            lcd.print("ON");
        }
        else {
            lcd.print("OFF");
        }

        //Second row
        lcd.setCursor(0, 1);
        lcd.print(p2Digits(feed.Hour));
        lcd.print(":");
        lcd.print(p2Digits(feed.Minute));
        switch (feed.ClockMode) {
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
            lcd.print("  H");
            break;
        default:
            lcd.print("  H");
            break;
        }
        switch (feed.AlarmMode) {
            //0=Daily, 1=Weekday, 2=Weekend, 3=Once
        case 0:
            lcd.print(" Daily");
            break;
        case 1:
            lcd.print(" Weekday");
            break;
        case 2:
            lcd.print(" Weekend");
            break;
        case 3:
            lcd.print(" Once");
            break;
        default:
            break;
        }
        PreviousFeedTime = feed;
    }
}


String p2Digits(int numValue) {
    // utility function for digital clock display
    // converts int to two digit char array
    String str;

    if (numValue < 10) {
        str = "0" + String(numValue);
    }
    else {
        str = String(numValue);
    }
    return str;
}


void displayClock(bool changeFlag = false) {
    /* 
     *   changeFlag - true forces display refresh
     *              - false does nothing
     */

    DateTime NowTime;            //create DateTime struct from Library
    NowTime = Clock.read();      // get the latest clock values

        // Check the temperature every 65 seconds OR
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

    //Update Display - Only change display if change is detected
    if (changeFlag == true) {
      lcd.init();  
      lcd.clear();
      lcd.backlight();

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
            lcd.print("  M ");
            break;
        default:
            break;
        }
        if (CurrentTemperature < 100.0) { lcd.print(" "); }
        lcd.print(String(CurrentTemperature, 1));  // converts float to string
                                                  // with 1 decimal place
        lcd.print((char)223);                     // prints the degree symbol
        if (Fahrenheit) { lcd.print("F"); }
        else { lcd.print("C"); }

        //Second Row  dow mm/dd/yyyy
        lcd.setCursor(0, 1);                       // Column, Row
        lcd.print("Nxt Feed ");          // amount of food remaining in the container

        //displayNextFeed(FeedTime1);
        lcd.setCursor(14, 1);                                         
        lcd.write(4);                             //Skinny letter F
        lcdAlarmIndicator();                      //lcd.print A1, A2, BA, or -

        PreviousTime = Clock.read();
    }

//    if(distConfirmed ){  //and feed plate is empty
//        feederDoor();
//      }
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
        lcd.print("-");
        break;
    case 1:
        lcd.write(1); //up
        break;
    case 2:
        lcd.write(2); //down
        break;
    case 3:
        lcd.write(3); //both
        break;
    default:
        break;
    }
}


void toggleLEDred(bool ledON = true) {
      bool ledState;

    ledState = digitalRead(redLedPin);                //get the state of LED
    if (ledON == true) {
        digitalWrite(redLedPin, !ledState);           //do the opposite
    }
    else {
        digitalWrite(redLedPin, LOW);
    }
}


void toggleBuzzer() {
    int thisNote;    

      // if(!distConfirmed){ 
        for(thisNote = 0;thisNote < 5;thisNote++){ 
          int noteDuration = 1000 / noteDurations[thisNote];  // to calculate the note duration, take one second divided by the note type. e.g. quarter note = 1000 / 4,
          tone(buzzerPin, melody[thisNote], noteDuration);

          int pauseBetweenNotes = noteDuration * 1.30;  //to distinguish the notes, set a minimum time between them. the note's duration + 30% seems to work well:
          delay(pauseBetweenNotes);

          noTone(buzzerPin);
        }             
}


void editClock(byte i) {  
    //First Row  hh:mm AM ###.#°F
    //Second Row Feed Amt ###g
    //                             hh    mm    AM     F      g
    byte cursorPositions[][2] = { {1,0},{4,0},{7,0},{15,0},{11,1} };
    lcd.setCursor(cursorPositions[i][0], cursorPositions[i][1]);
    lcd.cursor();
    lcd.blink();
}


void editFeedTime(byte i) {
    /* Alarm 1      ON
     hh:mm AM Weekday           
                                    hh    mm    AM   Weekday */
    byte cursorPositions[][2] = { {1,1},{4,1},{7,1},{9,1} };
    lcd.setCursor(cursorPositions[i][0], cursorPositions[i][1]);
    lcd.cursor();
    lcd.blink();
}


byte CheckFeedTimeStatus() {
    /* Returns:
     0 - No alarms
     1 - Alarm 1 enabled
     2 - Alarm 2 enabled
     3 - Both alarms enabled
    */
    bool FeedTimeStatus = digitalRead(SQW_Pin);
    byte flaggedFeedTimes = Clock.flaggedAlarms();

    //INTSQW is Active-Low Interrupt
    if (FeedTimeStatus == LOW) {       
        FeederState = Feeding;  //feeding alarm detected
    }
    return flaggedFeedTimes;
}


void fixFeedTimeClockMode(byte FeedTimeIndex, byte NewClockMode) {
    /** ********************************************************
     * Fixes feedTime clockmode if clock.clockmode is switch
     * between 12hr and 24hr clockmodes
     ********************************************************* */
     
    AlarmTime feed = Clock.readAlarm(FeedTimeIndex);

        if (feed.Hour > 12) {
            feed.ClockMode = PMhr;
        }
        else {
            feed.ClockMode = AMhr;
        }
        feed.Hour %= 12;
        if (feed.Hour == 0) { feed.Hour = 12; }
    
    else if (NewClockMode == M24hr) {
        feed.Hour %= 12;
        feed.Hour += (12 * feed.ClockMode);
        feed.ClockMode = M24hr;
    }
    Clock.setAlarm(feed, FeedTimeIndex);

}


//void displayNextFeed(byte i){
//      DateTime NowTime;   
//      NowTime = Clock.read();
//       
//      AlarmTime feed;
//      
//      if (feed.Enabled){
//          if(i == 1){
//           feed = Clock.readAlarm(FeedTime1);
//         }
//         else{
//           feed = Clock.readAlarm(FeedTime2);
//          }
//      }
//      else {
//        //write the word null on the screen
//      }
//  
//}


//void showAmtOfFood(){
//  
//}
//
//
//void feederDoor(){
// 
//
//    if(distConfirmed){  //and feed plate empty
//      
//      }
//}


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


//bool checkDistance(){
//  unsigned int distance = sonar.ping_cm(); // Ping in cm
//    
//     if (distance < maxDistance){
//      return true;
//     }
//     else{ return false;}
//}

/*
 * 
 */
