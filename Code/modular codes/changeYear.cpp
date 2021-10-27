void changeYear(byte i, bool increment) {
    DateTime NowTime;
    NowTime = Clock.read();
    int Year = NowTime.Year;
    if (increment == true) {
        Year += 1;
    }
    else {
        Year -= 1;
    }
    if (Year < 18) { Year = 199; }
    if (Year > 199) { Year = 18; }
    NowTime.Year = byte(Year);
    Clock.write(NowTime);
}