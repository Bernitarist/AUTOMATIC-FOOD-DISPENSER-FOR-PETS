void setup() {
    // Get the start time
    RunTime = millis();

        // Pin Modes            
    pinMode(LED_Pin, OUTPUT);
    digitalWrite(LED_Pin, LOW);
    pinMode(BUZZER_Pin, OUTPUT);
    digitalWrite(BUZZER_Pin, LOW);

         //LCD Stuff        
    lcd.init();
    lcd.clear();
    lcd.backlight();
    
    lcd.createChar(1, upArrow);   //Create custom lcd characters
    lcd.createChar(2, downArrow);
    lcd.createChar(3, bothArrows);
    lcd.createChar(4, feedArrow);

         //Clock Stuff        
    Clock.begin();
    if (Clock.getOSFStatus() == true) {
        ClockState = PowerLoss;
    }
    CurrentTemperature = getTemperatureValue();

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

