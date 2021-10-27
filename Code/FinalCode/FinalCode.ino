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
const byte FeedTime1 = 1;  //all clocks
const byte FeedTime2 = 2;


                                      /*******************
                                      * GLOBAL VARIABLES
                                      ********************/
 unsigned long RunTime;  // Used to track time between get temperature value
 float CurrentTemperature;
 bool Fahrenheit;  // true for Farenheit and false for celcius
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

byte feedLetter[8] = {
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
    lcd.createChar(4, feedLetter);

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
        bHoldButtonFlag = false;  // After a hold button is released, a button click is also registered ignore clicks for SkipClickTime ms  
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
        case Feeding:
            switch (b.pinValue()) {
            case CtrlPin:
                //do nothing
                break;
            case Lt_Pin:
            case Rt_Pin:
                //turn off feeding alarms
                clearAlarms();
                FeederState = ShowClock;
                break;
            default:
                //do nothing
                break;
            }
            break;
        case EditClock:
            switch (b.pinValue()) {
            case CtrlPin:
                //Increments cursor position
                //cpIndex += 1 % 7;
                cpIndex += 1;
                cpIndex %= 4;
                break;
            case Lt_Pin:
                // Decrements value
                // First Row  hh:mm AM ###.#°F
                //             0  1  2       3
                //second row           ##.#Kg
                //                        4
                switch (cpIndex) {
                case 0:
                    //edit Hours
                    changeHour(clock0, false);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(clock0, false);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(clock0, false);
                    break;
                case 3:
                    //Farenheit
                    changeTemp();
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
                    //edit Hours
                    changeHour(clock0, true);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(clock0, true);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(clock0, true);
                    break;
                case 3:
                    //Farenheit
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
            //End EditClock
            break;
        case EditFeedTime1:
            //Edit FeedTime1 Mode
            switch (b.pinValue()) {
            case CtrlPin:
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
                    changeHour(FeedTime1, false);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(FeedTime1, false);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(FeedTime1, false);
                    break;
                case 3:
                    //AlarmMode
                    changeAlarmMode(FeedTime1, false);
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
                    changeHour(FeedTime1, true);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(FeedTime1, true);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(FeedTime1, true);
                    break;
                case 3:
                    //AlarmMode
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
            //Edit FeedTime2 Mode
            switch (b.pinValue()) {
            case CtrlPin:
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
                    changeHour(FeedTime2, false);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(FeedTime2, false);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(FeedTime2, false);
                    break;
                case 3:
                    //AlarmMode
                    changeAlarmMode(FeedTime2, false);
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
                    changeHour(FeedTime2, true);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(FeedTime2, true);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(FeedTime2, true);
                    break;
                case 3:
                    //AlarmMode
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
