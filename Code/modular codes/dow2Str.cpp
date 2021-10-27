String dow2Str(byte bDow) {
    // Day of week to string or char array. DOW 1=Sunday, 0 is undefined
    static const char* str[] = { "---", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    if (bDow > 7) bDow = 0;
    return(str[bDow]);
}