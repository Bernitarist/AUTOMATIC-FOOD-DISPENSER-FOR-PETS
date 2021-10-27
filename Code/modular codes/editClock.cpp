void editClock(byte i) {
    //First Row  hh:mm AM ###.#°F
    //Second Row dow mm/dd/yyyyA^
    //                             hh    mm    AM     F     mm    dd   yyyy
    byte cursorPositions[][2] = { {1,0},{4,0},{7,0},{15,0},{5,1},{8,1},{13,1} };
    //lcd.setCursor(Column, Row);
    //Serial.print("editclock position = "); Serial.println(i);
    lcd.setCursor(cursorPositions[i][0], cursorPositions[i][1]);
    lcd.cursor();
    lcd.blink();
}
