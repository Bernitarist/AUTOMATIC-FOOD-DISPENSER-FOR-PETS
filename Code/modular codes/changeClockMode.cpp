void changeClockMode(byte i, bool increment = true) {
    
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