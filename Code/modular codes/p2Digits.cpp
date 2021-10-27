String p2Digits(int numValue) {
    // utility function for digital clock display
    // converts int to two digit char array
    String str;

    if (numValue < 10) {
        str = "0" + String(numValue);
    }
    else {
        str = String(numValue);
    }
    return str;
}