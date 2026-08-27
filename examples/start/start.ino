/*
eOS3 library for USB and Ethernet UDP/TCP is placed under the MIT license
Copyright (c) 2025 Stefan Staub

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/**
 * @brief hardware sugestions
 * - Buttons and Encoders
 *   put a 100nF ceramic capacitor between GND and Pins of buttons and encoders (A/B).
 * 
 * - Faders are a linear type with 10kOhm, from Bourns or ALPS and can be 45/60/80/100mm long
 *   put 100nF ceramic capitors between ground and fader levelers to prevent analog noise
 * 
 * - Raspberry Pi Pico / Pico2
 *   use AGND pin instaed of the normal GND pins
 *   use ADC_REF instaed of the normal 3V3 pin
 * 
 * - TEENSY 3.x:
 *   use ANALOG GND instead of the normal GND
 */

#include "Arduino.h"
#include "eOS3.h"
// put the headers of other libraries you want to use here

// put all #defines here, e.g. for buttons ...

// put all variables here

// put all the class initialisers here
eOS3 eos;

void setup() {
	// put your setup code here, to run once

	// initialise connection
	eos.begin(); // for USB connection

	}

void loop() {
	// put update funktions for buttons and encoders here
	eos.update();

	}

void maintain() {
	/*
	maintain() is called when new OSC data comes in
	this function should not deleted to work the whole code properly!
	if you don't need it, leave it empty!
	put all your parsers and display handling here
	*/

	}

void connected() {
	/*
	connected() is called when a new connection is established
	this function should not deleted to work the whole code properly!
	if you don't need it, leave it empty!
	put all the init functions for fader and direct selects here,
	also filters and subscribtions
	*/

	}

void disconnected() {
	/*
	disconnected() is called when a connection failed
	this function should not deleted to work the whole code properly!
	if you don't need it, leave it empty!
	put all things here when the connection failed like splash screen
	*/

	}