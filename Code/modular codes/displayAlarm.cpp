void displayAlarm(byte index, bool changeFlag = false) {
    /* ***************************************************** *
     * Display Alarm Clock
     *
     * Parameters:
     *   index - the integer value of the alarm clock to
     *           display - 1 or 2
     * ***************************************************** */
     /*  Reminder of AlarmTime Structure:
       struct AlarmTime {
       uint8_t Second;       // 0-59 = 6 bits 0=for alarm2
       uint8_t Minute;       // 0-59 = 6 bits
       uint8_t Hour;         // 0-23 = 5 bits
       uint8_t AlarmMode;    // 0=Daily, 1=Weekday, 2=Weekend, 3=Once
       uint8_t ClockMode;    // 0-2; 0=AM, 1=PM, 2=24hour
       bool Enabled;         // true/false
       }
      */
      /* LCD alarm display pseudo code:
         Alarm 1      ON
         hh:mm AM Daily
         Alarm 1      OFF
         hh:mm PM Weekday
         Alarm 1      ON
         hh:mm AM Weekend
         Alarm 2      ON
         hh:mm 24 Once                                         */
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