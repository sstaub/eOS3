// Simple example with 2 SeeSaw Encoder modules, 1 SeeSaw NeoKey 1x4 and an I2C LCD Display 4x20

#include "Arduino.h"
#include "eOS3.h"
#include "LCDi2c.h"
#include "Seesaw.h"

#define LCD_ROWS    4
#define LCD_COLUMNS 20

// put all #defines here, e.g. for buttons, encoders, lcd ...
eOS3 eos;
LCDi2c lcd; // default I2C address 0x27
SeesawEncoder encS1(0x36); // default I2C address 0x36
SeesawEncoder encS2(0x37); // default I2C address 0x37, A0 bridged
SeesawNeoKey4 key4; // default I2C address 0x30
Encoder enc1(REVERSE);
AbsoluteLevels enc1Button(HOME, PARAMETER);
Encoder enc2(REVERSE);
AbsoluteLevels enc2Button(HOME, PARAMETER);
SelectParameter selection(2);
Button2nd btn1_2nd(KEY, "Next");
Button2nd btn2_2nd(KEY, "Last");
Channel chan;
Shift shift;
Acceleration acc;
Control2nd btn2nd;

// function prototypes
void display(); // callback for page and parameter data updates

void setup() {
	lcd.begin(LCD_ROWS, LCD_COLUMNS);
	lcd.cls();
	lcd.print("eOS3 v1.0.0");
	lcd.locate(2, 1);
	lcd.print("Connecting ...");
	encS1.begin(); // start I2C communication of encoder 1
	encS2.begin(); // start I2C communication of encoder 2
	key4.begin(); // start I2C communication of key 1x4
	selection.callbackPage(display);
	selection.parameter("Intens");
	selection.parameter("");
	selection.parameter("Pan");
	selection.parameter("Tilt");
	selection.parameter("Edge");
	selection.parameter("Zoom");
	selection.parameter("Iris");
	selection.parameter("Diffusion");
	selection.parameter("Red");
	selection.parameter("Blue");
	selection.parameter("Green");
	selection.parameter("White");
	selection.parameter("Cyan");
	selection.parameter("Magenta");
	selection.parameter("Yellow");
	selection.parameter("CTO");
	selection.parameter("Frame Thrust A", "Frame A");
	selection.parameter("Frame Angle A", "Angle A");
	selection.parameter("Frame Thrust C","Thrust C");
	selection.parameter("Frame Angle C", "Angle C");
	selection.parameter("Frame Thrust B", "Thrust B");
	selection.parameter("Frame Angle B", "Angle B");
	selection.parameter("Frame Thrust D", "Thrust D");
	selection.parameter("Frame Angle D", "Angle D");
	selection.parameter("Frame Assembly", "Assembly");
	eos.begin(); // for USB connection
}

void loop() {
	// put update functions for buttons and encoders here
	eos.update();
	shift.update(key4.button(4));
	acc.update(key4.button(3));
	btn2nd.update(key4.button(4));
	btn1_2nd.update(key4.button(1));
	btn2_2nd.update(key4.button(2));
	enc1.update(encS1.position());
	enc1Button.update(encS1.button());
	enc2.update(encS2.position());
	enc2Button.update(encS2.button());
	selection.update(key4.button(2), key4.button(1));
	}

void maintain() {
	/*
	maintain() is called when new OSC data comes in
	this function should not deleted to work the whole code properly!
	if you don't need it, leave it empty!
	put all your parsers and display handling here
	*/

	uint8_t enc = selection.parse();
	if (enc == 1) {
		lcd.clp(4, 1, 10);
		if(selection.active(1)) {
			lcd.locate(4, 1);
			lcd.print(selection.value(1), 3);
			}
		return;
		}

	if (enc == 2) {
		lcd.clp(4, 11, 10);
		if(selection.active(2)) {
			lcd.locate(4, 11);
			lcd.print(selection.value(2), 3);
			}
		return;
		}

	if (chan.parse()) {
		lcd.locate(1, 1);
		lcd.clr(1);
		lcd.print("Chan: ");
		lcd.print(chan.selection().c_str());
		}
	}

void connected() {
	/*
	connected() is called when a new connection is established
	this function should not deleted to work the whole code properly!
	if you don't need it, leave it empty!
	put all the init functions for fader and direct selects here,
	also filters and subscribtions
	*/
	lcd.clr(2);
	lcd.locate(2, 1);
	lcd.print("Connected!");
	delay(1000);
	lcd.cls();
	lcd.locate(1, 1);
	lcd.clr(1);
	lcd.print("Chan ");
	display();
	}

void disconnected() {
	/*
	disconnected() is called when a connection failed
	this function should not deleted to work the whole code properly!
	if you don't need it, leave it empty!
	put all things here when the connection failed like splash screen
	*/
	lcd.cls();
	lcd.print("Connection fail!");
	delay(1000);
	lcd.locate(2, 1);
	lcd.print("Rebooting ...");
	delay(1000);
	eos.reboot();
	}

void display() {
	enc1.parameter(selection.parameter(1));
	enc2.parameter(selection.parameter(2));
	enc1Button.parameter(selection.parameter(1));
	enc2Button.parameter(selection.parameter(2));
	lcd.clr(2);
	lcd.locate(2, 1);
	lcd.print("Page ");
	lcd.print(selection.page());
	lcd.print("/");
	lcd.print(selection.pages());
	lcd.clr(3);
	lcd.locate(3, 1);
	lcd.print(selection.alias(1).c_str());
	lcd.locate(3, 11);
	lcd.print(selection.alias(2).c_str());
	lcd.clr(4);
	if(selection.active(1)) {
		lcd.locate(4, 1);
		lcd.print(selection.value(1), 3);
		}
	if(selection.active(2)) {
		lcd.locate(4, 11);
		lcd.print(selection.value(2), 3);
		}
	}