void changeHour(byte i, bool increment) {
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
        //Clock
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
        alarm.Hour = byte(Hour);      
        Clock.setAlarm(alarm, 1);
        break;
    case alarm2:
        //alarm2
        alarm.Hour = byte(Hour);
        Clock.setAlarm(alarm, 2);
        break;
    default:
        //Clock
        NowTime.Hour = Hour;
        Clock.write(NowTime);
        break;
    }
    
}