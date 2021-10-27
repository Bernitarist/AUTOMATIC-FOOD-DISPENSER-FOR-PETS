void displayClock(bool changeFlag) {
    /* ***************************************************** *
     * Display clock - skips update if there are no changes
     *
     * Parameters:
     *   changeFlag - true forces display refresh
     *              - false does nothing
     * ***************************************************** */
    DateTime NowTime;            //create DateTime struct from Library
    NowTime = Clock.read();      // get the latest clock values

    // CheckFlag Section:
    // The DS3231 temperature can be read once every 64 seconds.
    // Check the temperature every 65 seconds OR
    // Check the temperature if Fahrenheit changes
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

        //Second Row  dow mm/dd/yyyy
        lcd.setCursor(0, 1);                       // Column, Row
        lcd.print(dow2Str(NowTime.Dow));          // Integer Day of the week
                                                  // convert to String with
                                                  // an optional leading zero
        lcd.print(" ");
        lcd.print(p2Digits(NowTime.Month));
        lcd.print("/");
        lcd.print(p2Digits(NowTime.Day));
        lcd.print("/");
        int i = 2000 + NowTime.Year;
        lcd.print(i);
        lcd.write(4);                             //Skinny letter A
        lcdAlarmIndicator();                      //lcd.print A1, A2, BA, or -

        PreviousTime = Clock.read();
    }
}