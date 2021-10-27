byte dot[8] =
{
0b00000,
0b00000,
0b00000,
0b00000,
0b00000,
0b01100,
0b01100,
0b00000
};
for (int i = 3; i < 6; i++) {
    lcd.setCursor(i, 1);
    lcd.write(byte(1));
    delay(900);
};

lcd.setCursor(3, 1);
lcd.print("   ");
delay(700);

void menu() {
    int up, down, left, right;

    lcd.setCursor(0, 0);
    lcd.print("Press ");
}