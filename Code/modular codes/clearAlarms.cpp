void clearAlarms() {
    //Clear alarm flags
    Clock.clearAlarms();
    //toggleBuzzer();
    toggleLED(true);
    lcd.display();                     // Just in case it was off
}