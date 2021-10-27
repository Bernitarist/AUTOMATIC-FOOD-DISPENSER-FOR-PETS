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