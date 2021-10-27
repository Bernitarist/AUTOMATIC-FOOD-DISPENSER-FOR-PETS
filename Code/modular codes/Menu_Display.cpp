void setup() {
	menu();
}
void loop() {
	
	bool any = isAnyPressed();

	if (any == false) {
		lcd.clear();
		lcd.setCursor(0, 0);
		//some RTC function

		lcd.setCursor(0, 1);
		lcd.print("next feed:");
		//some RTC function
		delay(4000);
		lcd.setCursor(0, 1);
		lcd.print("                ");
		delay(200);
		lcd.setCursor(0, 1);
		lcd.print("Feed Amt:");
		//some weight function
	}

	if (any == true) {
		setData();
	}
}

void menu() {

	bool up, down, any;

	byte up[8] =
	{
	0b00000,
	0b00100,
	0b01110,
	0b11111,
	0b00100,
	0b00100,
	0b00100,
	0b00100
	};
	byte down[8] =
	{
	0b00000,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b11111,
	0b01110,
	0b00100
	};

	lcd.createChar(0, up);
	lcd.createChar(1, down);

	any = isAnyPressed();
	while (any == false) {
		lcd.setCursor(0, 0);
		lcd.print("Press ")
		lcd.setCursor(6, 0);
		lcd.write(byte(0));
		lcd.setCursor(7, 0);
		lcd.print(" to menu ");
		lcd.setCursor(0, 1);
		lcd.print("and ");
		lcd.write(byte(1));
		lcd.setCursor(5, 1);
		lcd.print(" to cancel ");
	}
	up = isUpPressed();
	if (up) {
		setData();
	}
	if (down) {
		loop();
	}
}

bool isAnyPressed() {
	if () {
		return true;
	}
}
bool isUpPressed() {
	if () {
		return true;
	}
}

void setData() {
	byte up[8] =
	{
	0b00000,
	0b00100,
	0b01110,
	0b11111,
	0b00100,
	0b00100,
	0b00100,
	0b00100
	};
	byte down[8] =
	{
	0b00000,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b11111,
	0b01110,
	0b00100
	};

	lcd.createChar(0, up);
	lcd.createChar(1, down);

	lcd.clear();

	lcd.setCursor(0, 0);
	lcd.print("Set clock?");

	lcd.setCursor(0, 1);	
	lcd.write(byte(0));
	lcd.setCursor(1, 1);
	lcd.print("(YES)  ");
	lcd.setCursor(8, 1);
	lcd.write(byte(1));
	lcd.setCursor(9, 1);
	lcd.print("(NO)   ");

	if (yes) {
		//some RTC function
	}
	if(no) {
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Set feed time 1");
		lcd.setCursor(0, 1);
		//some RTC function

		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Set feed time 2");
		lcd.setCursor(0, 1);
		//some RTC function

		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Set amt of food");
		lcd.setCursor(0, 1);
		//some weight function
	}
}




