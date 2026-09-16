// Simple example with 2 SeeSaw Encoder modules, 2 SeeSaw NeoKey 1x4 and an I2C LCD Display 4x20

#include "Arduino.h"
#include "eOS3.h"
#include "LCDi2c.h"
#include "Seesaw.h"

// put all #defines here, e.g. for buttons, encoders, lcd ...
#define LCD_ROWS    4
#define LCD_COLUMNS 20

// put all the class initialisers here
eOS3 eos;
LCDi2c lcd; // default I2C address 0x27
SeesawEncoder encS1(0x36); // default I2C address 0x36
SeesawEncoder encS2(0x37); // default I2C address 0x37, A0 bridged
SeesawNeoKey4 keyA(0x30); // default I2C address 0x30
SeesawNeoKey4 keyB(0x31); // default I2C address 0x31, A0 bridged
Encoder enc1(REVERSE);
AbsoluteLevels enc1Button(HOME, PARAMETER);
Encoder enc2(REVERSE);
AbsoluteLevels enc2Button(HOME, PARAMETER);
SelectCategory category(2);
Button2nd btn1_2nd(KEY, "Next");
Button2nd btn2_2nd(KEY, "Last");
Button2nd btn3_2nd(KEY, "Select_Last");
Button2nd btn4_2nd(KEY, "Select_Manual");
Button2nd btn5_2nd(KEY, "Select_Active");
Button2nd btn6_2nd(KEY, "Clear_Cmd");
Channel chan;
Shift shift;
Acceleration acc;
Control2nd btn2nd;

// function prototypes
void display(); // callback for page and parameter data updates

void setup() {
	// put your setup code here, to run once
	lcd.begin(LCD_ROWS, LCD_COLUMNS);
	lcd.cls();
	lcd.printf("eOS3 v1.0.0");
	lcd.locate(2, 1);
	lcd.printf("Connecting ...");
	encS1.begin(); // start I2C communication of encoder 1
	encS2.begin(); // start I2C communication of encoder 2
	keyA.begin(); // start I2C communication of keyA 1x4
	keyB.begin(); // start I2C communication of keyB 1x4
	category.callbackPage(display);
	// here you can add more parameters in the list 
	category.parameter(INTENSITY, "Intens");
	category.parameter(FOCUS, "Pan");
	category.parameter(FOCUS, "Tilt");
	category.parameter(FOCUS, "X Focus");
	category.parameter(FOCUS, "Y Focus");
	category.parameter(FOCUS, "Z Focus");
	category.parameter(COLOR, "Red");
	category.parameter(COLOR, "Blue");
	category.parameter(COLOR, "Green");
	category.parameter(COLOR, "White");
	category.parameter(COLOR, "Cyan");
	category.parameter(COLOR, "Magenta");
	category.parameter(COLOR, "Yellow");
	category.parameter(COLOR, "CTO");
	category.parameter(COLOR, "Hue");
	category.parameter(COLOR, "Saturation");
	category.parameter(IMAGE, "Gobo Index/Speed", "Gobo I/S");
	category.parameter(IMAGE, "Gobo Select", "Gobo Sel");
	category.parameter(IMAGE, "Gobo Index/Speed 2", "Gobo2 I/S");
	category.parameter(IMAGE, "Gobo Select 2", "Gobo2 Sel");
	category.parameter(FORM, "Edge");
	category.parameter(FORM, "Zoom");
	category.parameter(FORM, "Iris");
	category.parameter(FORM, "Diffusion");
	category.parameter(FORM, "Shutter Strobe", "Strobe");
	category.parameter(SHUTTER, "Frame Thrust A", "Frame A");
	category.parameter(SHUTTER, "Frame Angle A", "Angle A");
	category.parameter(SHUTTER, "Frame Thrust C","Thrust C");
	category.parameter(SHUTTER, "Frame Angle C", "Angle C");
	category.parameter(SHUTTER, "Frame Thrust B", "Thrust B");
	category.parameter(SHUTTER, "Frame Angle B", "Angle B");
	category.parameter(SHUTTER, "Frame Thrust D", "Thrust D");
	category.parameter(SHUTTER, "Frame Angle D", "Angle D");
	category.parameter(SHUTTER, "Frame Assembly", "Assembly");
	eos.begin(); // for USB connection
	}

void loop() {
	// put update functions for buttons and encoders here
	eos.update();
	shift.update(keyB.button(4));
	acc.update(keyA.button(4));
	btn2nd.update(keyA.button(4));
	btn1_2nd.update(keyA.button(1));
	btn2_2nd.update(keyB.button(1));
	btn3_2nd.update(keyA.button(2));
	btn4_2nd.update(keyA.button(3));
	btn5_2nd.update(keyB.button(2));
	btn6_2nd.update(keyB.button(3));
	enc1.update(encS1.position());
	enc1Button.update(encS1.button());
	enc2.update(encS2.position());
	enc2Button.update(encS2.button());
	category.update(keyA.button(1), keyA.button(2), keyA.button(3), keyB.button(1), keyB.button(2), keyB.button(3));
	}

void maintain() {
	/*
	maintain() is called when new OSC data comes in
	this function should not deleted to work the whole code properly!
	if you don't need it, leave it empty!
	put all your parsers and display handling here
	*/
	uint8_t enc = category.parse();
	if (enc == 1) {
		lcd.clp(4, 1, 10);
		if(category.active(1)) {
			lcd.locate(4, 1);
			lcd.print(category.value(1), 3);
			}
		return;
		}

	if (enc == 2) {
		lcd.clp(4, 11, 10);
		if(category.active(2)) {
			lcd.locate(4, 11);
			lcd.print(category.value(2), 3);
			}
		return;
		}

	if(chan.parse()) {
		lcd.locate(1, 1);
		lcd.clr(1);
		lcd.print("Chan ");
		lcd.print(chan.selection().c_str());
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
	lcd.clr(2);
	lcd.locate(2, 1);
	lcd.printf("Connected!");
	delay(1000);
	lcd.cls();
	lcd.locate(1, 1);
	lcd.clr(1);
	lcd.printf("Chan ");
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
	enc1.parameter(category.parameter(1));
	enc2.parameter(category.parameter(2));
	enc1Button.parameter(category.parameter(1));
	enc2Button.parameter(category.parameter(2));
	lcd.clr(2);
	lcd.locate(2, 1);
	lcd.print(category.categoryName().c_str());
	lcd.print(" ");
	lcd.print(category.page());
	lcd.print("/");
	lcd.print(category.pages());
	lcd.clr(3);
	lcd.locate(3, 1);
	lcd.print(category.alias(1).c_str());
	lcd.locate(3, 11);
	lcd.print(category.alias(2).c_str());
	lcd.clr(4);
	if(category.active(1)) {
		lcd.locate(4, 1);
		lcd.print(category.value(1), 3);
		}
	if(category.active(2)) {
		lcd.locate(4, 11);
		lcd.print(category.value(2), 3);
		}
	}
