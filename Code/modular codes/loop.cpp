void loop() {
    static long previousMillis = 0;

    switch (ClockState) {
    case PowerLoss:
        displayClock(false);
        //Flash Clock
        if ((millis() - previousMillis) >= flashInterval) {
            previousMillis = millis();
            if (bDisplayStatus == true) {
                lcd.noDisplay();
            }
            else {
                lcd.display();
            }
            bDisplayStatus = !bDisplayStatus;
        }
        break;
    case ShowClock:
        displayClock(false);
        break;
    case ShowAlarm1:
        //AlarmRunTime is defined by toggleShowAlarm
        if ((millis() - AlarmRunTime) <= Alarm_View_Pause) {
            displayAlarm(alarm1);
        }
        else {
            ClockState = ShowClock;
            displayClock(true);
        }
        break;
    case ShowAlarm2:
        //AlarmRunTime is defined by toggleShowAlarm
        if ((millis() - AlarmRunTime) <= Alarm_View_Pause) {
            displayAlarm(alarm2);
        }
        else {
            ClockState = ShowClock;
            displayClock(true);
        }
        break;
    case Alarm:
        displayClock(true);
        //Flash Clock
        if ((millis() - previousMillis) >= flashInterval) {
            previousMillis = millis();
            if (bDisplayStatus == true) {
                lcd.noDisplay();
            }
            else {
                lcd.display();
            }
            bDisplayStatus = !bDisplayStatus;
            toggleLED(true);
            toggleBuzzer();
        }
        break;
    case EditClock:
        editClock(cpIndex);
        displayClock(true);
        break;
    case EditAlarm1:
        editAlarm(cpIndex);
        displayAlarm(alarm1);
        break;
    case EditAlarm2:
        editAlarm(cpIndex);
        displayAlarm(alarm2);
        break;
    default:
        displayClock(true);
        break;
    }
    LtKey.process();
    RtKey.process();
    SnoozeKey.process();
    ActiveAlarms = CheckAlarmStatus();  //Returns which alarms are activated
}