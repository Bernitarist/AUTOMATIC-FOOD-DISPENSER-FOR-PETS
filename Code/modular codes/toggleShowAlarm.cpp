void toggleShowAlarm(byte i) {
    
        if (i == 2) {
            ClockState = ShowAlarm2;
        }
        else {
            ClockState = ShowAlarm1;
        }
        AlarmTime alarm;
        alarm = Clock.readAlarm(i);
        alarm.Enabled = !alarm.Enabled;
        Clock.setAlarm(alarm, i);
        AlarmRunTime = millis();
        displayAlarm(i, true);
    //otherwise do nothing
}