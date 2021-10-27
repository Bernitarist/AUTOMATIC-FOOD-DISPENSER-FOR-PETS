#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup(){
    lcd.init();
    lcd.clear();
    lcd.backlight();
	welcome();
	delay(4000);
	lcd.clear();
}
void loop() {
}

void welcome() {
    byte zero[8] =
    {
    0b00000,
    0b11000,
    0b11000,
    0b10100,
    0b10011,
    0b10000,
    0b10010,
    0b10000
    };
    byte one[8] =
    {
    0b00000,
    0b00110,
    0b00110,
    0b01010,
    0b10010,
    0b00010,
    0b10010,
    0b00010
    };
    byte two[8] =
    {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00001,
    0b00010,
    0b00011
    };
    byte three[8] =
    {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11000,
    0b00100,
    0b10010
    };
    byte four[8] =
    {
    0b10011,
    0b01000,
    0b00111,
    0b00010,
    0b00111,
    0b01000,
    0b01000,
    0b00111
    };
    byte five[8] =
    {
    0b10010,
    0b00100,
    0b11011,
    0b00000,
    0b00000,
    0b00001,
    0b00010,
    0b11111
    };
    byte six[8] =
    {
    0b00000,
    0b00000,
    0b11100,
    0b00011,
    0b10001,
    0b00001,
    0b00001,
    0b11110
    };
    byte seven[8] =
    {
    0b00101,
    0b00101,
    0b00101,
    0b11001,
    0b00010,
    0b11100,
    0b00000,
    0b00000
    };


    lcd.createChar(0, zero);
    lcd.createChar(1, one);
    lcd.createChar(2, two);
    lcd.createChar(3, three);
    lcd.createChar(4, four);
    lcd.createChar(5, five);
    lcd.createChar(6, six);
    lcd.createChar(7, seven);

    lcd.setCursor(0, 0);
    lcd.print("WELCOME!!  ");

    lcd.setCursor(10, 0);
    lcd.write(byte(0));
    lcd.setCursor(11, 0);
    lcd.write(byte(1));
    lcd.setCursor(12, 0);
    lcd.write(byte(2));
    lcd.setCursor(13, 0);
    lcd.write(byte(3));
    lcd.setCursor(10, 1);
    lcd.write(byte(4));
    lcd.setCursor(11, 1);
    lcd.write(byte(5));
    lcd.setCursor(12, 1);
    lcd.write(byte(6));
    lcd.setCursor(13, 1);
    lcd.write(byte(7));
}


