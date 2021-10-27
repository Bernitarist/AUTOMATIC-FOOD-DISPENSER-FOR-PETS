void ButtonClick(Button &b) {

    if (bHoldButtonFlag == true) {
        // After a hold button is released, a button click is also registered      
        bHoldButtonFlag = false;
    }
    else {
        //PowerLoss,ShowClock, Alarm, EditClock, EditAlarm1, EditAlarm2
        switch (ClockState) {
        case PowerLoss:
            //any clickbutton and return to ShowClock
            ClockState = ShowClock;
            Clock.clearOSFStatus();
            break;
        case ShowClock:
            //ShowClock Mode
            //show alarm screens
            switch (b.pinValue()) {
            case Snooze_Pin:
                //Do Nothing
                break;
            case Lt_Pin:
                toggleShowAlarm(alarm1);
                break;
            case Rt_Pin:
                toggleShowAlarm(alarm2);
                break;
            default:
                //do nothing
                break;
            }
            break;
            //ShowAlarm1 or ShowAlarm2 does nothing
        case Alarm:
            //Alarm Mode
            switch (b.pinValue()) {
            case Snooze_Pin:
                //Snooze alarm for 9 minutes
                Snooze();
                ClockState = ShowClock;
                break;
            case Lt_Pin:
            case Rt_Pin:
                //turn off alarms
                clearAlarms();
                ClockState = ShowClock;
                break;
            default:
                //do nothing
                break;
            }
            break;
        case EditClock:
            //Edit Clock Mode
            switch (b.pinValue()) {
            case Snooze_Pin:
                //Increments cursor position
                //cpIndex += 1 % 7;
                cpIndex += 1;
                cpIndex %= 7;  //reach the end and return to the beginning
                break;
            case Lt_Pin:
                // Decrements value
                // First Row  hh:mm AM ###.#°F
                //             0  1  2       3
                // Second Row dow mm/dd/yyyy
                //                 4  5    6
                switch (cpIndex) {
                case 0:
                    //edit Hours
                    changeHour(clock0, false);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(clock0, false);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(clock0, false);
                    break;
                case 3:
                    //Farenheit
                    changeTemp();
                    break;
                case 4:
                    //edit month
                    changeMonth(clock0, false);
                    break;
                case 5:
                    //edit day
                    changeDay(clock0, false);
                    break;
                case 6:
                    //edit year
                    changeYear(clock0, false);
                    break;
                default:
                    //do nothing
                    break;
                }
                break;
            case Rt_Pin:
                // Increments value
                switch (cpIndex) {
                case 0:
                    //edit Hours
                    changeHour(clock0, true);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(clock0, true);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(clock0, true);
                    break;
                case 3:
                    //Farenheit
                    changeTemp();
                    break;
                case 4:
                    //edit month
                    changeMonth(clock0, true);
                    break;
                case 5:
                    //edit day
                    changeDay(clock0, true);
                    break;
                case 6:
                    //edit year
                    changeYear(clock0, true);
                    break;
                default:
                    //do nothing
                    break;
                }
                break;
            default:
                //do nothing
                break;
            }
            //End EditClock
            break;
        case EditAlarm1:
            //Edit Alarm1 Mode
            switch (b.pinValue()) {
            case Snooze_Pin:
                //Increments cursor position
                cpIndex += 1;
                cpIndex %= 4;
                //cpIndex += 1 % 4; didn't work
                break;
            case Lt_Pin:
                // Decrements value      hh:mm AM Weekday
                //cpIndex is global
                switch (cpIndex) {
                case 0:
                    //edit Hours
                    changeHour(alarm1, false);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(alarm1, false);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(alarm1, false);
                    break;
                case 3:
                    //AlarmMode
                    changeAlarmMode(alarm1, false);
                    break;
                default:
                    //do nothing
                    break;
                }
                break;
            case Rt_Pin:
                // Increments value
                //cpIndex is global
                switch (cpIndex) {
                case 0:
                    //edit Hours
                    changeHour(alarm1, true);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(alarm1, true);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(alarm1, true);
                    break;
                case 3:
                    //AlarmMode
                    changeAlarmMode(alarm1, true);
                    break;
                default:
                    //do nothing
                    break;
                }
                break;
            default:
                //do nothing
                break;
            }
            break;
        case EditAlarm2:
            //Edit Alarm2 Mode
            switch (b.pinValue()) {
            case Snooze_Pin:
                //Increments cursor position
                cpIndex += 1;
                cpIndex %= 4;
                break;
            case Lt_Pin:
                // Decrements value      hh:mm AM Weekday
                //cpIndex is global
                switch (cpIndex) {
                case 0:
                    //edit Hours
                    changeHour(alarm2, false);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(alarm2, false);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(alarm2, false);
                    break;
                case 3:
                    //AlarmMode
                    changeAlarmMode(alarm2, false);
                    break;
                default:
                    //do nothing
                    break;
                }
                break;
            case Rt_Pin:
                // Increments value
                //cpIndex is global
                switch (cpIndex) {
                case 0:
                    //edit Hours
                    changeHour(alarm2, true);
                    break;
                case 1:
                    //edit Minute
                    changeMinute(alarm2, true);
                    break;
                case 2:
                    //edit ClockMode
                    changeClockMode(alarm2, true);
                    break;
                case 3:
                    //AlarmMode
                    changeAlarmMode(alarm2, true);
                    break;
                default:
                    //do nothing
                    break;
                }
                break;
            default:
                //do nothing
                break;
            }
            break;
        default:
            //todo
            break;
        }
    }
}