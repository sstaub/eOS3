// Simple example with 2 SeeSaw Encoder modules, 1 SeeSaw NeoKey 1x4 and an I2C LCD Display 4x20

#include "Arduino.h"
#include "eOS3.h"
#include "LCDi2c.h"
#include "Seesaw.h"

#define LCD_ROWS    4
#define LCD_COLUMNS 20

eOS3 eos;

LCDi2c lcd; // default I2C address 0x27

SeesawEncoder encS1(0x36); // default I2C address 0x36
SeesawEncoder encS2(0x37); // default I2C address 0x37, A0 bridged
SeesawNeoKey4 keyA(0x30); // default I2C address 0x30
SeesawNeoKey4 keyB(0x31); // default I2C address 0x30, A0 bridged

Encoder enc1(REVERSE);
AbsoluteLevels enc1Button(HOME, PARAMETER);
Encoder enc2(REVERSE);
AbsoluteLevels enc2Button(HOME, PARAMETER);
SelectCategory selection(2);
Button2nd btn1_2nd(KEY, "Next");
Button2nd btn2_2nd(KEY, "Last");
Channel channel;
Shift shift;
Control2nd btn2nd;

string selctionLast;

void updateDisplay();

void setup() {
	lcd.begin(LCD_ROWS, LCD_COLUMNS);
	lcd.cls();
	lcd.print("eOS3 v1.0.0");
	lcd.locate(2, 1);
	lcd.print("connecting ...");
	encS1.begin(); // start I2C communication of encoder 1
	encS2.begin(); // start I2C communication of encoder 2
	keyA.begin(); // start I2C communication of key 1x4
	keyB.begin(); // start I2C communication of key 1x4
	selection.callback(updateDisplay);
	selection.parameter(INTENSITY, "Intens");
	selection.parameter(INTENSITY, "");
	selection.parameter(FOCUS, "Pan");
	selection.parameter(FOCUS, "Tilt");
	selection.parameter(FORM, "Edge");
	selection.parameter(FORM, "Zoom");
	selection.parameter(FORM, "Iris");
	selection.parameter(FORM, "Diffusn");
	selection.parameter(COLOR, "Red");
	selection.parameter(COLOR, "Blue");
	selection.parameter(COLOR, "Green");
	selection.parameter(COLOR, "White");
	selection.parameter(COLOR, "Cyan");
	selection.parameter(COLOR, "Magenta");
	selection.parameter(COLOR, "Yellow");
	selection.parameter(COLOR, "CTO");
	selection.parameter(SHUTTER, "Frame Thrust A", "Frame A");
	selection.parameter(SHUTTER, "Frame Angle A", "Angle A");
	selection.parameter(SHUTTER, "Frame Thrust C","Thrust C");
	selection.parameter(SHUTTER, "Frame Angle C", "Angle C");
	selection.parameter(SHUTTER, "Frame Thrust B", "Thrust B");
	selection.parameter(SHUTTER, "Frame Angle B", "Angle B");
	selection.parameter(SHUTTER, "Frame Thrust D", "Thrust D");
	selection.parameter(SHUTTER, "Frame Angle D", "Angle D");
	selection.parameter(SHUTTER, "Frame Assembly", "Assembly");
	eos.begin(); // for USB connection
}

void loop() {
	// put update functions for buttons and encoders here
	eos.update();
	shift.update(keyB.button(4));
	btn2nd.update(keyA.button(4));
	btn1_2nd.update(keyA.button(1));
	btn2_2nd.update(keyB.button(1));
	enc1.update(encS1.position());
	enc1Button.update(encS1.button());
	enc2.update(encS2.position());
	enc2Button.update(encS2.button());
	selection.update(keyA.button(1), keyA.button(2), keyA.button(3), keyB.button(1), keyB.button(2), keyB.button(3));
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

	if(channel.parse()) {
		if (channel.selection() != selctionLast) {
			selctionLast = channel.selection();
			lcd.locate(1, 1);
			lcd.clr(1);
			lcd.print("Chan ");
			lcd.print(channel.selection().c_str());
			}
		return;
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

	updateDisplay();
	}

void disconnected() {
	/*
	disconnected() is called when a connection failed
	this function should not deleted to work the whole code properly!
	if you don't need it, leave it empty!
	put all things here when the connection failed like splash screen
	*/

	}

void updateDisplay() {
	enc1.parameter(selection.parameter(1));
	enc2.parameter(selection.parameter(2));
	enc1Button.parameter(selection.parameter(1));
	enc2Button.parameter(selection.parameter(2));
	lcd.clr(2);
	lcd.locate(2, 1);
	lcd.print(selection.categoryName().c_str());
	lcd.locate(2, 11);
	lcd.print("Page ");
	lcd.print(selection.page());
	lcd.print("/");
	lcd.print(selection.pages());
	lcd.clr(3);
	lcd.locate(3, 1);
	if (selection.alias(1) != "")
		lcd.print(selection.alias(1).c_str());
	else
		lcd.print(selection.parameter(1).c_str());
	lcd.locate(3, 11);
	if (selection.alias(2) != "")
		lcd.print(selection.alias(2).c_str());
	else
		lcd.print(selection.parameter(2).c_str());
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