byte CheckAlarmStatus() {
    /* Returns:
     0 - No alarms
     1 - Alarm 1 enabled
     2 - Alarm 2 enabled
     3 - Both alarms enabled
    */
    bool AlarmStatus = digitalRead(SQW_Pin);
    byte flaggedAlarms = Clock.flaggedAlarms();

    //INTSQW is Active-Low Interrupt or Square-Wave Output
    if (AlarmStatus == LOW) {
        //Alarm detected
        ClockState = Alarm;
    }
    return flaggedAlarms;
}