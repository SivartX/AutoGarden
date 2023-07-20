/*
 Name:		GardenSpinkler.ino
 Created:	7/2/2023 12:06:56 PM
 Author:	Bowman
*/



#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <RTClib.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

RTC_DS3231 DS3231;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

const int btnPinOne = 4;
int btnStateOne = 0;

const int btnPinTwo = 2;
int btnStateTwo = 0;

const int btnPinThree = 3;
int btnStateThree = 0;

int mainMenu = 0;
String daysOfweek[] = { "year" ,"month","day","hour","minute","second" };
int maxDaysOfweek[] = { 100,12,31,24,59,59 };
int flag = 0;

void printTimeToSerial(DateTime);
void printTextToOLED(Adafruit_SSD1306&, DateTime, String);
DateTime adjustTime(Adafruit_SSD1306&, DateTime, String, int, int);
int loopInRange(int, int, int);


// the setup function runs once when you press reset or power the board
void setup() {

	Serial.begin(9600);
	Wire.begin();

	DS3231.begin();
	// Uncomment the line below if you want to set the time only once
	//setDS3231Time(2023, 7, 10, 21, 15, 0);

	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
		Serial.println(F("SSD1306 allocation failed"));
		for (;;); // Don't proceed, loop forever
	}

	// Clear the buffer
	display.clearDisplay();

	// Set text size to 1x, color to white, and start at (0,0)
	display.setTextSize(1);
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(50, 0);

	// Print 'Hello, world!'
	display.println("Loading...");

	// Display the buffer contents
	display.display();

	pinMode(btnPinOne, INPUT_PULLUP);
	pinMode(btnPinTwo, INPUT_PULLUP);
	pinMode(btnPinThree, INPUT_PULLUP);

}

// the loop function runs over and over again forever
void loop() {

	// Get the current time from the DS3231 module
	DateTime now = DS3231.now();

	btnStateOne = digitalRead(btnPinOne);
	btnStateTwo = digitalRead(btnPinTwo);
	btnStateThree = digitalRead(btnPinThree);
	
	if (btnStateTwo == HIGH) {
		 //Increment mainMenu only when button is pressed
		mainMenu = (mainMenu + 1) % 4;  // This will cycle mainMenu through 0, 1, 2, 3
		delay(200);
	}

	if (btnStateThree == HIGH) {
		// Decrement mainMenu only when button is pressed
		mainMenu = (mainMenu + 3) % 4;  // This will cycle mainMenu through 3, 2, 1, 0
		 delay(200);
	}



	String message;

	switch (mainMenu) {

	case 0:

		message = "Set sprinkler Time";
		printTextToOLED(display, now, message);


		break;
	case 1:

		message = "Set alarm duration";
		printTextToOLED(display, now, message);

		break;
	case 2:

		message = "Set alarm Days";
		printTextToOLED(display, now, message);

		break;

	case 3:

		message = "Set master clock";
		printTextToOLED(display, now, message);
		

		while (digitalRead(btnPinOne) == HIGH) {

			now = adjustTime(display, now, daysOfweek[flag], 0, maxDaysOfweek[flag]);
			flag++;
		}
		if (flag == 5) {
			flag = 0;
		}
		
		
			
	
		
			

		break;
	}

	// Always print the message to the OLED display, even if the button hasn't been pressed
	printTextToOLED(display, now, message);



	delay(100);

}

void printTimeToSerial(DateTime now) {

	Serial.print("Current time: ");
	Serial.print(now.day());
	Serial.print("/");
	Serial.print(now.month());
	Serial.print("/");
	Serial.print(now.year());
	Serial.print(" ");
	Serial.print(now.hour());
	Serial.print(":");
	Serial.print(now.minute());
	Serial.print(":");
	Serial.println(now.second());

}
void printTextToOLED(Adafruit_SSD1306& display, DateTime now, String messageOne) {

	// Clear the buffer
	display.clearDisplay();

	// Set text size to 1x, color to white, and start at (0,0)
	display.setTextSize(1);
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(13, 0);

	display.print(now.day());
	display.print("/");
	display.print(now.month());
	display.print("/");
	display.print(now.year());
	display.print(" ");
	display.print(now.hour());
	display.print(":");
	display.print(now.minute());
	display.print(":");
	display.println(now.second());

	display.setCursor(13, 20);
	display.println(messageOne);

	// Display the buffer contents
	display.display();
	delay(100);

}
DateTime adjustTime(Adafruit_SSD1306& display, DateTime now, String part, int min, int max) {

	printTextToOLED(display, now, "Changing " + part);

	bool flag = true;

	while (flag) {

		int value;

		if (part == "year") value = now.year();
		else if (part == "month") value = now.month();
		else if (part == "day") value = now.day();
		else if (part == "hour") value = now.hour();
		else if (part == "minute") value = now.minute();
		else if (part == "second") value = now.second();

		if (digitalRead(btnPinTwo) == HIGH) {
			value++;
			value = loopInRange(value, min, max);
		}
		if (digitalRead(btnPinThree) == HIGH) {
			value--;
			value = loopInRange(value, min, max);
		}

		if (part == "year") now = DateTime(value, now.month(), now.day(), now.hour(), now.minute(), now.second());
		else if (part == "month") now = DateTime(now.year(), value, now.day(), now.hour(), now.minute(), now.second());
		else if (part == "day") now = DateTime(now.year(), now.month(), value, now.hour(), now.minute(), now.second());
		else if (part == "hour") now = DateTime(now.year(), now.month(), now.day(), value, now.minute(), now.second());
		else if (part == "minute") now = DateTime(now.year(), now.month(), now.day(), now.hour(), value, now.second());
		else if (part == "second") now = DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), value);

		
		printTextToOLED(display, now, "Changing " + part);

		if (digitalRead(btnPinOne) == HIGH) {
			flag = false;
			delay(200);
		}
	}
	return now;
}

int loopInRange(int val, int min, int max) {

		if (val < min) {
			return max;
		}
		else if (val > max) {
			return min;
		}
		else {
			return val;
		}
}

