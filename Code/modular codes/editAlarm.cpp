void editAlarm(byte i) {
    /* Alarm 1      ON
     hh:mm AM Weekday            hh    mm    AM   Weekday */
     //lcd.setCursor(Column, Row);
    byte cursorPositions[][2] = { {1,1},{4,1},{7,1},{9,1} };
    lcd.setCursor(cursorPositions[i][0], cursorPositions[i][1]);
    lcd.cursor();
    lcd.blink();
}