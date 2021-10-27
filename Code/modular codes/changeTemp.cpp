void changeTemp() {
    //change the temperature to F or C
    Fahrenheit = !Fahrenheit;
    CurrentTemperature = getTemperatureValue();
    RunTime = millis();
    displayClock(true);
}