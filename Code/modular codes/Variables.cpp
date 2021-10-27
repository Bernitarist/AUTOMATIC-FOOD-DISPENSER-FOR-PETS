
/* ***********************************************************
 *                      Global Variables                     *
 * ********************************************************* */

 // notes in the melody:
int melody[] = {
    //NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
    NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6,
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
//int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };
int noteDurations[] = { 16, 16, 16, 16, 16 };

enum States {
    PowerLoss,
    ShowClock,
    ShowAlarm1,
    ShowAlarm2,
    Alarm,
    EditClock,
    EditAlarm1,
    EditAlarm2
};

States ClockState = ShowClock;
States PrevState = EditAlarm2;     // Used for debugging

byte HourType = 0;                // 0=AM/PM, 1=24hour - used in display alarm - to be deleted
bool Fahrenheit = true;           // Or Celsius=false
bool PrevFahrenheit = Fahrenheit;  // Capture previous Fahrenheit
float CurrentTemperature;         // Maybe move as static variable under displayClock function
unsigned long RunTime;            // Used to track time between get temperature value
unsigned long buttonHoldPrevTime = 0.0;  // Used to track button hold times 
unsigned long AlarmRunTime;
DateTime PreviousTime;            // Maybe move as static variable under displayClock function
AlarmTime PreviousAlarm;          // Maybe move as static variable under displayAlarm function
byte EditHourType = 0;            // 0=AM, 1=PM, 2=24hour - used for edit only
byte cpIndex = 0;                 // Cursor Position Index - used for edit mode
byte ActiveAlarms = 0;            // used to store active alarms (not enabled alarms)
bool bHoldButtonFlag = false;     // used to prevent holdButton also activating clickButton
bool bDisplayStatus = true;       // used to track the lcd display on status

//custom LCD characters: https://omerk.github.io/lcdchargen/
//Up arrow
byte upArrow[8] = {
              0b00100,
              0b01110,
              0b11111,
              0b00000,
              0b00000,
              0b00000,
              0b00000,
              0b00000 };
//Down arrow
byte downArrow[8] = {
              0b00000,
              0b00000,
              0b00000,
              0b00000,
              0b00000,
              0b11111,
              0b01110,
              0b00100 };
//Both arrows
byte bothArrows[8] = {
              0b00100,
              0b01110,
              0b11111,
              0b00000,
              0b00000,
              0b11111,
              0b01110,
              0b00100 };
//Skinny letter F
byte feedLetter[8] = {
                0b00000,
                0b00000,
                0b00111,
                0b00100,
                0b00111,
                0b00100,
                0b00100,
                0b00100 };
