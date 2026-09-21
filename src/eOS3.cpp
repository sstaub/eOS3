#include "eOS3.h"
#include <strings.h>

OSC osc;

const uint8_t END = 0xC0;
const uint8_t ESC = 0xDB;
const uint8_t ESC_END = 0xDC;
const uint8_t ESC_ESC = 0xDD;

#define BUTTON_PRESS         (int32_t)1
#define BUTTON_RELEASE       (int32_t)0

// callbacks
cbptr callbackConnect = connected;
cbptr callbackDisconnect = disconnected;
cbptr callbackReceived = maintain;

// connection time out handling
uint32_t lastMessageRxTime = 0;
bool timeoutPingSend = false;;
bool connectedToEos = false;
#define PING_AFTER_IDLE_MS    2500
#define TIMEOUT_AFTER_IDLE_MS 5000

// wheel collection time, needed cause of undeterministic approch of /eos/out/wheel/
#define COLLECT_DELAY 500

// shift, accelaration and 2nd/3rd global variables
bool shiftState = false;
bool accelerationState = false;
uint8_t intensTick = 8;
uint8_t accelerationTick = 8;
bool state2nd = false;
bool state3rd = false; 


/*******************************************************************************
 * The master class eOS3
*******************************************************************************/

eOS3::eOS3() {}

void eOS3::begin() {
	osc.begin();
	connectedToEos = true;
	}

void eOS3::begin(UDP &udp, IPAddress ip, uint16_t udpRxPort, uint16_t udpTxPort) {
	osc.begin(udp, ip, udpRxPort, udpTxPort);
	connectedToEos = true;
	callbackConnect();
	}

void eOS3::begin(Client &tcp, IPAddress ip, uint16_t tcpPort) {
	osc.begin(tcp, ip, tcpPort);
	connectedToEos = true;
	callbackConnect();
	}

void eOS3::reboot() {
	#if defined(PICO_RP2040) || defined(PICO_RP2350) || defined(PICO_RP2350A) || defined(PICO_RP2350B)
		rp2040.reboot();
	#endif

	#if defined (TEENSY_ARDUINO40) || defined(TEENSY_ARDUINO41) || defined(TEENSY_ARDUINO35) || defined(TEENSY_ARDUINO36)
		USB1_USBCMD = 0; // disconnect USB
		delay(50);       // enough time for USB hubs/ports to detect disconnect
		SCB_AIRCR = 0x05FA0004;
	#endif
}

void eOS3::update() {
	if (osc.receive()) { 
		callbackReceived();
		connectedToEos = true;
		timeoutPingSend = false;
		lastMessageRxTime = millis();
		}
	if (lastMessageRxTime > 0) {
		uint32_t diff = millis() - lastMessageRxTime;
		if (diff > TIMEOUT_AFTER_IDLE_MS) {
			connectedToEos = false;
			lastMessageRxTime = 0;
			timeoutPingSend = false;
			callbackDisconnect();
			}
		if (!timeoutPingSend && diff > PING_AFTER_IDLE_MS) {
			ping();
			timeoutPingSend = true;
			}
		}
	}

void eOS3::filter(string filter) {
	osc.message("/eos/filter/add", filter);
	}

void eOS3::filterRemove(string filter) {
	osc.message("/eos/filter/remove", filter);
	}

void eOS3::filterClear() {
	osc.message("/eos/filter/clear");
	}

void eOS3::subscription(subscribe_t subscribe) {
	osc.message("/eos/subscribe", (int32_t)subscribe);
	}

void eOS3::subscription(string parameter, subscribe_t subscribe) {
	osc.message("/eos/subscribe/param/" + parameter, (int32_t)subscribe);
	}

void eOS3::command(string cmd) {
	osc.message("/eos/cmd", cmd);
	}

void eOS3::newCommand(string cmd) {
	osc.message("/eos/newcmd", cmd);
	}

void eOS3::reset() {
	osc.message("/eos/reset");
	}

void eOS3::ping() {
	osc.message("/eos/ping");
	}

void eOS3::ping(int32_t number) {
	osc.message("/eos/ping", number);
	}

void eOS3::ping(string msg) {
	osc.message("/eos/ping", msg);
	}

void eOS3::user(int16_t userID) {
	osc.message("/eos/user", (int32_t)userID);
	}

void eOS3::initFaders(uint8_t faders, uint8_t index, uint8_t page) {
	osc.message(patternFader(faders, index, page));
	}

void eOS3::initDS(button_t type, uint8_t buttons, uint8_t index, uint16_t page, bool flexi) {
	osc.message(patternDS(type, buttons, index, page, flexi));
	}


/*******************************************************************************
 * Shift button class
*******************************************************************************/

Shift::Shift(uint8_t pin) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	}

Shift::Shift() {
	}

void Shift::tick(uint8_t tick) {
	intensTick = tick;
	}

void Shift::mode(buttonMode_t modus) {
	this->modus = modus;
	}

bool Shift::state() {
	return shiftState;
	}

void Shift::update() {
	if (modus == PUSH) {
		if (digitalRead(pin) != last) {
			if (last == false) {
				last = true;
				shiftState = false;
				}
			else {
				last = false;
				shiftState = true;
				}
			}
		}
	else if (modus == TOGGLE) {
		if (digitalRead(pin) != last) {
			if (last == false) last = true;
			else {
				last = false;
				shiftState = !shiftState;
				}
			}
		}
	}

void Shift::update(bool state) {
	if (modus == PUSH) {
		if (state != last) {
			if (last == false) {
				last = true;
				shiftState = true;
				}
			else {
				last = false;
				shiftState = false;
				}
			}
		}
	else if (modus == TOGGLE) {
		if (state != last) {
			if (last == false) {
				last = true;
				shiftState = !shiftState;
				}
			else last = false;
			}
		}
	}


/*******************************************************************************
 * Accelaration button class
*******************************************************************************/

Acceleration::Acceleration(uint8_t pin) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	}

Acceleration::Acceleration() {
	}

void Acceleration::tick(uint8_t tick) {
	accelerationTick = tick;
	}

void Acceleration::mode(buttonMode_t modus) {
	this->modus = modus;
	}

bool Acceleration::state() {
	return accelerationState;
	}

void Acceleration::update() {
	if (modus == PUSH) {
		if (digitalRead(pin) != last) {
			if (last == false) {
				last = true;
				accelerationState = false;
				}
			else {
				last = false;
				accelerationState = true;
				}
			}
		}
	else if (modus == TOGGLE) {
		if (digitalRead(pin) != last) {
			if (last == false) last = true;
			else {
				last = false;
				accelerationState = !accelerationState;
				}
			}
		}
	}

void Acceleration::update(bool state) {
	if (modus == PUSH) {
		if (state != last) {
			if (last == false) {
				last = true;
				accelerationState = true;
				}
			else {
				last = false;
				accelerationState = false;
				}
			}
		}
	else if (modus == TOGGLE) {
		if (state != last) {
			if (last == false) {
				last = true;
				accelerationState = !accelerationState;
				}
			else last = false;
			}
		}
	}

/*******************************************************************************
 * 2nd button control class
*******************************************************************************/

Control2nd::Control2nd(uint8_t pin) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	}

Control2nd::Control2nd() {
	}

void Control2nd::mode(buttonMode_t modus) {
	this->modus = modus;
	}

bool Control2nd::state() {
	return state2nd;
	}

void Control2nd::update() {
	if (modus == PUSH) {
		if (digitalRead(pin) != last) {
			if (last == false) {
				last = true;
				state2nd = false;
				}
			else {
				last = false;
				state2nd = true;
				}
			}
		}
	else if (modus == TOGGLE) {
		if (digitalRead(pin) != last) {
			if (last == false) last = true;
			else {
				last = false;
				state2nd = !state2nd;
				}
			}
		}
	}

void Control2nd::update(bool state) {
	if (modus == PUSH) {
		if (state != last) {
			if (last == false) {
				last = true;
				state2nd = true;
				}
			else {
				last = false;
				state2nd = false;
				}
			}
		}
	else if (modus == TOGGLE) {
		if (state != last) {
			if (last == false) {
				last = true;
				state2nd = !state2nd;
				}
			else last = false;
			}
		}
	}


/*******************************************************************************
 * 3rd button control class
*******************************************************************************/

Control3rd::Control3rd(uint8_t pin) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	}

Control3rd::Control3rd() {
	}

void Control3rd::mode(buttonMode_t modus) {
	this->modus = modus;
	}

bool Control3rd::state() {
	return state3rd;
	}

void Control3rd::update() {
	if (modus == PUSH) {
		if (digitalRead(pin) != last) {
			if (last == false) {
				last = true;
				state3rd = false;
				}
			else {
				last = false;
				state3rd = true;
				}
			}
		}
	else if (modus == TOGGLE) {
		if (digitalRead(pin) != last) {
			if (last == false) last = true;
			else {
				last = false;
				state3rd = !state3rd;
				}
			}
		}
	}

void Control3rd::update(bool state) {
	if (modus == PUSH) {
		if (state != last) {
			if (last == false) {
				last = true;
				state3rd = true;
				}
			else {
				last = false;
				state3rd = false;
				}
			}
		}
	else if (modus == TOGGLE) {
		if (state != last) {
			if (last == false) {
				last = true;
				state3rd = !state3rd;
				}
			else last = false;
			}
		}
	}


/*******************************************************************************
 * Button class
*******************************************************************************/

Button::Button(uint8_t pin, button_t type, string strng) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	pattern = patternTypeString(type, strng);
	//lastDebounceTime = millis();
	}

Button::Button(uint8_t pin, button_t type, uint16_t number) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	pattern = patternTypeNumber(type, number);
	//lastDebounceTime = millis();
	}

Button::Button(button_t type, string strng) {
	pattern = patternTypeString(type, strng);
	}

Button::Button(button_t type, uint16_t number) {
	pattern = patternTypeNumber(type, number);
	}

void Button::update() {
	if (state2nd || state3rd) return;
	if (digitalRead(pin) != last) {
		if (last == false) {
			last = true;
			}
		else {
			last = false;
			osc.message(pattern);
			}
		}
	}

void Button::update(bool state) {
	if (state2nd || state3rd) return;
	if (state != last) {
		if (last == false) {
			last = true;
			osc.message(pattern);
			}
		else last = false;
		}
	}


/*******************************************************************************
 * Button2nd class
*******************************************************************************/

Button2nd::Button2nd(uint8_t pin, button_t type, string strng) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	pattern = patternTypeString(type, strng);
	}

Button2nd::Button2nd(uint8_t pin, button_t type, uint16_t number) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	pattern = patternTypeNumber(type, number);
	}

Button2nd::Button2nd(button_t type, string strng) {
	pattern = patternTypeString(type, strng);
	}

Button2nd::Button2nd(button_t type, uint16_t number) {
	pattern = patternTypeNumber(type, number);
	}

void Button2nd::update() {
	if (digitalRead(pin) != last) {
		if (last == false) last = true;
		else {
			last = false;
			if (state2nd == true) osc.message(pattern);
			}
		}
	}

void Button2nd::update(bool state) {
	if (state != last) {
		if (last == false) {
			last = true;
			if (state2nd == true) osc.message(pattern);
			}
		else last = false;
		}
	}


/*******************************************************************************
 * Button3rd class
*******************************************************************************/

Button3rd::Button3rd(uint8_t pin, button_t type, string strng) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	pattern = patternTypeString(type, strng);
	}

Button3rd::Button3rd(uint8_t pin, button_t type, uint16_t number) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	pattern = patternTypeNumber(type, number);
	}

Button3rd::Button3rd(button_t type, string strng) {
	pattern = patternTypeString(type, strng);
	}

Button3rd::Button3rd(button_t type, uint16_t number) {
	pattern = patternTypeNumber(type, number);
	}

void Button3rd::update() {
	if (digitalRead(pin) != last) {
		if (last == false) last = true;
		else {
			last = false;
			if (state3rd == true) osc.message(pattern);
			}
		}
	}

void Button3rd::update(bool state) {
	if (state != last) {
		if (last == false) {
			last = true;
			if (state3rd == true) osc.message(pattern);
			}
		else last = false;
		}
	}


/*******************************************************************************
 * Wheel class
*******************************************************************************/

Wheel::Wheel(uint8_t pinA, uint8_t pinB, direction_t direction) {
	this->pinA = pinA;
	this->pinB = pinB;
	this->direction = direction;
	pinMode(pinA, INPUT_PULLUP);
	pinMode(pinB, INPUT_PULLUP);
	pinALast = digitalRead(pinA);
	}

Wheel::Wheel(direction_t direction) {
	this->direction = direction;
	}

void Wheel::callback(cbptr call) {
	this->call = call;
	}

void Wheel::wheelNumber(uint8_t wheel) {
	this->wheel = wheel;
	}

bool Wheel::parse() {
	if (osc.getPattern().find("/eos/out/active/wheel/") == 0) {
		uint16_t whl = stoi(osc.getPattern().substr(osc.getPattern().rfind('/') + 1));
		if (osc.getInt(2) == 0) { // category 0, no data
			if (whl == wheel) {
				param = "";
				val = 0.0f;
				act = false;
				if (call != nullptr) call();
				return true;
				}
			}
		if (wheel == whl) {
			param = osc.getString(1).substr(0, osc.getString(1).rfind('[') - 2);
			val = osc.getFloat(3);
			act = true;
			if (call != nullptr) call();
			return true;
			}
		}
	return false;
	}

float Wheel::value() {
	return val;
	}

bool Wheel::active() {
	return act;
	}

string Wheel::parameter() {
	return param;
	}

void Wheel::update() {
	if (wheel == 0) return;
	encoderMotion = 0;
	pinACurrent = digitalRead(pinA);	
	if ((pinALast) && (!pinACurrent)) {
		if (digitalRead(pinB)) encoderMotion = - 1;
		else encoderMotion = 1;
		if (direction == REVERSE) encoderMotion = -encoderMotion;
		}
	pinALast = pinACurrent;

	if (encoderMotion != 0) {
		string wheelMsg = "/eos/active/wheel/";
		if (accelerationState == true) encoderMotion *= accelerationTick;
		if (shiftState == true) {
			wheelMsg += "fine/" + to_string(wheel);
			}
		else {
			wheelMsg += "coarse/" + to_string(wheel);
			}
		osc.message(wheelMsg, encoderMotion);
		}
	}

void Wheel::update(bool stateA, bool stateB) {
	if (wheel == 0) return;
	encoderMotion = 0;
	pinACurrent = !stateA;	
	if ((pinALast) && (!pinACurrent)) {
		if (!stateB) encoderMotion = - 1;
		else encoderMotion = 1;
		if (direction == REVERSE) encoderMotion = -encoderMotion;
		}
	pinALast = pinACurrent;

	if (encoderMotion != 0) {
		string wheelMsg = "/eos/active/wheel/";
		if (accelerationState == true) encoderMotion *= accelerationTick;
		if (shiftState == true) {
			wheelMsg += "fine/" + to_string(wheel);
			}
		else {
			wheelMsg += "coarse/" + to_string(wheel);
			}
		osc.message(wheelMsg, encoderMotion);
		}
	}

void Wheel::update(int32_t motion) {
	if (wheel == 0) return;
	if (motion == motionLast) return;
	if (motion < motionLast) encoderMotion = -1;
	if (motion > motionLast) encoderMotion = 1;
	motionLast = motion;
	if (direction == REVERSE) encoderMotion = -encoderMotion;
	string wheelMsg = "/eos/active/wheel/";
	if (accelerationState == true) encoderMotion *= accelerationTick;
	if (shiftState == true) {
		wheelMsg += "fine/" + to_string(wheel);
		}
	else {
		wheelMsg += "coarse/" + to_string(wheel);
		}
	osc.message(wheelMsg, encoderMotion);
	}


/*******************************************************************************
 * Encoder class
*******************************************************************************/

Encoder::Encoder(uint8_t pinA, uint8_t pinB, direction_t direction) {
	this->pinA = pinA;
	this->pinB = pinB;
	this->direction = direction;
	pinMode(pinA, INPUT_PULLUP);
	pinMode(pinB, INPUT_PULLUP);
	}

Encoder::Encoder(direction_t direction) {
	this->direction = direction;
	}

void Encoder::callback(cbptr call) {
	this->call = call;
	}

void Encoder::parameter(string param) {
	this->param = param;
	int pos = 0;
	while (pos != -1) {
		pos = this->param.find('_', pos);
		if (pos >=0) this->param[pos] = ' ';
		}
	}

string Encoder::parameter() {
	return param;
	}

bool Encoder::parse() {
	if (osc.getPattern().find("/eos/out/active/wheel/") == 0) {
		uint16_t whl = stoi(osc.getPattern().substr(osc.getPattern().rfind('/') + 1));
		if (osc.getInt(2) == 0) { // category 0, no data
			if (whl == wheel) {
				wheel = 0;
				val = 0.0f;
				act = false;
				if (call != nullptr) call();
				return true;
				}
			}
		string parameter = osc.getString(1).substr(0, osc.getString(1).rfind('[') - 2);
		if (parameter != param && wheel == whl) {
			wheel = 0;
			val = 0.0f;
			act = false;
			if (call != nullptr) call();
			return true;
			}
		if (parameter == param) {
			val = osc.getFloat(3);
			act = true;
			wheel = whl;
			if (call != nullptr) call();
			return true;
			}
		}
	return false;
	}

float Encoder::value() {
	return val;
	}

bool Encoder::active() {
	return act;
	}

void Encoder::update() {
	if (param == "") return;
	encoderMotion = 0;
	currentA = digitalRead(pinA);	
	if ((lastA) && (!currentA)) {
		if (digitalRead(pinB)) encoderMotion = - 1;
		else encoderMotion = 1;
		if (direction == REVERSE) encoderMotion = -encoderMotion;
		}
	lastA = currentA;
	if (encoderMotion != 0) {
		string wheelMsg = "/eos/wheel";
		if (shiftState == true) {
			if (param == "Intens" || param == "intens") encoderMotion *= intensTick;
			else wheelMsg += "/fine";
			}
		else wheelMsg += "/coarse";
		if (accelerationState == true) encoderMotion *= accelerationTick;
		string param1 = param;
		int slash = param1.find('/');
		if (slash > 0) param1[slash] = '\\';
		wheelMsg += '/' + param1;
		osc.message(wheelMsg, encoderMotion);
		}
	}

void Encoder::update(bool stateA, bool stateB) {
	if (param == "") return;
	encoderMotion = 0;
	currentA = stateA;	
	if ((lastA) && (!currentA)) {
		if (stateB) encoderMotion = -1;
		else encoderMotion = 1;
		if (direction == REVERSE) encoderMotion = -encoderMotion;
		}
	lastA = currentA;
	if (encoderMotion != 0) {
		string wheelMsg = "/eos/wheel";
		if (shiftState == true) {
			if (param == "Intens" || param == "intens") encoderMotion *= intensTick;
			else wheelMsg += "/fine";
			}
		else wheelMsg += "/coarse";
		if (accelerationState == true) encoderMotion *= accelerationTick;
		int slash = param.find('/');
		string param1 = param;
		if (slash > 0) param1[slash] = '\\';
		wheelMsg += '/' + param1;
		osc.message(wheelMsg, encoderMotion);
		}
	}

void Encoder::update(int32_t motion) {
	if (param == "") return;
	if (motion == motionLast) return;
	if (motion < motionLast) encoderMotion = -1;
	if (motion > motionLast) encoderMotion = 1;
	motionLast = motion;
	if (direction == REVERSE) encoderMotion = -encoderMotion;
	string wheelMsg = "/eos/wheel";
	if (shiftState == true) {
		if (param == "Intens" || param == "intens") encoderMotion *= intensTick;
		else wheelMsg += "/fine";
		}
	else wheelMsg += "/coarse";
	if (accelerationState == true) encoderMotion *= accelerationTick;
	int slash = param.find('/');
	string param1 = param;
	if (slash > 0) param1[slash] = '\\';
	wheelMsg += '/' + param1;
	osc.message(wheelMsg, encoderMotion);
	}


/*******************************************************************************
 * Absolute levels class
*******************************************************************************/

AbsoluteLevels::AbsoluteLevels(uint8_t pin, levels_t function, levels_t function2nd) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	this->function = function;
	this->function2nd = function2nd;
	}

AbsoluteLevels::AbsoluteLevels(levels_t function, levels_t function2nd) {
	this->function = function;
	this->function2nd = function2nd;
	}

void AbsoluteLevels::parameter(string param) {
	this->param = param;
	int pos = 0;
	while (pos != -1) {
		pos = this->param.find(' ', pos);
		if (pos >=0) this->param[pos] = '_';
		}
	if (function != PARAMETER) {
		string param1 = this->param;
		int slash = param1.find('/');
		if (slash > 0) param1[slash] = '\\';
		patternFunction = patternAbsolute(function, param1);
		}
	else patternFunction = patternAbsolute(function, this->param);
	if (function2nd != PARAMETER) {
		string param1 = this->param;
		int slash = param1.find('/');
		if (slash > 0) param1[slash] = '\\';
		patternFunction2nd = patternAbsolute(function2nd, param1);
		}
	else patternFunction2nd = patternAbsolute(function2nd, this->param);
	}

void AbsoluteLevels::update() {
	if (state2nd || state3rd) return;
	if (digitalRead(pin) != last) {
		if (last == false) last = true;
		else {
			last = false;
			if (shiftState == true) {
				if (function2nd == PARAMETER) osc.message("/eos/cmd", param);
				else if (function2nd) osc.message(patternFunction2nd);
				}
			else {
				if (function == PARAMETER) osc.message("/eos/cmd", param);
				else if (function) osc.message(patternFunction);
				}
			}
		}
	}

void AbsoluteLevels::update(bool state) {
	if (state2nd || state3rd) return;
	if (state != last) {
		if (last == false) {
			last = true;
			if (shiftState == true) {
				if (function2nd == PARAMETER) osc.message("/eos/cmd", param);
				else if (function2nd) osc.message(patternFunction2nd);
				}
			else {
				if (function == PARAMETER) osc.message("/eos/cmd", param);
				else if (function) osc.message(patternFunction);
				}
			}
		else last = false;
		}
	}


/*******************************************************************************
 * Direct select class
*******************************************************************************/

DS::DS(uint8_t pin, uint8_t number, uint8_t index) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	this->number = number;
	this->index = index;
	pattern = "/eos/ds/" + to_string(index) + '/' + to_string(number);
	}

DS::DS(uint8_t number, uint8_t index) {
	this->number = number;
	this->index = index;
	pattern = "/eos/ds/" + to_string(index) + '/' + to_string(number);
	}

void DS::update() {
	if (state2nd || state3rd) return;
	if (digitalRead(pin) != last) {
		if (last == false) last = true;
		else {
			last = false;
			osc.message(pattern);
			}
		}
	}

void DS::update(bool state) {
	if (state2nd || state3rd) return;
	if (state != last) {
		if (last == false) {
			last = true;
			osc.message(pattern);
			}
		else last = false;
		}
	}


/*******************************************************************************
 * Direct select handle class
*******************************************************************************/

DSTool::DSTool(uint8_t pinUp, uint8_t pinDown) {
	this->pinUp = pinUp;
	pinMode(pinUp, INPUT_PULLUP);
	pinUpLast = digitalRead(pinUp);
	this->pinDown = pinDown;
	pinMode(pinDown, INPUT_PULLUP);
	pinDownLast = digitalRead(pinDown);
	}

DSTool::DSTool() {
	}

void DSTool::flexiButton(uint8_t pinFlexi) {
	this->pinFlexi = pinFlexi;
	pinMode(pinFlexi, INPUT_PULLUP);
	pinFlexiLast = digitalRead(pinFlexi);
	}

void DSTool::flexiButton() {}

void DSTool::init(button_t type, uint8_t buttons, uint8_t index) {
	this->type = type;
	this->buttons = buttons;
	this->index = index;
	dsData.resize(buttons);
	patternUp = "/eos/ds/" + to_string(index) + "/page/1";
	patternDown = "/eos/ds/" + to_string(index) + "/page/-1";
	patternSearchButton = "/eos/out/ds/" + to_string(index) + '/';
	patternSearchPage = "/eos/out/ds/" + to_string(index);
	osc.message(patternDS(type, buttons, index, currentPageLast[type], flexiStateLast[type]));
	}

void DSTool::typeDS(button_t type) {
	this->type = type;
	osc.message(patternDS(type, buttons, index, currentPageLast[type], flexiStateLast[type]));
	}

int8_t DSTool::parse() {
	if (osc.getPattern().compare(patternSearchPage) == 0) {
		int firstSpace = osc.getString(1).find(' ');
		int bracketOpen = osc.getString(1).find('[');
		int bracketClosed = osc.getString(1).find(']');
		int flexi = osc.getString(1).find("flexi");
		typeName = osc.getString(1).substr(0, firstSpace - 1);
		if(flexi != -1) {
			flexiState = true;
			currentPage = stoi(osc.getString(1).substr(bracketOpen + 1, flexi - bracketOpen - 3));
			}
		else {
			flexiState = false;
			currentPage = stoi(osc.getString(1).substr(bracketOpen + 1, bracketClosed - bracketOpen - 1));
			}
		currentPageLast[type] = currentPage;
		flexiStateLast[type] = flexiState;
		if (callPage != nullptr) callPage(currentPage);
		return -1;
		}
	
	if (osc.getPattern().find(patternSearchButton) == 0) {
		uint8_t ds = stoi(osc.getPattern().substr(osc.getPattern().rfind("/") + 1));
		int lf = osc.getString(1).find("||");
		int bracketOpen2 = osc.getString(1).find('[');
		string label = osc.getString(1).substr(0, bracketOpen2 - 1);
		if (lf != -1) {
			label.replace(lf, 2, " ");
			}
		dsData[ds - 1].label = label;
		dsData[ds - 1].number = stoi(osc.getString(2));
		if (callData != nullptr) callData(ds);
		return ds;
		}
	return 0;
	}

void DSTool::callbackPage(cbptr2 call) {
	callPage = call;
	}

void DSTool::callbackData(cbptr2 call) {
	callData = call;
	}

string DSTool::label(uint8_t number) {
	if (number <= dsData.size())
		return dsData[number - 1].label;
	return "";
	}

uint16_t DSTool::number(uint8_t number) {
	if (number <= dsData.size())
		return dsData[number - 1].number;
	return 0;
	}

string DSTool::typeDS() {
	return typeName;
	}

uint16_t DSTool::page() {
	return currentPage;
	}

bool DSTool::flexi() {
	return flexiState;
	}

void DSTool::updateButtons() {
	if (state2nd || state3rd) return;
	if (digitalRead(pinUp) != pinUpLast) {
		if (pinUpLast == false) pinUpLast = true;
		else {
			pinUpLast = false;
			osc.message(patternUp);
			}
		return;
		}

	if (digitalRead(pinDown) != pinDownLast) {
		if (pinDownLast == false) pinDownLast = true;
		else {
			pinDownLast = false;
			osc.message(patternDown);
			}
		return;
		}
	}

void DSTool::updateButtons(bool stateUp, bool stateDown) {
	if (state2nd || state3rd) return;
	if (stateUp != pinUpLast) {
		if (pinUpLast == false) {
			pinUpLast = true;
			osc.message(patternUp);
			}
		else pinUpLast = false;
		return;
		}

	if (stateDown != pinDownLast) {
		if (pinDownLast == false) {
			pinDownLast = true;
			osc.message(patternDown);
			}
		else pinDownLast = false;
		return;
		}
	}

void DSTool::updateFlexi() {
	if (digitalRead(pinFlexi) != pinFlexiLast) {
		if (pinFlexiLast == false) pinFlexiLast = true;
		else {
			pinFlexiLast = false;
			flexiState = !flexiState;
			currentPage = 1;
			osc.message(patternDS(type, buttons, index, currentPage, flexiState));
			}
		return;
		}
	}

void DSTool::updateFlexi(bool stateFlexi) {
	if (stateFlexi != pinFlexiLast) {
		if (pinFlexiLast == false) {
			pinFlexiLast = true;
			flexiState = !flexiState;
			currentPage = 1;
			osc.message(patternDS(type, buttons, index, 1, flexiState));
			}
		else pinFlexiLast = false;
	return;
		}
	}


/*******************************************************************************
 * DS Type button
*******************************************************************************/

ButtonDSType::ButtonDSType(DSTool &dsTool, uint8_t pin, button_t type) {
	this->dsTool = &dsTool;
	this->pin = pin;
	this->type = type;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	}

ButtonDSType::ButtonDSType(DSTool &dsTool, button_t type) {
	this->dsTool = &dsTool;
	this->type = type;
	}

void ButtonDSType::update() {
	if (state2nd || state3rd) return;
	if (digitalRead(pin) != last) {
		if (last == false) {
			last = true;
			}
		else {
			last = false;
			dsTool->typeDS(type);
			}
		}
	}

void ButtonDSType::update(bool state) {
	if (state2nd || state3rd) return;
	if (state != last) {
		if (last == false) {
			last = true;
			dsTool->typeDS(type);
			}
		else last = false;
		}
	}


/*******************************************************************************
 * Submaster class
*******************************************************************************/

Submaster::Submaster(uint8_t analogPin, uint16_t sub) {
	this->analogPin = analogPin;
	this->sub = sub;
	updateTime = millis();
	patternSub = "/eos/sub/" + to_string(sub);
	}

Submaster::Submaster(uint16_t sub) {
	this->sub = sub;
	updateTime = millis();
	patternSub = "/eos/sub/" + to_string(sub);
	}

uint8_t Submaster::value() {
	return val;
	}

void Submaster::fireButton(uint8_t firePin) {
	this->firePin = firePin;
	pinMode(firePin, INPUT_PULLUP);
	fireLast = digitalRead(firePin);
	patternFire = "/eos/sub/" + to_string(sub) + "/fire";
	}

void Submaster::fireButton() {
	patternFire = "/eos/sub/" + to_string(sub) + "/fire";
	}

void Submaster::callback(cbptr call) {
	this->call = call;
	}

void Submaster::updateAnalog() {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		int raw = analogRead(analogPin);
		if (raw < (analogLast - FADER_THRESHOLD) || raw > (analogLast + FADER_THRESHOLD)) {
			analogLast = raw;
			val = map(analogLast, FADER_THRESHOLD, 1023 - FADER_THRESHOLD, 0, 100);
			if (valLast != val) {
				valLast = val;
				osc.message(patternSub, (val / 100.0f));
				if (call != nullptr) call();
				}
			}
		updateTime = millis();
		}
	}

void Submaster::updateAnalog(int analog) {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		int raw = analog;
		if (raw < (analogLast - FADER_THRESHOLD) || raw > (analogLast + FADER_THRESHOLD)) {
			analogLast = raw;
			val = map(analogLast, FADER_THRESHOLD, 1023 - FADER_THRESHOLD, 0, 100);
			if (valLast != val) {
				valLast = val;
				osc.message(patternSub, (val / 100.0f));
				if (call != nullptr) call();
				}
			}
		updateTime = millis();
		}
	}

void Submaster::updateValue(uint8_t value) {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		osc.message(patternSub, (value / 100.0f));
		if (call != nullptr) call();
		updateTime = millis();
		}
	}

void Submaster::updateFire() {
	if (digitalRead(firePin) != fireLast) {
		if (fireLast == false) {
			fireLast = true;
			osc.message(patternFire, BUTTON_RELEASE);
			}
		else {
			fireLast = false;
			osc.message(patternFire, BUTTON_PRESS);
			}
		}
	}

void Submaster::updateFire(bool fireState) {
	if(fireState != fireLast) {
		if (fireLast == LOW) {
			fireLast = HIGH;
			osc.message(patternFire, BUTTON_PRESS);
			}
		else {
			fireLast = LOW;
			osc.message(patternFire, BUTTON_RELEASE);
			}
		}
	}


/*******************************************************************************
 * Fader class
*******************************************************************************/

Fader::Fader(uint8_t analogPin, uint8_t fader, uint8_t index) {
	this->analogPin = analogPin;
	this->fader = fader;
	this->index = index;
	updateTime = millis();
	patternFader = "/eos/fader/" + to_string(index) + "/" + to_string(fader);
	}

Fader::Fader(uint8_t fader, uint8_t index) {
	this->fader = fader;
	this->index = index;
	updateTime = millis();
	patternFader = "/eos/fader/" + to_string(index) + "/" + to_string(fader);
	}

void Fader::fireButton(uint8_t firePin) {
	this->firePin = firePin;
	pinMode(firePin, INPUT_PULLUP);
	fireLast = digitalRead(firePin);
	patternFire = "/eos/fader/" + to_string(index) + "/" + to_string(fader) + "/fire";
	}

void Fader::fireButton() {
	patternFire = "/eos/fader/" + to_string(index) + "/" + to_string(fader) + "/fire";
	}

void Fader::stopButton(uint8_t stopPin) {
	this->stopPin = stopPin;
	pinMode(stopPin, INPUT_PULLUP);
	stopLast = digitalRead(stopPin);
	patternStop = "/eos/fader/" + to_string(index) + "/" + to_string(fader) + "/stop";
	}

void Fader::stopButton() {
	patternStop = "/eos/fader/" + to_string(index) + "/" + to_string(fader) + "/stop";
	}

void Fader::loadButton(uint8_t loadPin) {
	this->loadPin = loadPin;
	pinMode(loadPin, INPUT_PULLUP);
	loadLast = digitalRead(loadPin);
	patternLoad = "/eos/fader/" + to_string(index) + "/" + to_string(fader) + "/load";
	}

void Fader::loadButton() {
	patternLoad = "/eos/fader/" + to_string(index) + "/" + to_string(fader) + "/load";
	}

void Fader::callback(cbptr call) {
	this->call = call;
	}

uint8_t Fader::value() {
	return val;
	}

void Fader::fetch(uint8_t fetchValue) {
	this->fetchValue = fetchValue;
	lockState = true;
	}

void Fader::jitter(uint8_t delta) {
	this->delta = delta;
	}

bool Fader::lock() {
	return lockState;
	}

void Fader::lock(bool state) {
	lockState = state;
	}

void Fader::updateAnalog() {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		int raw = analogRead(analogPin);
		if (raw < (analogLast - FADER_THRESHOLD) || raw > (analogLast + FADER_THRESHOLD)) {
			analogLast = raw;
			val = map(analogLast, FADER_THRESHOLD, 1023 - FADER_THRESHOLD, 0, 100);
			if (valLast != val) {
				valLast = val;
				if (lockState == true) {
					if ((valLast <= fetchValue + delta) && (valLast >= fetchValue - delta)) lockState = false; 
					}
				if (lockState == false) {
					osc.message(patternFader, (val / 100.0f));
					if (call != nullptr) call();
					}
				}
			}
		updateTime = millis();
		}
	}

void Fader::updateAnalog(int value) {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		int raw = value;
		if (raw < (analogLast - FADER_THRESHOLD) || raw > (analogLast + FADER_THRESHOLD)) {
			analogLast = raw;
			val = map(analogLast, FADER_THRESHOLD, 1023 - FADER_THRESHOLD, 0, 100);
			if (valLast != val) {
				valLast = val;
				if (lockState == true) {
					if ((valLast <= fetchValue + delta) && (valLast >= fetchValue - delta)) lockState = false; 
					}
				if (lockState == false) {
					osc.message(patternFader, (val / 100.0f));
					if (call != nullptr) call();
					}
				}
			}
		updateTime = millis();
		}
	}

void Fader::updateValue(uint8_t value) {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		if (lockState == true) {
			if ((valLast <= fetchValue + delta) && (valLast >= fetchValue - delta)) lockState = false; 
			}
		if (lockState == false) {
			osc.message(patternFader, (value / 100.0f));
			if (call != nullptr) call();
			}
		updateTime = millis();
		}
	}

void Fader::updateFire() {
	if (digitalRead(firePin) != fireLast) {
		if (fireLast == false) {
			fireLast = true;
			osc.message(patternFire, BUTTON_RELEASE);
			}
		else {
			fireLast = false;
			osc.message(patternFire, BUTTON_PRESS);
			}
		}
	}

void Fader::updateFire(bool fireState) {
	if(fireState != fireLast) {
		if (fireLast == LOW) {
			fireLast = HIGH;
			osc.message(patternFire, BUTTON_PRESS);
			}
		else {
			fireLast = LOW;
			osc.message(patternFire, BUTTON_RELEASE);
			}
		}
	}

void Fader::updateStop() {
	if (digitalRead(stopPin) != stopLast) {
		if (stopLast == false) {
			stopLast = true;
			osc.message(patternStop, BUTTON_RELEASE);
			}
		else {
			stopLast = false;
			osc.message(patternStop, BUTTON_PRESS);
			}
		}
	}

void Fader::updateStop(bool stopState) {
	if(stopState != fireLast) {
		if (stopLast == LOW) {
			stopLast = HIGH;
			osc.message(patternStop, BUTTON_PRESS);
			}
		else {
			stopLast = LOW;
			osc.message(patternStop, BUTTON_RELEASE);
			}
		}
	}

void Fader::updateLoad() {
	if (digitalRead(loadPin) != loadLast) {
		if (loadLast == false) {
			loadLast = true;
			osc.message(patternLoad, BUTTON_RELEASE);
			}
		else {
			fireLast = false;
			osc.message(patternLoad, BUTTON_PRESS);
			}
		}
	}

void Fader::updateLoad(bool loadState) {
	if(loadState != fireLast) {
		if (loadLast == LOW) {
			loadLast = HIGH;
			osc.message(patternLoad, BUTTON_PRESS);
			}
		else {
			loadLast = LOW;
			osc.message(patternLoad, BUTTON_RELEASE);
			}
		}
	}


/*******************************************************************************
 * Fader handling class
*******************************************************************************/

FaderTool::FaderTool(uint8_t pinUp, uint8_t pinDown) {
	this->pinUp = pinUp;
	pinMode(pinUp, INPUT_PULLUP);
	pinUpLast = digitalRead(pinUp);
	this->pinDown = pinDown;
	pinMode(pinDown, INPUT_PULLUP);
	pinDownLast = digitalRead(pinDown);
	}

FaderTool::FaderTool() {}

void FaderTool::init(uint8_t faders, uint8_t index) {
	this->faders = faders;
	this->index = index;
	faderData.resize(faders);
	osc.message(patternFader(10, index, currentPage));
	patternUp = "/eos/fader/" + to_string(index) + "/page/1";
	patternDown = "/eos/fader/" + to_string(index) + "/page/-1";
	patternSearchName = "/eos/out/fader/" + to_string(index) + '/';
	patternSearchRange = "/eos/out/fader/range/" + to_string(index) + '/';
	patternSearchPage = "/eos/out/fader/" + to_string(index);
	patternSearchValue = "/eos/fader/" + to_string(index) + '/';
	}

int8_t FaderTool::parse() {
	if (osc.getPattern().compare(patternSearchPage) == 0) {
		currentPage = stoi(osc.getString(1));
		if (callPage != nullptr) callPage(currentPage);
		return -1;
		}

	if (osc.getPattern().find(patternSearchRange) == 0) {
		uint8_t fader = stoi(osc.getPattern().substr(osc.getPattern().rfind("/") + 1));
		faderData[fader - 1].min = osc.getInt(1);
		faderData[fader - 1].max = osc.getInt(2);
		if (callRange != nullptr) callRange(fader);
		return fader;
		}

	if (osc.getPattern().find(patternSearchName) == 0) {
		int lastSlash = osc.getPattern().rfind('/');
		int secondLastSlash = osc.getPattern().rfind('/', lastSlash - 1);
		uint8_t fader = stoi(osc.getPattern().substr(secondLastSlash + 1, lastSlash - secondLastSlash));
		string label = osc.getString(1);
		if(label == "") faderData[fader].faderType = UNMAPPED;
		else if (label.find("S") == 0) faderData[fader].faderType = SUB;
		else if (label.find("GM") == 0) faderData[fader].faderType = GM;
		else if (label.find("L") == 0) {
			faderData[fader].faderType = QL;
			int lf = label.find("\x0A"); // [EOS-57604]
			if (lf > 0) label[lf] = ' ';
			}
		else if (label.find("Pr") == 0) faderData[fader].faderType = PR;
		else if (label.find("IP") == 0) faderData[fader].faderType = IPR;
		else if (label.find("FP") == 0) faderData[fader].faderType = FPR;
		else if (label.find("CP") == 0) faderData[fader].faderType = CPR;
		else if (label.find("BP") == 0) faderData[fader].faderType = BPR;
		else if (label.find("Global") == 0) faderData[fader].faderType = GFX;
		else if (label.find("Man") == 0) faderData[fader].faderType = MANUAL;
		faderData[fader - 1].label = label;
		if (callName != nullptr) callName(fader);
		return fader;
		}
	if (osc.getPattern().find(patternSearchValue) == 0) {
		uint8_t fader = stoi(osc.getPattern().substr(osc.getPattern().rfind("/") + 1));
		faderData[fader - 1].value = (uint8_t)(osc.getFloat(1) * 100);
		if (callValue != nullptr) callValue(fader);
		return fader;
		}
	return 0;
	}

void FaderTool::callbackPage(cbptr2 call) {
	callPage = call;
	}

void FaderTool::callbackName(cbptr2 call) {
	callName = call;
	}

void FaderTool::callbackRange(cbptr2 call) {
	callRange = call;
	}

void FaderTool::callbackValue(cbptr2 call) {
	callValue = call;
	}

uint8_t FaderTool::page() {
	return currentPage;
	}

uint8_t FaderTool::value(uint8_t fader) {
	if (fader <= faderData.size())
		return faderData[fader - 1].value;
	return 0;
	}

string FaderTool::label(uint8_t fader) {
	if (fader <= faderData.size())
		return faderData[fader - 1].label;
	return "";
	}

uint16_t FaderTool::rangeMin(uint8_t fader) {
	if (fader <= faderData.size())
		return faderData[fader - 1].min;
	return 0;
	}

uint16_t FaderTool::rangeMax(uint8_t fader) {
	if (fader <= faderData.size())
		return faderData[fader - 1].max;
	return 0;
	}

fader_t FaderTool::type(uint8_t fader) {
	if (fader <= faderData.size())
		return faderData[fader].faderType;
	return (fader_t)0;
	}

string FaderTool::typeString(uint8_t fader) {
	if (fader <= faderData.size()) {
		switch (faderData[fader].faderType) {
			case UNMAPPED: {
				return "Unmapped";
				break;
				}
			case SUB: {
				return "Submaster";
				break;
				}
			case GM: {
				return "Grand Master";
				break;
				}
			case QL: {
				return "Cue List";
				break;
				}
			case PR: {
				return "Preset";
				break;
				}
			case IPR: {
				return "Intensity Preset";
				break;
				}
			case FPR: {
				return "Focus Preset";
				break;
				}
			case CPR: {
				return "Color Preset";
				break;
				}
			case BPR: {
				return "Beam Preset";
				break;
				}
			case GFX: {
				return "Global FX";
				break;
				}
			case MANUAL: {
				return "Man Time";
				break;
				}
			default: {}
			}
		}
	return "";
	}

void FaderTool::update() {
	if (state2nd || state3rd) return;
	if (digitalRead(pinUp) != pinUpLast) {
		if (pinUpLast == false) {
			pinUpLast = true;
			}
		else {
			pinUpLast = false;
			osc.message(patternUp);
			}
		return;
		}

	if (digitalRead(pinDown) != pinDownLast) {
		if (pinDownLast == false) {
			pinDownLast = true;
			}
		else {
			pinDownLast = false;
			osc.message(patternDown);
			}
		return;
		}
	}

void FaderTool::update(bool stateUp, bool stateDown) {
	if (state2nd || state3rd) return;
	if (stateUp != pinUpLast) {
		if (pinUpLast == false) {
			pinUpLast = true;
			osc.message(patternUp);
			}
		else {
			pinUpLast = false;
			}
		return;
		}

	if (stateDown != pinDownLast) {
		if (pinDownLast == false) {
			pinDownLast = true;
			osc.message(patternDown);
			}
		else {
			pinDownLast = false;
			}
		return;
		}
	}


/*******************************************************************************
 * Parameter list handling class
*******************************************************************************/

SelectParameter::SelectParameter(uint8_t pinUp, uint8_t pinDown, uint8_t encoders) {
	this->pinUp = pinUp;
	pinMode(pinUp, INPUT_PULLUP);
	pinUpLast = digitalRead(pinUp);
	this->pinDown = pinDown;
	pinMode(pinDown, INPUT_PULLUP);
	pinDownLast = digitalRead(pinDown);
	this->encoders = encoders;
	currentPage = 1;
	pagesCount = 1;
	idx = new int [encoders];
	for (uint8_t i = 0; i < encoders; i++) idx[i] = i;
	}

SelectParameter::SelectParameter(uint8_t encoders) {
	this->encoders = encoders;
	currentPage = 1;
	pagesCount = 1;
	idx = new int [encoders];
	for (uint8_t i = 0; i < encoders; i++) idx[i] = i;
	}

void SelectParameter::callbackPage(cbptr call) {
	this->callPage = call;
	}

void SelectParameter::callbackEncoder(cbptr2 call) {
	this->callEncoder= call;
	}

void SelectParameter::parameter(string parameter, string alias) {
	Wheel add;
	add.parameter = parameter;
	add.alias = alias;
	add.active = false;
	add.value = 0.0f;
	add.wheel = 0;
	param.push_back(add);
	parameterCount = param.size();
	if (parameterCount > encoders) {
		pagesCount = parameterCount / encoders;
		if (parameterCount % encoders) pagesCount++;
		}
	}

uint8_t SelectParameter::parse() {
	if (osc.getPattern().find("/eos/out/active/wheel/") == 0) {
		uint16_t whl = stoi(osc.getPattern().substr(osc.getPattern().rfind('/') + 1));
		if(osc.getInt(2) == 0) {
			for (int i = 0; i < parameterCount; i++) {
				if (param[i].wheel == whl) {
					param[i].value = 0.0f;
					param[i].wheel = 0;
					param[i].active = false;
					for (uint8_t j = 0; j < encoders; j++) {
						if (i == idx[j]) {
							if (callEncoder != nullptr) callEncoder(j + 1);
							return j + 1;
							}
						}
					}
				}
			return 0;
			}
		string parameter = osc.getString(1).substr(0, osc.getString(1).rfind('[') - 2);
		for (int i = 0; i < parameterCount; i++) {
			if (parameter != param[i].parameter && param[i].wheel == whl) {
				param[i].value = 0.0f;
				param[i].wheel = 0;
				param[i].active = false;
				for (uint8_t j = 0; j < encoders; j++) {
					if (i == idx[j]) {
						if (callEncoder != nullptr) callEncoder(j + 1);
						return j + 1;
						}
					}
				}
			if (param[i].parameter == parameter) {
				param[i].value = osc.getFloat(3);
				param[i].wheel = whl;
				param[i].active = true;
				for (uint8_t j = 0; j < encoders; j++) {
					if (i == idx[j]) {
						if (callEncoder != nullptr) callEncoder(j + 1);
						return j + 1;
						}
					}
				}
			}
		}
	return 0;
	}

string SelectParameter::parameter(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return "";
	if (encoder > 0 && encoder <= encoders) {
		return param[idx[encoder - 1]].parameter;
		}
	return "";
	}

string SelectParameter::alias(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return "";
	if (encoder > 0 && encoder <= encoders) {
		if (param[idx[encoder - 1]].alias.size())
			return param[idx[encoder - 1]].alias;
		else
			return param[idx[encoder - 1]].parameter;
		}
	return "";
	}

float SelectParameter::value(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return 0.0f;
	if (encoder > 0 && encoder <= encoders) {
		return param[idx[encoder - 1]].value;
		}
	return 0.0f;
	}

bool SelectParameter::active(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return false;
	if (encoder > 0 && encoder <= encoders) {
		return param[idx[encoder - 1]].active;
		}
	return false;
	}

int SelectParameter::wheel(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return 0;
	if (encoder > 0 && encoder <= encoders) {
		return param[idx[encoder - 1]].wheel;
		}
	return 0;
	}

uint8_t SelectParameter::pages() {
	return pagesCount;
	}

uint8_t SelectParameter::page() {
	return currentPage;
	}

void SelectParameter::indexEncoder() {
	for (uint8_t i = 0; i < encoders; i++) {
		idx[i] = ((currentPage - 1) * encoders) + i;
		if (idx[i] >= parameterCount) idx[i] = -1;
		}
	if (callPage != nullptr) callPage();
	}

void SelectParameter::update() {
	if (state2nd || state3rd) return;
	if (digitalRead(pinUp) != pinUpLast) {
		if (pinUpLast == false) {
			pinUpLast = true;
			}
		else {
			pinUpLast = false;
			if (currentPage < pagesCount) {
				currentPage++;
				}
			else {
				currentPage = 1;
				}
			indexEncoder();
			}
		return;
		}

	if (digitalRead(pinDown) != pinDownLast) {
		if (pinDownLast == false) {
			pinDownLast = true;
			}
		else {
			pinDownLast = false;
			if (currentPage > 1) {
				currentPage--;
				}
			else {
				currentPage = pagesCount;
				}
			indexEncoder();
			}
		return;
		}
	}

void SelectParameter::update(bool stateUp, bool stateDown) {
	if (state2nd || state3rd) return;
	if (stateUp != pinUpLast) {
		if (pinUpLast == false) {
			pinUpLast = true;
			if (currentPage < pagesCount) {
				currentPage++;
				}
			else {
				currentPage = 1;
				}
			indexEncoder();
			}
		else {
			pinUpLast = false;
			}
		return;
		}

	if (stateDown != pinDownLast) {
		if (pinDownLast == false) {
			pinDownLast = true;
			if (currentPage > 1) {
				currentPage--;
				}
			else {
				currentPage = pagesCount;
				}
			indexEncoder();
			}
		else {
			pinDownLast = false;
			}
		return;
		}
	}


/*******************************************************************************
 * Parameter category list handling class
*******************************************************************************/

SelectCategory::SelectCategory(uint8_t pinIntens, uint8_t pinFocus, uint8_t pinColor, uint8_t pinImage, uint8_t pinForm, uint8_t pinShutter, uint8_t encoders) {
	pins[0] = pinIntens;
	pins[1] = pinFocus;
	pins[2] = pinColor;
	pins[3] = pinImage;
	pins[4] = pinForm;
	pins[5] = pinShutter;
	for (uint8_t i = 0; i < 6; i++) {
		pinMode(pins[i], INPUT_PULLUP);
		pinsLast[i] = digitalRead(pins[i]);
		}
	this->encoders = encoders;
	idx = new int [encoders];
	for (uint8_t i = 0; i < encoders; i++) idx[i] = -1;
	categoryData[INTENSITY].currentPage = 1;
	}

SelectCategory::SelectCategory(uint8_t encoders) {
	this->encoders = encoders;
	idx = new int [encoders];
	for (uint8_t i = 0; i < encoders; i++) idx[i] = -1;
	categoryData[INTENSITY].currentPage = 1;
	}

void SelectCategory::callbackPage(cbptr call) {
	this->callPage = call;
	}

void SelectCategory::callbackEncoder(cbptr2 call) {
	this->callEncoder = call;
	}

void SelectCategory::parameter(category_t category, string parameter, string alias) {
	Wheel add;
	add.parameter = parameter;
	add.alias = alias;
	add.active = false;
	add.value = 0.0f;
	add.wheel = 0;
	param[category].push_back(add);
	categoryData[category].parameters = param[category].size();
	categoryData[category].currentPage = 1;
	categoryData[category].pages = 1;
	if (category == INTENSITY) { // for idx for startup
		if (param[INTENSITY].size() && param[INTENSITY].size() <= encoders) {
			idx[param[INTENSITY].size() - 1] = param[INTENSITY].size() - 1;
			}
		}
	if (categoryData[category].parameters > encoders) {
		categoryData[category].pages = categoryData[category].parameters / encoders;
		if (categoryData[category].parameters % encoders) categoryData[category].pages++;
		}
	}

uint8_t SelectCategory::parse() {
	if (osc.getPattern().find("/eos/out/active/wheel/") == 0) {
		uint16_t wheel = stoi(osc.getPattern().substr(osc.getPattern().rfind('/') + 1));
		string parameter = osc.getString(1).substr(0, osc.getString(1).rfind('[') - 2);
		uint8_t category = osc.getInt(2);
		if (category == 0) {
			for (int k = 0; k < 6; k++) {
				for (int i = 0; i < categoryData[k].parameters; i++) {
					if (param[k][i].wheel == wheel) {
						param[k][i].value = 0.0f;
						param[k][i].wheel = 0;
						param[k][i].active = false;
						for (int j = 0; j < encoders; j++) {
							if (i == idx[j]) {
								if (callEncoder != nullptr) callEncoder(j + 1);
								return j + 1;
								}
							}
						}
					}
				}
			return 0;
			}
		
		for (int k = 0; k < 6; k++) {
			for (int i = 0; i < categoryData[k].parameters; i++) {
				if (parameter != param[k][i].parameter && param[k][i].wheel == wheel) {
					param[k][i].value = 0.0f;
					param[k][i].wheel = 0;
					param[k][i].active = false;
					}
				}	
			}
		
		for (int i = 0; i < categoryData[category - 1].parameters; i++) {
			if (param[category - 1][i].parameter == parameter) {
				param[category - 1][i].value = osc.getFloat(3);
				param[category - 1][i].wheel = wheel;
				param[category - 1][i].active = true;
				for (int j = 0; j < encoders; j++) {
					if (i == idx[j]) {
						if (callEncoder != nullptr) callEncoder(j + 1);
						return j + 1;
						}
					}
				}
			}
		}	
	return 0;
	}

string SelectCategory::parameter(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return "";
	if (encoder > 0 && encoder <= encoders) {
		if (categoryData[currentCategory].parameters > 0) {
			return param[currentCategory][idx[encoder - 1]].parameter;
			}
		}
	return "";
	}

string SelectCategory::alias(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return "";
	if (encoder > 0 && encoder <= encoders) {
		if (categoryData[currentCategory].parameters > 0) {
			if (param[currentCategory][idx[encoder - 1]].alias.size())
				return param[currentCategory][idx[encoder - 1]].alias;
			else
				return param[currentCategory][idx[encoder - 1]].parameter;
			}
		}
	return "";
	}

float SelectCategory::value(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return 0.0f;
	if (encoder > 0 && encoder <= encoders) {
		if (categoryData[currentCategory].parameters > 0)
			return param[currentCategory][idx[encoder - 1]].value;
		}
	return 0.0f;
	}

bool SelectCategory::active(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return false;
	if (encoder > 0 && encoder <= encoders) {
		if (categoryData[currentCategory].parameters > 0)
			return param[currentCategory][idx[encoder - 1]].active;
		}
	return false;
	}

void SelectCategory::indexEncoder(category_t category) {
	for (uint8_t i = 0; i < encoders; i++) {
		idx[i] = ((categoryData[category].currentPage - 1) * encoders) + i;
		if (idx[i] >= categoryData[category].parameters) idx[i] = -1;
		}
	if (callPage != nullptr) callPage();
	}

category_t SelectCategory::category() {
	return currentCategory;
	}

string SelectCategory::categoryName() {
	switch (currentCategory) {
		case INTENSITY:
			return "Intensity";
		case FOCUS:
			return "Focus";
		case COLOR:
			return "Color";
		case IMAGE:
			return "Image";
		case FORM:
			return "Form";
		case SHUTTER:
			return "Shutter";
		}
	return "";
	}

uint8_t SelectCategory::page() {
	return categoryData[currentCategory].currentPage;
	}

uint8_t SelectCategory::pages() {
	return categoryData[currentCategory].pages;
	}

uint8_t SelectCategory::page(category_t category) {
	return categoryData[category].currentPage;
	}

uint8_t SelectCategory::pages(category_t category) {
	return categoryData[category].pages;
	}

uint8_t SelectCategory::count(category_t category) {
	return categoryData[category].parameters;
	}

void SelectCategory::update() {
	if (state2nd || state3rd) return;
	for (uint8_t i = 0; i < 6; i++) {
		if (digitalRead(pins[i]) != pinsLast[i]) {
			if (pinsLast[i] == false) {
				pinsLast[i] = true;
				}
			else {
				pinsLast[i] = false;
				currentCategory = (category_t)i;
				if (lastCategory != (category_t)i) {
					lastCategory = (category_t)i;
					}
				else {
					if (categoryData[(category_t)i].pages == 0) return;
					if (categoryData[(category_t)i].currentPage < categoryData[(category_t)i].pages) {
						categoryData[(category_t)i].currentPage++;
						}
					else {
						categoryData[(category_t)i].currentPage = 1;
						}
					}
				indexEncoder((category_t)i);
				}
			return;
			}
		}
	}

void SelectCategory::update(bool stateIntens, bool stateFocus, bool stateColor, bool stateImage, bool stateForm, bool stateShutter) {
	if (state2nd || state3rd) return;
	bool states[6] {stateIntens, stateFocus, stateColor, stateImage, stateForm, stateShutter}; 
	for (uint8_t i = 0; i < 6; i++) {
		if (states[i] != pinsLast[i]) {
			if (pinsLast[i] == false) {
				pinsLast[i] = true;
				}
			else {
				pinsLast[i] = false;
				currentCategory = (category_t)i;
				if (lastCategory != (category_t)i) {
					lastCategory = (category_t)i;
					}
				else {
					if (categoryData[(category_t)i].pages == 0) return;
					if (categoryData[(category_t)i].currentPage < categoryData[(category_t)i].pages) {
						categoryData[(category_t)i].currentPage++;
						}
					else {
						categoryData[(category_t)i].currentPage = 1;
						}
					}
				indexEncoder((category_t)i);
				}
			return;
			}
		}
	}

void SelectCategory::update(category_t category) {
	if (state2nd || state3rd) return;
	currentCategory = category;
	if (lastCategory != category) {
		lastCategory = category;
		}
	else {
		if (categoryData[category].pages == 0) return;
		if (categoryData[category].currentPage < categoryData[category].pages) {
			categoryData[category].currentPage++;
			}
		else {
			categoryData[category].currentPage = 1;
			}
		}
		indexEncoder(category);
	}


/*******************************************************************************
 * Parameter category dynamic handling class
*******************************************************************************/

SelectDynamic::SelectDynamic(uint8_t pinIntens, uint8_t pinFocus, uint8_t pinColor, uint8_t pinImage, uint8_t pinForm, uint8_t pinShutter, uint8_t encoders) {
	pins[0] = pinIntens;
	pins[1] = pinFocus;
	pins[2] = pinColor;
	pins[3] = pinImage;
	pins[4] = pinForm;
	pins[5] = pinShutter;
	for (uint8_t i = 0; i < 6; i++) {
		pinMode(pins[i], INPUT_PULLUP);
		pinsLast[i] = digitalRead(pins[i]);
		}
	this->encoders = encoders;
	idx = new int [encoders];
	for (uint8_t i = 0; i < encoders; i++) idx[i] = -1;
	}

SelectDynamic::SelectDynamic(uint8_t encoders) {
	this->encoders = encoders;
	idx = new int [encoders];
	for (uint8_t i = 0; i < encoders; i++) idx[i] = -1;
	}

void SelectDynamic::callbackPage(cbptr call) {
	this->callPage = call;
	}

void SelectDynamic::callbackEncoder(cbptr2 call) {
	this->callEncoder = call;
	}

string SelectDynamic::parameter(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return "";
	if (categoryData[currentCategory].parameters == 0) return "";
	if (encoder > 0 && encoder <= encoders) {
			return param[idx[encoder - 1]].parameter;
		}
	return "";
	}

string SelectDynamic::alias(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return "";
	if (categoryData[currentCategory].parameters == 0) return "";
	if (encoder > 0 && encoder <= encoders) {
		if (aliases.size()) {
			for (size_t i = 0; i < aliases.size(); i++) {
				if (aliases[i][0].compare(param[idx[encoder - 1]].parameter) == 0) {
					return aliases[i][1];
					}
				}
			}
		return param[idx[encoder - 1]].parameter;
		}
	return "";
}

void SelectDynamic::alias(string parameter, string alias) {
	aliases.push_back({parameter, alias});
	}

bool SelectDynamic::active(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return false;
	return true;
	}

uint8_t SelectDynamic::wheel(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return 0;
	if (encoder <= encoders)
		return idx[encoder - 1];
	return 0;
	}

float SelectDynamic::value(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return 0.0f;
	if (encoder > 0 && encoder <= encoders) {
		return param[idx[encoder - 1]].value;
		}
	return 0.0f;
	}

void SelectDynamic::indexWheel() {
	for(uint8_t i = 0; i < 6; i++) {
		categoryData[i].parameters = 0;
		categoryData[i].start = 0;
		categoryData[i].end = 0;
		categoryData[i].pages = 0;
		categoryData[i].currentPage = 0;
		}
	size_t loop = 0;
	for (uint8_t i = 0; i < 6; i++) {
		for (; loop < param.size(); loop++) {
			if (param[loop].category == i + 1) {
				++categoryData[i].parameters;
				categoryData[i].end = loop;
				}
			else break;
			}
		}
	for (uint8_t i = 0; i < 6; i++) {
		if (categoryData[i].parameters == 0) {
		categoryData[i].start = 0;
		categoryData[i].pages = 0;
			}
		else {
			categoryData[i].start = categoryData[i].end - categoryData[i].parameters + 1;
			if (categoryData[i].parameters > encoders) { 
				if ((categoryData[i].parameters % encoders) == 0) {
					categoryData[i].pages = categoryData[i].parameters / encoders;
					}
				else {
					categoryData[i].pages = categoryData[i].parameters / encoders + 1;
					}
				}
			else categoryData[i].pages = 1;
			}
		}
	indexCollect();
	}

void SelectDynamic::indexCollect() {
	for (uint8_t i = 0; i < 6; i++) {
		if (categoryData[i].parameters)
			categoryData[i].currentPage = 1;
		else categoryData[i].currentPage = 0;
		}
	if (categoryData[currentCategory].parameters) {
		for (uint8_t i = 0; i < encoders; i++) {
			int wheel = categoryData[currentCategory].start + i;
			wheel = wheel + encoders * (categoryData[currentCategory].currentPage - 1);
			if (wheel > categoryData[currentCategory].end) wheel = -1;
			idx[i] = wheel;
			}
		}
	else {
		for (uint8_t i = 0; i < encoders; i++) {
			idx[i] = -1;
			}
		}
	if (callPage != nullptr) callPage();
	}

void SelectDynamic::indexEncoder(category_t category) {
	for (uint8_t i = 0; i < encoders; i++) {
		if (categoryData[category].parameters) {
			int wheel = categoryData[category].start + i;
			wheel = wheel + encoders * (categoryData[category].currentPage - 1);
			if (wheel > categoryData[category].end) wheel = -1;
			idx[i] = wheel;
			}
		else idx[i] = -1;
		}
	if (callPage != nullptr) callPage();
	}

uint8_t SelectDynamic::parse() {
	if (osc.getPattern().find("/eos/out/active/wheel/") == 0) {
		uint16_t wheel = stoi(osc.getPattern().substr(osc.getPattern().rfind('/') + 1));
		uint8_t category = osc.getInt(2);
		string parameter = osc.getString(1).substr(0, osc.getString(1).rfind('[') - 2);
		if (param.size() < wheel) param.resize(wheel);
		if (parameter.compare(param[wheel - 1].parameter) != 0) {
			if (collectFlag == false) {
				collectFlag = true;
				collectTime = millis();
				}
			}
		param[wheel - 1].category = category;
		param[wheel - 1].value = osc.getFloat(3);
		param[wheel - 1].parameter = parameter;
		for (uint8_t j = 0; j < encoders; j++) {
			if (wheel - 1 == idx[j]) {
				if (callEncoder != nullptr) callEncoder(j + 1);
				return j + 1;
				}
			}
		}
	return 0;
	}

void SelectDynamic::collect() {
	if ((millis() >= (collectTime + COLLECT_DELAY)) && collectFlag == true) {
		collectFlag = false;
		indexWheel();
		}
	}

category_t SelectDynamic::category() {
	return currentCategory;
	}

string SelectDynamic::categoryName() {
	switch (currentCategory) {
		case INTENSITY:
			return "Intensity";
		case FOCUS:
			return "Focus";
		case COLOR:
			return "Color";
		case IMAGE:
			return "Image";
		case FORM:
			return "Form";
		case SHUTTER:
			return "Shutter";
		}
	return "";
	}

uint8_t SelectDynamic::page() {
	return categoryData[currentCategory].currentPage;
	}

uint8_t SelectDynamic::pages() {
	return categoryData[currentCategory].pages;
	}

uint8_t SelectDynamic::page(category_t category) {
	return categoryData[category].currentPage;
}

uint8_t SelectDynamic::pages(category_t category) {
	return categoryData[category].pages;
}

uint8_t SelectDynamic::count(category_t category) {
	return categoryData[category].parameters;
}

void SelectDynamic::update() {
	collect();
	if (state2nd || state3rd) return;
		for (uint8_t i = 0; i < 6; i++) {
		if (digitalRead(pins[i]) != pinsLast[i]) {
			if (pinsLast[i] == false) {
				pinsLast[i] = true;
				}
			else {
				pinsLast[i] = false;
				currentCategory = (category_t)i;
				if (lastCategory != (category_t)i) {
					lastCategory = (category_t)i;
					}
				else {
					if (categoryData[(category_t)i].pages == 0) return;
					if (categoryData[(category_t)i].currentPage < categoryData[(category_t)i].pages) {
						categoryData[(category_t)i].currentPage++;
						}
					else {
						categoryData[(category_t)i].currentPage = 1;
						}
					}
				indexEncoder((category_t)i);
				}
			return;
			}
		}
	}

void SelectDynamic::update(bool stateIntens, bool stateFocus, bool stateColor, bool stateImage, bool stateForm, bool stateShutter) {
	collect();
	if (state2nd || state3rd) return;
	bool states[6] {stateIntens, stateFocus, stateColor, stateImage, stateForm, stateShutter}; 
	for (uint8_t i = 0; i < 6; i++) {
		if (states[i] != pinsLast[i]) {
			if (pinsLast[i] == false) {
				pinsLast[i] = true;
				}
			else {
				pinsLast[i] = false;
				currentCategory = (category_t)i;
				if (lastCategory != (category_t)i) {
					lastCategory = (category_t)i;
					}
				else {
					if (categoryData[(category_t)i].pages == 0) return;
					if (categoryData[(category_t)i].currentPage < categoryData[(category_t)i].pages) {
						categoryData[(category_t)i].currentPage++;
						}
					else {
						categoryData[(category_t)i].currentPage = 1;
						}
					}
				indexEncoder((category_t)i);
				}
			return;
			}
		}
	}

void SelectDynamic::update(category_t category) {
	collect();
	if (state2nd || state3rd) return;
	currentCategory = category;
	if (lastCategory != category) {
		lastCategory = category;
		}
	else {
		if (categoryData[category].pages == 0) return;
		if (categoryData[category].currentPage < categoryData[category].pages) {
			categoryData[category].currentPage++;
			}
		else {
			categoryData[category].currentPage = 1;
			}
		}
		indexEncoder(category);
	}


/*******************************************************************************
 * Special parser classes
*******************************************************************************/

Softkey::Softkey() {
	}

void Softkey::callback(cbptr2 call) {
	this->call = call;
	}

uint8_t Softkey::parse() {
	if (osc.getPattern().find("/eos/out/softkey/") == 0) {
		uint8_t idx = stoi(osc.getPattern().substr(osc.getPattern().rfind('/') + 1));
		softkey[idx - 1] = osc.getString(1);
		if (call != nullptr) call(idx);
		return idx;
		}
	return 0;
	}

string Softkey::label(uint8_t sk) {
	return softkey[sk - 1];
	}

PanTilt::PanTilt() {}

void PanTilt::callback(cbptr call) {
	this->call = call;
	}

bool PanTilt::parse() {
	if (osc.getPattern().compare("/eos/out/pantilt") == 0) {
		if (osc.getTagSize()) activ = true;
		else activ = false;
		panMinVal = osc.getFloat(1);
		panVal = osc.getFloat(5);
		panMaxVal = osc.getFloat(2);
		tiltMinVal = osc.getFloat(3);
		tiltVal = osc.getFloat(6);
		tiltMaxVal = osc.getFloat(4);
		if (call != nullptr) call();
		return true;
		}
	return false;
	}

bool PanTilt::active() {
	return activ;
	}

float PanTilt::panMin() {
	return panMinVal;
	}

float PanTilt::panMax() {
	return panMaxVal;
	}

float PanTilt::pan() {
	return panVal;
	}

float PanTilt::tiltMin() {
	return tiltMinVal;
	}

float PanTilt::tiltMax() {
	return tiltMaxVal;
	}

float PanTilt::tilt() {
	return tiltVal;
	}

XYZ::XYZ() {}

void XYZ::callback(cbptr call) {
	this->call = call;
	}

bool XYZ::parse() {
	if (osc.getPattern().compare("/eos/out/xyz") == 0) {
		if (osc.getTagSize()) activ = true;
		else activ = false;
		xVal = osc.getFloat(1);
		yVal = osc.getFloat(2);
		zVal = osc.getFloat(3);
		if (call != nullptr) call();
		return true;
		}
	return false;
	}

bool XYZ::active() {
	return activ;
	}

float XYZ::x() {
	return xVal;
	}

float XYZ::y() {
	return yVal;
	}

float XYZ::z() {
	return zVal;
	}

HueSat::HueSat() {}

void HueSat::callback(cbptr call) {
	this->call = call;
	}

bool HueSat::parse() {
	if (osc.getPattern().compare("/eos/out/color/hs") == 0) {
		if (osc.getTagSize()) activ = true;
		else activ = false;
		hueVal = osc.getFloat(1);
		saturationVal = osc.getFloat(2);
		HStoRGB(hueVal, saturationVal);
		color565Val = col565(redVal, greenVal, blueVal);
		if (call != nullptr) call();
		return true;
		}
	return false;
	}

bool HueSat::active() {
	return activ;
	}

float HueSat::hue() {
	return hueVal;
	}

float HueSat::saturation() {
	return saturationVal;
	}

uint8_t HueSat::red() {
	return redVal;
	}

uint8_t HueSat::green() {
	return greenVal;
	}

uint8_t HueSat::blue() {
	return blueVal;
	}

uint16_t HueSat::color565() {
	return color565Val;
	}

void HueSat::HStoRGB(float h, float s) {
	s /= 100;
	float v = 1; // asume value  = 1
	int i;
	float f, p, q, t;
	if (s == 0) { // achromatic (grey)
		redVal = 255;
		greenVal = 255;
		blueVal = 255;
		return;
		}
	h /= 60; // sector 0 to 5
	i = (int)h;
	f = h - i; // factorial part of h
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f) );
	switch (i) {
		case 0: {
			redVal = (uint8_t)(v * 255);
			greenVal = (uint8_t)(t * 255);
			blueVal = (uint8_t)(p * 255);
			return;
			}
		case 1: {
			redVal = (uint8_t)(q * 255);
			greenVal = (uint8_t)(v * 255);
			blueVal = (uint8_t)(p * 255);
			return;
			}
		case 2: {
			redVal = (uint8_t)(p * 255);
			greenVal = (uint8_t)(v * 255);
			blueVal = (uint8_t)(t * 255);
			return;
			}
		case 3: {
			redVal = (uint8_t)(p * 255);
			greenVal = (uint8_t)(q * 255);
			blueVal = (uint8_t)(v * 255);
			return;
			}
		case 4: {
			redVal = (uint8_t)(t * 255);
			greenVal = (uint8_t)(p * 255);
			blueVal = (uint8_t)(v * 255);
			return;
			}
		default: {
			redVal = (uint8_t)(v * 255);
			greenVal = (uint8_t)(p * 255);
			blueVal = (uint8_t)(q * 255);
			return;
			}
		}
	}

uint16_t HueSat::col565(uint8_t red, uint8_t green, uint8_t blue) {
	red >>= 3;
	green >>= 2;
	blue >>= 3;
	return (red << 11) | (green << 5) | blue;
	}

Channel::Channel() {}

void Channel::callback(cbptr call) {
	this->call = call;
	}

bool Channel::parse() {
	if (osc.getPattern().compare("/eos/out/active/chan") == 0) {
		channelString = osc.getString(1);
		if (channelString.size() == 0) {
			select = "";
			val = "";
			typ = "";
			dmx = "";
			if (call != nullptr) call();
			return true;
			}
		int space = channelString.find(' ');
		int bracketOpen = channelString.find('[');
		int bracketClose = channelString.find(']');
		int at = channelString.find('@');
		select = channelString.substr(0, space);
		val = channelString.substr(bracketOpen + 1, bracketClose - bracketOpen - 1);
		// check if only chan number -> not patched
		if (space == -1) {
			val = "";
			typ = "";
			dmx = "";
			if (call != nullptr) call();
			return true;
			}
		// check if typ is empty -> EOS Channel List BUG 
		if (bracketClose == (int)channelString.size() - 1) {
			typ = "";
			dmx = "";
			if (call != nullptr) call();
			return true;
			}
		if (at != -1) {
			typ = channelString.substr(bracketClose + 2, at - bracketClose - 2);
			dmx = channelString.substr(at + 2);
			}
		else { // no dmx infos
			typ = channelString.substr(bracketClose + 2);
			dmx = "";
			}
		if (call != nullptr) call();
		return true;
		}
	return false;
	}

string Channel::channel() {
	return channelString;
	}

string Channel::selection() {
	return select;
	}

string Channel::value() {
	return val;
	}

string Channel::type() {
	return typ;
	}

string Channel::address() {
	return dmx;
	}

Command::Command() {}

void Command::callback(cbptr call) {
	this->call = call;
	}

bool Command::parse() {
	if (osc.getPattern().compare("/eos/out/cmd") == 0) {
		commandString = osc.getString(1);
		if (call != nullptr) call();
		return true;
		}
	return false;
	}

string Command::command() {
	return commandString;
	}

Cue::Cue() {
	}

void Cue::callback(cbptrC call) {
	this->call = call;
	}

cue_t Cue::parse() {
	if (osc.getPattern().compare("/eos/out/active/cue/text") == 0) {
		cuedata[0].cueText = osc.getString(1);
		parseData(0);
		if (call != nullptr) call(ACTIVE);
		return ACTIVE;
		}

	if (osc.getPattern().compare("/eos/out/pending/cue/text") == 0) {
		cuedata[1].cueText = osc.getString(1);
		parseData(1);
		if (call != nullptr) call(PENDING);
		return PENDING;
		}
	
	if (osc.getPattern().compare("/eos/out/previous/cue/text") == 0) {
		cuedata[2].cueText = osc.getString(1);
		parseData(2);
		if (call != nullptr) call(PREVIOUS);
		return PREVIOUS;
		}
	return NODATA;
	}

void Cue::parseData(uint8_t data) {
	if (cuedata[data].cueText.size() == 0) { // empty texts for previous and pending cues
		cuedata[data].cueList = "";
		cuedata[data].cueNumber = "";
		cuedata[data].duration = "";
		cuedata[data].label = "";
		return;
		}
	int prog = cuedata[data].cueText.find('%');
	int firstSpace = cuedata[data].cueText.find(' ');
	int lastSpace = cuedata[data].cueText.rfind(' ');
	int secondLastSpace = cuedata[data].cueText.rfind(' ', lastSpace - 1);
	int slash = cuedata[data].cueText.find('/');
	cuedata[data].cueList = cuedata[data].cueText.substr(0, slash);
	cuedata[data].cueNumber = cuedata[data].cueText.substr(slash + 1, firstSpace - slash - 1);
	if (slash == -1) { // no cuelist/cuenumber info, happens on GotoQ Out / GotoQ 0
		cuedata[data].label = "";
		cuedata[data].cueList = "";
		cuedata[data].cueNumber = "";
		}
	if (prog == -1) { // no progress
		if (firstSpace == lastSpace) cuedata[data].label = ""; // no label
		else cuedata[data].label = cuedata[data].cueText.substr(firstSpace + 1, lastSpace - firstSpace - 1);
		cuedata[data].duration = cuedata[data].cueText.substr(lastSpace + 1);
		}
	else {
		cuedata[data].progress = cuedata[data].cueText.substr(lastSpace + 1);
		if (firstSpace == secondLastSpace) cuedata[data].label = ""; // no label
		else cuedata[data].label = cuedata[data].cueText.substr(firstSpace + 1, secondLastSpace - firstSpace - 1);
		cuedata[data].duration = cuedata[data].cueText.substr(secondLastSpace + 1, lastSpace - secondLastSpace - 1);
		}
	}

string Cue::cue(cue_t type) {
	switch (type) {
		case ACTIVE:
			return cuedata[0].cueText;
			break;
		case PENDING:
			return cuedata[1].cueText;
			break;
		case PREVIOUS:
			return cuedata[2].cueText;
			break;
		case NODATA:
			break;
		}
	return "";
	}

string Cue::cueList(cue_t type) {
	switch (type) {
		case ACTIVE:
			return cuedata[0].cueList;
			break;
		case PENDING:
			return cuedata[1].cueList;
			break;
		case PREVIOUS:
			return cuedata[2].cueList;
			break;
		case NODATA:
			break;
		}
	return "";
	}

string Cue::cueNumber(cue_t type) {
	switch (type) {
		case ACTIVE:
			return cuedata[0].cueNumber;
			break;
		case PENDING:
			return cuedata[1].cueNumber;
			break;
		case PREVIOUS:
			return cuedata[2].cueNumber;
			break;
		case NODATA:
			break;
		}
	return "";
	}

string Cue::label(cue_t type) {
	switch (type) {
		case ACTIVE:
			return cuedata[0].label;
			break;
		case PENDING:
			return cuedata[1].label;
			break;
		case PREVIOUS:
			return cuedata[2].label;
			break;
		case NODATA:
			break;
		}
	return "";
	}

string Cue::duration(cue_t type) {
	switch (type) {
		case ACTIVE:
			return cuedata[0].duration;
			break;
		case PENDING:
			return cuedata[1].duration;
			break;
		case PREVIOUS:
			return cuedata[2].duration;
			break;
		case NODATA:
			break;
		}
	return "";
	}

string Cue::progress(cue_t type) {
	switch (type) {
		case ACTIVE:
			return cuedata[0].progress;
			break;
		case PENDING:
			return cuedata[1].progress;
			break;
		case PREVIOUS:
			return cuedata[2].progress;
			break;
		case NODATA:
			break;
		}
	return "";
	}

Version::Version () {}

void Version::callback(cbptr call) {
	this->call = call;
	}

void Version::version() {
	osc.message("/eos/get/version");
	}

bool Version::parse() {
	if (osc.getPattern().compare("/eos/out/get/version") == 0) {
		softwareVersion = osc.getString(1);
		libraryVersion = osc.getString(2);
		if (call != nullptr) call();
		return true;
		}
	return false;
	}

string Version::software() {
	return softwareVersion;
	}

string Version::library() {
	return libraryVersion;
	}

User::User() {}

void User::callback(cbptr call) {
	this->call = call;
	}

bool User::parse() {
	if (osc.getPattern().compare("/eos/out/user") == 0) {
		userNumber = osc.getInt(1);
		if (call != nullptr) call();
		return true;
		}
	return false;
	}

uint16_t User::user() {
	return userNumber;
	}

Show::Show() {}

void Show::callback(cbptr call) {
	this->call = call;
	}

bool Show::parse() {
	if (osc.getPattern().compare("/eos/out/show/name") == 0) {
		showName = osc.getString(1);
		if (call != nullptr) call();
		return true;
		}
	return false;
	}

string Show::name() {
	return showName;
	}

EventState::EventState() {}

void EventState::callback(cbptr call) {
	this->call = call;
	}

bool EventState::parse() {
	if (osc.getPattern().compare("/eos/out/event/state") == 0 ) {
		eventState = osc.getInt(1);
		if (call != nullptr) call();
		return true;
		}
	return false;
	}

string EventState::state() {
	if(eventState == 0) return "BLIND";
	else return "LIVE";
	}


/*******************************************************************************
 * Helpers for creating patterns
*******************************************************************************/

string patternTypeNumber(button_t type, uint16_t number) {
	string pattern = "/eos/";
	switch (type) {
		case CHAN: {
			pattern += "chan/";
			pattern += to_string(number);
			break;
			}
		case GROUP: {
			pattern += "group/";
			pattern += to_string(number);
			break;
			}
		case IP: {
			pattern += "ip/";
			pattern += to_string(number);
			pattern += "/fire";
			break;
			}
		case FP: {
			pattern += "fp/";
			pattern += to_string(number);
			pattern += "/fire";
			break;
			}
		case CP: {
			pattern += "cp/";
			pattern += to_string(number);
			pattern += "/fire";
			break;
			}
		case BP: {
			pattern += "bp/";
			pattern += to_string(number);
			pattern += "/fire";
			break;
			}
		case PRESET: {
			pattern += "preset/";
			pattern += to_string(number);
			pattern += "/fire";
			break;
			}
		case MACRO: {
			pattern += "macro/";
			pattern += to_string(number);
			pattern += "/fire";
			break;
			}
		case MS: {
			pattern += "ms/";
			pattern += to_string(number);
			pattern += "/fire";
			break;
			}
		case SNAPSHOT: {
			pattern += "snap/";
			pattern += to_string(number);
			pattern += "/fire";
			break;
			}
		case FX: {
			pattern += "fx/";
			pattern += to_string(number);
			pattern += "/fire";
			break;
			}
		case SCENE: {
			pattern += "scene/";
			pattern += to_string(number);
			pattern += "/fire";
			break;
			}
		case PIXMAP: {
			pattern += "pixmap/";
			pattern += to_string(number);
			pattern += "/fire";
			break;
			}
		default: {}
		}
		return pattern;
	}

string patternTypeString(button_t type, string strng) {
	string pattern;
	switch (type) {
		case RAW: {
			pattern = strng;
			break;
			}
		case KEY: {
			pattern = "/eos/key/" + strng;
			break;
			}
		case SC: {
			pattern = "/eos/sc/" + strng;
			break;
			}
		default: {}
		}
		return pattern;
	}

string patternDS(button_t type, uint8_t buttons, uint8_t index, uint16_t page, bool flexi) {
	string dsInit = "/eos/ds/";
	dsInit += to_string(index);
	switch (type) {
		case CHAN: {
			dsInit += "/chan/";
			break;
			}
		case GROUP: {
			dsInit += "/group/";
			break;
			}
		case IP: {
			dsInit += "/ip/";
			break;
			}
		case FP: {
			dsInit += "/fp/";
			break;
			}
		case CP: {
			dsInit += "/cp/";
			break;
			}
		case BP: {
			dsInit += "/bp/";
			break;
			}
		case PRESET: {
			dsInit += "/preset/";
			break;
			}
		case MACRO: {
			dsInit += "/macro/";
			break;
			}
		case MS: {
			dsInit += "/ms/";
			break;
			}
		case SNAPSHOT: {
			dsInit += "/snap/";
			break;
			}
		case FX: {
			dsInit += "/fx/";
			break;
			}
		case SCENE: {
			dsInit += "/scene/";
			break;
			}
		default: {}
		}
	if (flexi) dsInit += "flexi/";
	dsInit += to_string(page);
	dsInit += '/';
	dsInit += to_string(buttons);
	return dsInit;
	}

string patternFader(uint8_t faders, uint8_t index, uint8_t page) {
	string faderInit = "/eos/fader/";
	faderInit += to_string(index);
	faderInit += "/config/";
	faderInit += to_string(page);
	faderInit += '/';
	faderInit += to_string(faders);
	return faderInit;
	}

string patternAbsolute(levels_t function, string param) {
	string pattern = "/eos/param/";
	pattern += param;
	switch (function) {
		case FULL: {
			pattern += "/full";
			break;
			}
		case OUT: {
			pattern += "/out";
			break;
			}
		case MINUS: {
			pattern += "/-%";
			break;
			}
		case PLUS: {
			pattern += "/+%";
			break;
			}
		case HOME: {
			pattern += "/home";
			break;
			}
		case LEVEL: {
			pattern += "/level";
			break;
			}
		case MAX: {
			pattern += "/max";
			break;
			}
		case MIN: {
			pattern += "/min";
			break;
			}
		default: {}
		}
		return pattern;
	}


/*******************************************************************************
 * Helpers for general data conversion
*******************************************************************************/

string ftos(float float32, uint8_t digits) {
	return to_string(float32).substr(0, to_string(float32).find(".") + digits + 1);
	}


/*******************************************************************************
 * OSC handling
*******************************************************************************/

OSC::OSC() {}

void OSC::begin() {
	interfaceType = OSCUSB;
	Serial.begin(115200);
	while (!Serial);
	sendHandshake();
	}

void OSC::begin(UDP &udp, IPAddress ip, uint16_t portUdpTx, uint16_t portUdpRx) {
	interfaceType = OSCUDP;
	this->udp = &udp;
	this->ip = ip;
	this->portUdpRx = portUdpRx;
	this->portUdpTx = portUdpTx;
	while(!udp.begin(portUdpRx));
	}

void OSC::begin(Client &tcp, IPAddress ip, uint16_t portTcp) {
	interfaceType = OSCTCP;
	this->tcp = &tcp;
	this->ip = ip;
	this->portTcp = portTcp;
	while(!tcp.connect(ip, portTcp));
	}

void OSC::message(string pattern) {
	bufferSend.assign(pattern.begin(), pattern.end());
	fillZeros();
	bufferSend.push_back(',');
	fillZeros();
	send();
	}

void OSC::message(string pattern, string strng) {
	bufferSend.assign(pattern.begin(), pattern.end());
	fillZeros();
	bufferSend.push_back(',');
	bufferSend.push_back('s');
	fillZeros();
	bufferSend.insert(bufferSend.end(), strng.begin(), strng.end());
	fillZeros();
	send();
	}

void OSC::message(string pattern, int32_t int32) {
	bufferSend.assign(pattern.begin(), pattern.end());
	fillZeros();
	bufferSend.push_back(',');
	bufferSend.push_back('i');
	fillZeros();
	// change byte order
	uint8_t *int32Array = (uint8_t *) &int32;
	bufferSend.push_back(int32Array[3]);
	bufferSend.push_back(int32Array[2]);
	bufferSend.push_back(int32Array[1]);
	bufferSend.push_back(int32Array[0]);
	send();
	}

void OSC::message(string pattern, float float32) {
	bufferSend.assign(pattern.begin(), pattern.end());
	fillZeros();
	bufferSend.push_back(',');
	bufferSend.push_back('f');
	fillZeros();
	// change byte order
	uint8_t *floatArray = (uint8_t *) &float32;
	bufferSend.push_back(floatArray[3]);
	bufferSend.push_back(floatArray[2]);
	bufferSend.push_back(floatArray[1]);
	bufferSend.push_back(floatArray[0]);
	send();
	}

void OSC::send() {
	switch (interfaceType) {
		case OSCUSB: {
			slipEncode();
			for (size_t i = 0; i < bufferSend.size(); i++) {
				Serial.write(bufferSend[i]);
				}
			break;
			}
		case OSCUDP: {
			udp->beginPacket(ip ,portUdpTx);
			udp->write(bufferSend.data(), bufferSend.size());
			udp->endPacket();
			break;
			}
		case OSCTCP: {
			slipEncode();
			if (!tcp->connected()) {
				callbackDisconnect();
				tcp->stop();
				while(!tcp->connect(ip, portTcp));
				}
			tcp->write(bufferSend.data(), bufferSend.size());
			break;
			}
		default:
			break;
		};
	}

int OSC::getTagSize() {
	return messageReceive.tagSize;
	}

string OSC::getPattern() {
	return messageReceive.pattern;
	}

int32_t OSC::getInt(uint8_t pos) {
	if (pos > 0 && pos <= messageReceive.tagSize && messageReceive.oscData[pos - 1].tag == 'i') {
		return messageReceive.oscData[pos - 1].int32;
		}
	return 0;
	}

float OSC::getFloat(uint8_t pos) {
	if (pos > 0 && pos <= messageReceive.tagSize && messageReceive.oscData[pos - 1].tag == 'f') {
		return messageReceive.oscData[pos - 1].float32;
		}
	return 0.0f;
	}

string OSC::getString(uint8_t pos) {
	if (pos > 0 && pos <= messageReceive.tagSize && messageReceive.oscData[pos - 1].tag == 's') {
		return messageReceive.oscData[pos - 1].strng;
		}
	return "";
	}

void OSC::sendHandshake() {
	Serial.write(END);
	Serial.write('O');
	Serial.write('K');
	Serial.write(END);
	}

bool OSC::receive() {
	switch (interfaceType) {
		case OSCUDP:
			if (receiveUDP()) {
				parse();
				return true;
				}
			break;
		case OSCTCP:
			if (receiveTCP()) {
				parse();
				return true;
				}
			break;
		case OSCUSB:
			if (receiveUSB()) {
				parse();
				return true;
				}
			break;
		}
		return false;
	}

// private methods

void OSC::parse() {
	messageReceive.pattern.clear();
	messageReceive.tag.clear();
	messageReceive.oscData.clear();
	int patternEnd = bufferReceive.find((char)0);
	messageReceive.pattern = bufferReceive.substr(0, patternEnd);
	int patternSize = messageReceive.pattern.size();
	int patternOffset = patternSize % 4;
	int tagStart;
	if (patternOffset == 0) tagStart = patternSize + 4;
	else tagStart = patternSize + (4 - patternOffset);
	int tagEnd = bufferReceive.find((char)0, tagStart);
	messageReceive.tag = bufferReceive.substr(tagStart + 1, tagEnd - tagStart - 1);
	int tagSize = messageReceive.tag.size() + 1;
	messageReceive.tagSize = tagSize - 1;
	int tagOffset = tagSize % 4;
	int dataStart;
	if (tagOffset == 0) dataStart = tagSize + 4 + tagStart;
	else dataStart = tagSize + (4 - tagOffset) + tagStart;
	messageReceive.oscData.resize(messageReceive.tagSize);
	for (int i = 0; i < messageReceive.tagSize; i++) {
		switch (messageReceive.tag[i]) {
			case 's': {
				messageReceive.oscData[i].tag = 's';
				int strngEnd = bufferReceive.find((char)0, dataStart);
				messageReceive.oscData[i].strng.assign(bufferReceive.begin() + dataStart, bufferReceive.begin() + strngEnd);
				int stringSize = messageReceive.oscData[i].strng.size();
				int stringOffset = stringSize % 4;
				if (stringOffset == 0) dataStart = dataStart + stringSize + 4;
				else dataStart = dataStart + stringSize + (4 - stringOffset);
				break;
				}
			case 'i': {
				messageReceive.oscData[i].tag = 'i';
				uint8_t int32Array[4];
				int32Array[0] = bufferReceive[dataStart + 3];
				int32Array[1] = bufferReceive[dataStart + 2];
				int32Array[2] = bufferReceive[dataStart + 1];
				int32Array[3] = bufferReceive[dataStart];
				messageReceive.oscData[i].int32 = *((int32_t*)int32Array);
				dataStart = dataStart + 4;
				break;
				}
			case 'f': {
				messageReceive.oscData[i].tag = 'f';
				uint8_t floatArray[4];
				floatArray[0] = bufferReceive[dataStart + 3];
				floatArray[1] = bufferReceive[dataStart + 2];
				floatArray[2] = bufferReceive[dataStart + 1];
				floatArray[3] = bufferReceive[dataStart];
				messageReceive.oscData[i].float32 = *((float*)floatArray);
				dataStart = dataStart + 4;
				break;
				}
			default: {}
			};
		}
	}

bool OSC::receiveUSB() {
	if (Serial.available()) {
		if (Serial.read() == END) {} // do nothing
		bufferReceive.clear();
		bool escFlag = false;
		while (Serial.available()) {
			uint8_t c = Serial.read();
			switch (c) {
				case END: {
					if (bufferReceive.compare("ETCOSC?") == 0) {
						sendHandshake();
						callbackConnect(); 
						return false;
						}
					else
						return true;
					}
				case ESC: {
					escFlag = true;
					break;
					}
				case ESC_END: {
					if (escFlag) {
						bufferReceive.push_back(END);
						escFlag = false;
						break;
						}
					}
				case ESC_ESC: {
					if (escFlag) {
						bufferReceive.push_back(ESC);
						escFlag = false;
						break;
						}
					}
				default: {
					bufferReceive.push_back(c);
					}
				};
			}
		}
	return false;
	}

bool OSC::receiveUDP() {
	int size = udp->parsePacket();
	if (size > 0) {
		bufferReceive.clear();
		for (int i = 0; i < size; i++) {
			bufferReceive.push_back(udp->read());
			}
		return true;
		}
	return false;
	}

bool OSC::receiveTCP() {
	if (tcp->available()) {
		if (tcp->read() == END) {} // do nothing
		bufferReceive.clear();
		bool escFlag = false;
		while (tcp->available()) {
			uint8_t c = tcp->read();
			switch (c) {
				case END: {
					return true;
					}
				case ESC: {
					escFlag = true;
					break;
					}
				case ESC_END: {
					if (escFlag) {
						bufferReceive.push_back(END);
						escFlag = false;
						break;
						}
					}
				case ESC_ESC: {
					if (escFlag) {
						bufferReceive.push_back(ESC);
						escFlag = false;
						break;
						}
					}
				default: {
					bufferReceive.push_back(c);
					}
				};
			}
		}
	return false;
	}

void OSC::slipEncode() {
	for (size_t i = 0; i < bufferSend.size(); i++) {
		if (bufferSend[i] == END) {
			bufferSend[i] = ESC;
			bufferSend.insert(bufferSend.begin() + i + 1, ESC_END);
			}
		else if (bufferSend[i] == ESC) {
			bufferSend[i] = ESC;
			bufferSend.insert(bufferSend.begin() + i + 1, ESC_ESC);
			}
		}
	bufferSend.insert(bufferSend.begin(), END);
	bufferSend.push_back(END);
	}

void OSC::fillZeros() {
	do {
		bufferSend.push_back(0);
	} while (bufferSend.size() %4 != 0);
}
