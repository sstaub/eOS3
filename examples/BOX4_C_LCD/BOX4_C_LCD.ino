// Simple example with hardware encoder, buttons and LCD Display 4x20

#include "Arduino.h"
#include "LCD.h"
#include "eOS3.h"


#define ENC_1_A      11
#define ENC_1_B      12
#define ENC_1_BTN    10
#define ENC_2_A      14
#define ENC_2_B      15
#define ENC_2_BTN    13

#define BTN_INTENS   2
#define BTN_FOCUS    3
#define BTN_COLOR    4
#define BTN_IMAGE    6
#define BTN_FORM     7
#define BTN_SHUTTER  8
#define BTN_ACC      9
#define BTN_SHIFT    5

#define LCD_RS       21
#define LCD_ENABLE   20
#define LCD_D4       19
#define LCD_D5       18
#define LCD_D6       17
#define LCD_D7       16

#define LCD_ROWS     4
#define LCD_COLUMNS  20

LCD lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7); // rs, enable, d4, d5, d6, d7

eOS3 eos;

Shift shift(BTN_SHIFT);
Acceleration accelaration(BTN_ACC);
Control2nd button2nd(BTN_ACC);
Encoder enc1(ENC_1_A, ENC_1_B, REVERSE);
AbsoluteLevels enc1Button(ENC_1_BTN, HOME, PARAMETER);
Encoder enc2(ENC_2_A, ENC_2_B, REVERSE);
AbsoluteLevels enc2Button(ENC_2_BTN, HOME, PARAMETER);
SelectCategory category(BTN_INTENS, BTN_FOCUS, BTN_COLOR, BTN_IMAGE, BTN_FORM, BTN_SHUTTER, 2);
Channel channel;
Button2nd btn1_2nd(BTN_INTENS, KEY, "Next");
Button2nd btn2_2nd(BTN_IMAGE, KEY, "Last");
Button2nd btn3_2nd(BTN_FOCUS, KEY, "Select_Last");
Button2nd btn4_2nd(BTN_COLOR, KEY, "Select_Manual");
Button2nd btn5_2nd(BTN_FORM, KEY, "Select_Active");
Button2nd btn6_2nd(BTN_SHUTTER, KEY, "Clear");

void updateDisplayDyn();

void encoder(uint8_t enc) {
	if (enc == 1) {
		lcd.clp(4, 1, 10);
		lcd.locate(4, 1);
		if (category.active(1))
			lcd.print(category.value(1), 3);
		return;
		}

	if (enc == 2) {
		lcd.clp(4, 11, 10);
		lcd.locate(4, 11);
		if (category.active(2))
			lcd.print(category.value(2), 3);
		return;
		}
	}

void chan() {
	lcd.locate(1, 1);
	lcd.clr(1);
	lcd.printf("Chan %.15s", channel.selection().c_str());
	}

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	//pinMode(BTN_SHIFT, INPUT_PULLUP);
	Serial.begin(11520);
	lcd.begin(LCD_ROWS, LCD_COLUMNS);
	lcd.cls();
	lcd.print("eOS3 v1.0.0");
	lcd.locate(2, 1);
	lcd.print("connecting ...");
	channel.callback(chan);
	category.callbackPage(updateDisplayDyn);
	category.callbackEncoder(encoder);
	category.parameter(INTENSITY, "Intens");
	category.parameter(INTENSITY, "");
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
	eos.begin(); // USB	
	}

void loop() {
	eos.update();
	shift.update();
	accelaration.update();
	button2nd.update();
	btn1_2nd.update();
	btn2_2nd.update();
	btn3_2nd.update();
	btn4_2nd.update();
	btn5_2nd.update();
	btn6_2nd.update();
	enc1.update();
	enc1Button.update();
	enc2.update();
	enc2Button.update();
	category.update();
	}

void maintain() {
	channel.parse();
	category.parse();
	}

void connected() {
	lcd.clr(2);
	lcd.locate(2, 1);
	lcd.print("connected!");
	delay(1000);
	lcd.cls();
	lcd.locate(1, 1);
	lcd.clr(1);
	lcd.print("Chan ");
	updateDisplayDyn();
	}

void disconnected() {
	Serial.println("Disconnected");
	}

void updateDisplayDyn() {
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
	lcd.printf("%.10s", category.alias(1).c_str());
	lcd.locate(3, 11);
	lcd.printf("%.10s", category.alias(2).c_str());
	lcd.clr(4);
	if (category.active(1))
		lcd.print(category.value(1), 3);
	else
		lcd.clp(4, 1, 10);
	lcd.locate(4, 11);
	if (category.active(2))
		lcd.print(category.value(2), 3);
	else
		lcd.clp(4, 11, 10);
	}
