void ButtonHold(Button& b) {
    //Clock States:
    // PowerLoss, ShowClock, ShowAlarm1, ShowAlarm2, Alarm, EditClock, EditAlarm1, EditAlarm2
    // static unsigned long buttonHoldPrevTime = 0;

    // To ignore back to back button hold? 
    if ((millis() - buttonHoldPrevTime) > 2000) {
        switch (ClockState) {
        case PowerLoss:
            //Any button held
            //Edit main clock display
            ClockState = EditClock;
            cpIndex = 0;
            buttonHoldPrevTime = millis();
            bHoldButtonFlag = true;
            Clock.clearOSFStatus();
            break;
        case ShowClock:
            switch (b.pinValue()) {
            case Snooze_Pin:
                //Edit main clock display
                ClockState = EditClock;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                break;
            case Lt_Pin:
                //Edit Alarm1
                ClockState = EditAlarm1;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayAlarm(1, false);
                break;
            case Rt_Pin:
                //Edit Alarm2
                ClockState = EditAlarm2;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayAlarm(2, true);
                break;
            default:
                break;
            }
            break;
        case ShowAlarm1:
            switch (b.pinValue()) {
            case Snooze_Pin:
                break;
            case Lt_Pin:
                ClockState = EditAlarm1;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayAlarm(1, true);
                //Switch to edit mode
                break;
            case Rt_Pin:
                //Do Nothing
                break;
            default:
                break;
            }
            break;
        case ShowAlarm2:
            switch (b.pinValue()) {
            case Snooze_Pin:
                break;
            case Lt_Pin:
                break;
            case Rt_Pin:
                //Edit Alarm2
                ClockState = EditAlarm2;
                cpIndex = 0;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayAlarm(2, true);
                break;
            default:
                break;
            }
            break;
        case Alarm:
            //Alarm Mode
            switch (b.pinValue()) {
            case Snooze_Pin:
                Snooze();             //Snooze alarm for 9 minutes
                ClockState = ShowClock;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayClock(true);
                break;
            case Lt_Pin:
            case Rt_Pin:
                //turn off alarms
                clearAlarms();
                ClockState = ShowClock;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayClock(true);
                break;
            default:
                //do nothing
                break;
            }
            break;
        case EditClock:  //Edit Clock
            switch (b.pinValue()) {
            case Snooze_Pin:
                lcd.noBlink();
                lcd.noCursor();
                ClockState = ShowClock;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                break;
            case Lt_Pin:
            case Rt_Pin:
            default:
                break;
            }
            break;
        case EditAlarm1:  //Edit Alarm1
            switch (b.pinValue()) {
            case Snooze_Pin:
                lcd.noBlink();
                lcd.noCursor();
                ClockState = ShowClock;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayClock(true);
                break;
            case Lt_Pin:
            case Rt_Pin:
            default:
                break;
            }
            break;
        case EditAlarm2:  //Edit Alarm1
            switch (b.pinValue()) {
            case Snooze_Pin:
                lcd.noBlink();
                lcd.noCursor();
                ClockState = ShowClock;
                buttonHoldPrevTime = millis();
                bHoldButtonFlag = true;
                displayClock(true);
                break;
            case Lt_Pin:
            case Rt_Pin:
            default:
                break;
            }
            break;
        default:
            //todo
            break;
        }
    }
}