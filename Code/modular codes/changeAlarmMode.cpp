void changeAlarmMode(byte i, bool increment) {
    /*  Change AlarmMode to 0=Daily, 1=Weekday, 2=Weekend, 3=Once
     *    i = 1 Alarm1
     *      = 2 Alarm2
     */
        // Instantiates object as struct AlarmTIme
        AlarmTime alarm = Clock.readAlarm(i);
        int AlarmMode = alarm.AlarmMode;;

        if (increment == true) {
            AlarmMode += 1;
            AlarmMode %= 4;
        }
        else {
            AlarmMode -= 1;
            AlarmMode %= 4;
        }

        if (AlarmMode < 0) { AlarmMode = 3; }
        //Serial.print("AlarmMode = ");Serial.println(AlarmMode,BIN);
        alarm.AlarmMode = byte(AlarmMode);
        Clock.setAlarm(alarm, i);
    
}