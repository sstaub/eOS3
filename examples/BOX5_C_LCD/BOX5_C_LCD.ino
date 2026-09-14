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
SelectDynamic dyn(BTN_INTENS, BTN_FOCUS, BTN_COLOR, BTN_IMAGE, BTN_FORM, BTN_SHUTTER, 2);
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
		if (dyn.active(1))
			lcd.print(dyn.value(1), 3);
		return;
		}

	if (enc == 2) {
		lcd.clp(4, 11, 10);
		lcd.locate(4, 11);
		if (dyn.active(2))
			lcd.print(dyn.value(2), 3);
		return;
		}
	}

void chan() {
	lcd.locate(1, 1);
	lcd.clr(1);
	lcd.print("Chan ");
	lcd.print(channel.selection().c_str());
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
	dyn.callbackPage(updateDisplayDyn);
	dyn.callbackEncoder(encoder);
	dyn.alias("Intensity Mode", "Mode");
	dyn.alias("Intensity Macros", "Macros");
	dyn.alias("Intensity Macro Rate", "M Rate");
	dyn.alias("Intensity Macro Crossfade Rate", "M X Rate");
	dyn.alias("Position MSpeed", "Pos MSpeed");
	dyn.alias("Position Blink", "Pos Blink");
	dyn.alias("Color Select", "Select");
	dyn.alias("Color Mix Mode", "Mix Mode");
	dyn.alias("Gobo Index/Speed", "Gobo I/S");
	dyn.alias("Gobo Select", "Gobo Sel");
	dyn.alias("Gobo Index/Speed 2", "Gobo2 I/S");
	dyn.alias("Gobo Select 2", "Gobo2 Sel");
	dyn.alias("Beam Fx Index/Speed", "BeamFx I/S");
	dyn.alias("Beam Fx Select", "BeamFx Sel");
	dyn.alias("Beam Macros", "Macros");
	dyn.alias("Animation Select", "Anim Sel");
	dyn.alias("Effect Macros", "FX Macros");
	dyn.alias("Global MSpeed", "Glb MSpd");
	dyn.alias("Shutter Strobe", "Strobe");
	dyn.alias("Beam MSpeed", "Beam MSpd");
	dyn.alias("Edge Distance", "Edge Dist");
	dyn.alias("LED Engine Effect Rate", "LedFX Rate");
	dyn.alias("LED Engine Effect Level", "LedFX Lev");
	dyn.alias("Light Diffusion", "Diffusion");
	dyn.alias("Frame Thrust A", "Frame A");
	dyn.alias("Frame Thrust B", "Frame B");
	dyn.alias("Frame Thrust C", "Frame C");
	dyn.alias("Frame Thrust D", "Frame D");
	dyn.alias("Frame Angle A", "Angle A");
	dyn.alias("Frame Angle B", "Angle B");
	dyn.alias("Frame Angle C", "Angle C");
	dyn.alias("Frame Angle D", "Angle D");
	dyn.alias("Frame Assembly", "Assembly");
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
	dyn.update();
	}

void maintain() {
	channel.parse();
	dyn.parse();
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
	enc1.parameter(dyn.parameter(1));
	enc2.parameter(dyn.parameter(2));
	enc1Button.parameter(dyn.parameter(1));
	enc2Button.parameter(dyn.parameter(2));
	lcd.clr(2);
	lcd.locate(2, 1);
	lcd.print(dyn.categoryName().c_str());
	lcd.print(" ");
	lcd.print(dyn.page());
	lcd.print("/");
	lcd.print(dyn.pages());
	lcd.clr(3);
	lcd.locate(3, 1);
	lcd.printf("%.10s", dyn.alias(1).c_str());
	lcd.locate(3, 11);
	lcd.printf("%.10s", dyn.alias(2).c_str());
	lcd.clr(4);
	if (dyn.active(1))
		lcd.print(dyn.value(1), 3);
	else
		lcd.clp(4, 1, 10);
	lcd.locate(4, 11);
	if (dyn.active(2))
		lcd.print(dyn.value(2), 3);
	else
		lcd.clp(4, 11, 10);
	}
