void lcdAlarmIndicator() {
    byte alarmEnabledStatus;

    alarmEnabledStatus = Clock.alarmStatus();
    /* Returns:
       0 - No alarms
       1 - Alarm 1 enabled
       2 - Alarm 2 enabled
       3 - Both alarms enabled
     */
    switch (alarmEnabledStatus) {
    case 0:
        //No alarms
        lcd.print("-");
        break;
    case 1:
        //alarm 1 enabled
        lcd.write(1); //cA1
        break;
    case 2:
        //alarm 2 enabled
        lcd.write(2); //cA2
        break;
    case 3:
        //both alarms enabled
        lcd.write(3); //cBA
        break;
    default:
        break;
    }
}