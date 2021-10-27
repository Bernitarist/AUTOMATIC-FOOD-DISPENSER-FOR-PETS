void toggleLED(bool ledON) {
    bool ledState;

    ledState = digitalRead(LED_Pin);                //get the state of LED
    if (ledON == true) {
        digitalWrite(LED_Pin, !ledState);           //do the opposite
    }
    else {
        digitalWrite(LED_Pin, LOW);
    }
}