void Snooze() {
    /* Begin the clock.snoozealarm method to delay the alarm
    *  for SnoozePeriod.  It also clears alarm flags.
    */
    // Global variable ActiveAlarms will have which alarm is active
    Serial.println("Snooze Activated");
    switch (ActiveAlarms) {
    case 0:
        //No flagged alarms
        break;
    case alarm1:
        //alarm 1
        Clock.snoozeAlarm(1, SnoozePeriod);
        break;
    case alarm2:
        //alarm 2
        Clock.snoozeAlarm(2, SnoozePeriod);
        break;
    case 3:
        //both alarms
        Clock.snoozeAlarm(1, SnoozePeriod);
        Clock.snoozeAlarm(2, SnoozePeriod);
        break;
    default:
        //do nothing
        break;
    }
    toggleLED(true);                  // Confirm LED turned off
    lcd.display();                     // Just in case it was off
}