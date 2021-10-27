void changeMinute(byte i, bool increment) {
    
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