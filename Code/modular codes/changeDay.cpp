void changeDay(byte i, bool increment) {
    DateTime NowTime;
    NowTime = Clock.read();
    int Day = NowTime.Day;
    byte Month = NowTime.Month;
    byte Year = NowTime.Year + 2000;
    byte DaysMax = 31;
    switch (Month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        DaysMax = 31;
        break;
    case 2:
        DaysMax = 28;
        if ((Year % 4 == 0) && (Year % 100 != 0) || (Year % 400 == 0)) {
            //those are the conditions to have a leap year
            DaysMax = 29;
        }
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        DaysMax = 30;
        break;
    default:
        break;
    }
    
    if (increment == true) { Day += 1; }
    else { Day -= 1; }
    if (Day < 1) { Day = DaysMax; }
    if (Day > DaysMax) { Day = 1; }
    
    NowTime.Day = byte(Day);
    Clock.write(NowTime);
}