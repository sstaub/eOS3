#include "eOS3.h"

// TODO debounce algorithm
/*
	if (digitalRead(pin) != lastButtonState) {
		lastDebounceTime = millis();
		}
	if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY_MS) {

		}
*/
// TODO map function
/*
long map(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}
*/

OSC osc;

// callbacks
cbptr callbackConnect = connected;
cbptr callbackDisconnect = disconnected;
cbptr callbackReceived = maintain;

// connection handling
uint32_t lastMessageRxTime = 0;
bool timeoutPingSend = false;
#define PING_AFTER_IDLE_MS    2500
#define TIMEOUT_AFTER_IDLE_MS 5000

// wheel collection time
#define MARK_TIME 200

// shift, accelaration and 2nd/3rd variables
bool shiftState = false;
bool accelerationState = false;
uint8_t intensTick = 8;
uint8_t accelerationTick = 8;
bool state2nd = false;
bool state3rd = false; 

/*******************************************************************************
 * The master class eOS3
 ******************************************************************************/

eOS3::eOS3() {}

void eOS3::begin() {
	osc.begin();
	callbackConnect();
	}

void eOS3::begin(UDP &udp, IPAddress ip, uint16_t udpRxPort, uint16_t udpTxPort) {
	osc.begin(udp, ip, udpRxPort, udpTxPort);
	callbackConnect();
	}

void eOS3::begin(Client &tcp, IPAddress ip, uint16_t tcpPort) {
	osc.begin(tcp, ip, tcpPort);
	callbackConnect();
	}

void eOS3::update() {
	if(osc.receive()) callbackReceived();
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

void eOS3::subscription(int32_t subscribe) {
	osc.message("/eos/subscribe", subscribe);
	}

void eOS3::subscription(string parameter, int32_t subscribe) {
	osc.message("/eos/subscribe/param/" + parameter, subscribe);
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

void eOS3::initDS(button_t type, uint8_t count, uint8_t index, uint16_t page, bool flexi) {
	osc.message(patternDS(type, count, index, page, flexi));
	}

/*******************************************************************************
 * Shift button class
 ******************************************************************************/

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
	if (modus == TOGGLE) {
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
	if (modus == TOGGLE) {
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
 ******************************************************************************/

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
	if (modus == TOGGLE) {
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
	if (modus == TOGGLE) {
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
 ******************************************************************************/

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
	if (modus == TOGGLE) {
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
	if (modus == TOGGLE) {
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
 ******************************************************************************/

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
	if (modus == TOGGLE) {
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
	if (modus == TOGGLE) {
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
 ******************************************************************************/

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
 ******************************************************************************/

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
 ******************************************************************************/

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
 ******************************************************************************/

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
				return true;
				}
			}
		if (wheel == whl) {
			param = osc.getString(1).substr(0, osc.getString(1).rfind('[') - 2);
			val = osc.getFloat(3);
			act = true;
			return true;
			}
		}
	return false;
	}

float Wheel::value() {
	return val;
	}

bool Wheel::activ() {
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
 ******************************************************************************/

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

void Encoder::parameter(string param, string alias) {
	this->param = param;
	paramAlias = alias;
	int pos = 0;
	while (pos != -1) {
		pos = this->param.find('_', pos);
		if (pos >=0) this->param[pos] = ' ';
		}
	}

string Encoder::parameter() {
	return param;
	}

string Encoder::alias() {
	return paramAlias;
	}

bool Encoder::parse() {
	if (osc.getPattern().find("/eos/out/active/wheel/") == 0) {
		uint16_t whl = stoi(osc.getPattern().substr(osc.getPattern().rfind('/') + 1));
		if (osc.getInt(2) == 0) { // category 0, no data
			if (whl == wheel) {
				wheel = 0;
				val = 0.0f;
				act = false;
				return true;
				}
			}
		string parameter = osc.getString(1).substr(0, osc.getString(1).rfind('[') - 2);
		if (parameter != param && wheel == whl) {
			wheel = 0;
			val = 0.0f;
			act = false;
			return true;
			}
		if (parameter == param) {
			val = osc.getFloat(3);
			act = true;
			wheel = whl;
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
 ******************************************************************************/

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
 ******************************************************************************/

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
 ******************************************************************************/

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

void DSTool::flexiPin(uint8_t pinFlexi) {
	this->pinFlexi = pinFlexi;
	pinMode(pinFlexi, INPUT_PULLUP);
	pinFlexiLast = digitalRead(pinFlexi);
	}

void DSTool::init(button_t type, uint8_t count, uint8_t index) {
	this->type = type;
	this->count = count;
	this->index = index;
	dsData = new DSData[count];
	patternUp = "/eos/ds/" + to_string(index) + "/page/1";
	patternDown = "/eos/ds/" + to_string(index) + "/page/-1";
	patternSearchButton = "/eos/out/ds/" + to_string(index) + '/';
	patternSearchPage = "/eos/out/ds/" + to_string(index);
	osc.message(patternDS(type, count, index, currentPageLast[type], flexiStateLast[type]));
	}

void DSTool::typeDS(button_t type) {
	this->type = type;
	patternDS(type, count, index, currentPageLast[type], flexiStateLast[type]);
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
		dsData[ds - 1].number = osc.getString(2);
		return ds;
		}
	return 0;
	}

string DSTool::label(uint8_t number) {
	return dsData[number - 1].label;
	}

string DSTool::number(uint8_t number) {
	return dsData[number - 1].number;
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

void DSTool::update() {
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

	if (digitalRead(pinFlexi) != pinFlexiLast) {
		if (pinFlexiLast == false) pinFlexiLast = true;
		else {
			pinFlexiLast = false;
			flexiState = !flexiState;
			currentPage = 1;
			osc.message(patternDS(type, count, index, currentPage, flexiState));
			}
		return;
		}
	}

void DSTool::update(bool stateUp, bool stateDown, bool stateFlexi) {
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

	if (stateFlexi != pinFlexiLast) {
		if (pinFlexiLast == false) {
			pinFlexiLast = true;
			flexiState = !flexiState;
			currentPage = 1;
			osc.message(patternDS(type, count, index, 1, flexiState));
			}
		else pinFlexiLast = false;
		return;
		}
	}

/*******************************************************************************
 * Submaster class
 ******************************************************************************/

Submaster::Submaster(uint8_t analogPin, uint16_t sub) {
	this->analogPin = analogPin;
	this->sub = sub;
	updateTime = millis();
	patternSub = "/eos/sub/" + to_string(sub);
	}

Submaster::Submaster(uint16_t sub) {
	this->sub = sub;
	updateTime = millis();
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
	this->firePin = VIRTUAL_PIN;
	patternFire = "/eos/sub/" + to_string(sub) + "/fire";
	}

void Submaster::update() {
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

	if (firePin != NO_PIN && firePin != VIRTUAL_PIN) {
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
	}

void Submaster::update(int analog, bool fireState) {
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

	if (firePin != NO_PIN && firePin != VIRTUAL_PIN) {
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
	if (firePin == VIRTUAL_PIN) {
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
	}

void Submaster::updateValue(uint8_t value, bool fireState) {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		osc.message(patternSub, (value / 100.0f));
		if (call != nullptr) call();
		updateTime = millis();
		}

	if (firePin != NO_PIN && firePin != VIRTUAL_PIN) {
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
	if (firePin == VIRTUAL_PIN) {
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
	}

/*******************************************************************************
 * Fader class
 ******************************************************************************/

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
	patternFader = "/eos/fader/" + to_string(index) + "/" + to_string(fader) + "/fire";
	}

void Fader::fireButton() {
	this->firePin = VIRTUAL_PIN;
	patternFader = "/eos/fader/" + to_string(index) + "/" + to_string(fader) + "/fire";
	}

void Fader::stopButton(uint8_t stopPin) {
	this->stopPin = stopPin;
	pinMode(stopPin, INPUT_PULLUP);
	stopLast = digitalRead(stopPin);
	patternFader = "/eos/fader/" + to_string(index) + "/" + to_string(fader) + "/stop";
	}

void Fader::stopButton() {
	this->stopPin = VIRTUAL_PIN;
	patternFader = "/eos/fader/" + to_string(index) + "/" + to_string(fader) + "/stop";
	}

void Fader::loadButton(uint8_t loadPin) {
	this->loadPin = loadPin;
	pinMode(loadPin, INPUT_PULLUP);
	loadLast = digitalRead(loadPin);
	patternFader = "/eos/fader/" + to_string(index) + "/" + to_string(fader) + "/load";
	}

void Fader::loadButton() {
	this->loadPin = VIRTUAL_PIN;
	patternFader = "/eos/fader/" + to_string(index) + "/" + to_string(fader) + "/load";
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

void Fader::update() {
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

	if (firePin != NO_PIN && firePin != VIRTUAL_PIN) {
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

	if (stopPin != NO_PIN && stopPin != VIRTUAL_PIN) {
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

	if (loadPin != NO_PIN && loadPin != VIRTUAL_PIN) {
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
	}


/*******************************************************************************
 * Fader handling class
 ******************************************************************************/

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
	faderData = new FaderData[faders];
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
		return -1;
		}

	if (osc.getPattern().find(patternSearchRange) == 0) {
		uint8_t fader = stoi(osc.getPattern().substr(osc.getPattern().rfind("/") + 1));
		faderData[fader - 1].min = osc.getInt(1);
		faderData[fader - 1].max = osc.getInt(2);
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
		return fader;
		}

	return 0;
	}

int8_t FaderTool::parseValue() {
	if (osc.getPattern().find(patternSearchValue) == 0) {
		uint8_t fader = stoi(osc.getPattern().substr(osc.getPattern().find("/") + 1));
		faderData[fader - 1].value = (uint8_t)(osc.getFloat(1) * 100);
		return fader;
		}
	return 0;
	}

uint8_t FaderTool::page() {
	return currentPage;
	}

uint8_t FaderTool::value(uint8_t fader) {
	return faderData[fader - 1].value;
	}

string FaderTool::label(uint8_t fader) {
	return faderData[fader - 1].label;
	}

uint16_t FaderTool::rangeMin(uint8_t fader) {
	return faderData[fader - 1].min;
	}

uint16_t FaderTool::rangeMax(uint8_t fader) {
	return faderData[fader - 1].max;
	}

fader_t FaderTool::type(uint8_t fader) {
	return faderData[fader].faderType;
	}

string FaderTool::typeString(uint8_t fader) {
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
 ******************************************************************************/

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

void SelectParameter::callback(cbptr call) {
	this->call = call;
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
						if (i == idx[j]) return j + 1;
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
					if (i == idx[j]) return j + 1;
					}
				}
			if (param[i].parameter == parameter) {
				param[i].value = osc.getFloat(3);
				param[i].wheel = whl;
				param[i].active = true;
				for (uint8_t j = 0; j < encoders; j++) {
					if (i == idx[j]) return j + 1;
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
		return param[idx[encoder - 1]].alias;
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
	if (call != nullptr) call();
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
 ******************************************************************************/

SelectCategory::SelectCategory(uint8_t pinIntens, uint8_t pinFocus, uint8_t pinColor, uint8_t pinImage, uint8_t pinForm, uint8_t pinShutter, uint8_t encoders) {
	this->pinIntens = pinIntens;
	pinMode(pinIntens, INPUT_PULLUP);
	pinIntensLast = digitalRead(pinIntens);
	this->pinFocus = pinFocus;
	pinMode(pinFocus, INPUT_PULLUP);
	pinFocusLast = digitalRead(pinFocus);
	this->pinColor = pinColor;
	pinMode(pinColor, INPUT_PULLUP);
	pinColorLast = digitalRead(pinColor);
	this->pinImage = pinImage;
	pinMode(pinImage, INPUT_PULLUP);
	pinImageLast = digitalRead(pinImage);
	this->pinForm = pinForm;
	pinMode(pinForm, INPUT_PULLUP);
	pinFormLast = digitalRead(pinForm);
	this->pinShutter = pinShutter;
	pinMode(pinShutter, INPUT_PULLUP);
	pinShutterLast = digitalRead(pinShutter);
	this->encoders = encoders;
	idx = new int [encoders];
	for (uint8_t i = 0; i < encoders; i++) idx[i] = i;
	categoryData[INTENSITY].currentPage = 1;
	}

SelectCategory::SelectCategory(uint8_t encoders) {
	this->encoders = encoders;
	idx = new int [encoders];
	for (uint8_t i = 0; i < encoders; i++) idx[i] = i;
	categoryData[INTENSITY].currentPage = 1;
	}

void SelectCategory::parameter(category_t category, string parameter, string alias) {
	Wheel add;
	add.parameter = parameter;
	add.alias = alias;
	add.active = false;
	add.value = 0.0f;
	add.wheel = 0;
	param[category].push_back(add);
	categoryData[category].parameterCount = param[category].size();
	categoryData[category].currentPage = 1;
	categoryData[category].pagesCount = 1;
	if (categoryData[category].parameterCount > encoders) {
		categoryData[category].pagesCount = categoryData[category].parameterCount / encoders;
		if (categoryData[category].parameterCount % encoders) categoryData[category].pagesCount++;
		}
	}

void SelectCategory::callback(cbptr call) {
	this->call = call;
	}

uint8_t SelectCategory::parse() {
	if (osc.getPattern().find("/eos/out/active/wheel/") == 0) {
		uint16_t wheel = stoi(osc.getPattern().substr(osc.getPattern().rfind('/') + 1));
		string parameter = osc.getString(1).substr(0, osc.getString(1).rfind('[') - 2);
		uint8_t category = osc.getInt(2);
		if (category == 0) {
			for (uint8_t k = 0; k < 6; k++) {
				for (int i = 0; i < categoryData[k].parameterCount; i++) {
					if (param[k][i].wheel == wheel) {
						param[k][i].value = 0.0f;
						param[k][i].wheel = 0;
						param[k][i].active = false;
						for (uint8_t j = 0; j < encoders; j++) {
							//if (i == idx[j] && k == currentCategory) return j + 1;
							if (i == idx[j]) return j + 1;
							}
						}
					}
				}
			return 0;
			}
		
		for (uint8_t k = 0; k < 6; k++) {
			for (int i = 0; i < categoryData[k].parameterCount; i++) {
				if (parameter != param[k][i].parameter && param[k][i].wheel == wheel) {
					param[k][i].value = 0.0f;
					param[k][i].wheel = 0;
					param[k][i].active = false;
					//for (uint8_t j = 0; j < encoders; j++) {
						//if (i == idx[j] && k == currentCategory) return j + 1;
						//if (i == idx[j]) return j + 1;
					//	}
					}
				}	
			}
		
		for (int i = 0; i < categoryData[category - 1].parameterCount; i++) {
			if (param[category - 1][i].parameter == parameter) {
				param[category - 1][i].value = osc.getFloat(3);
				param[category - 1][i].wheel = wheel;
				param[category - 1][i].active = true;
				for (uint8_t j = 0; j < encoders; j++) {
					//if (i == idx[j] && (category - 1 == currentCategory)) return j + 1;
					if (i == idx[j]) return j + 1;
					}
				}
			}
		
		}	
	return 0;
	}

string SelectCategory::parameter(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return "";
	if (encoder > 0 && encoder <= encoders) {
		if (categoryData[currentCategory].parameterCount > 0)
			return param[currentCategory][idx[encoder - 1]].parameter;
		}
	return "";
	}

string SelectCategory::alias(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return "";
	if (encoder > 0 && encoder <= encoders) {
		if (categoryData[currentCategory].parameterCount > 0)
			return param[currentCategory][idx[encoder - 1]].alias;
		}
	return "";
	}

float SelectCategory::value(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return 0.0f;
	if (encoder > 0 && encoder <= encoders) {
		if (categoryData[currentCategory].parameterCount > 0)
			return param[currentCategory][idx[encoder - 1]].value;
		}
	return 0.0f;
	}

bool SelectCategory::active(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return false;
	if (encoder > 0 && encoder <= encoders) {
		if (categoryData[currentCategory].parameterCount > 0)
			return param[currentCategory][idx[encoder - 1]].active;
		}
	return false;
	}

void SelectCategory::indexEncoder(category_t category) {
	for (uint8_t i = 0; i < encoders; i++) {
		idx[i] = ((categoryData[category].currentPage - 1) * encoders) + i;
		if (idx[i] >= categoryData[category].parameterCount) idx[i] = -1;
		}
	if (call != nullptr) call();
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
	return categoryData[currentCategory].pagesCount;
	}

uint8_t SelectCategory::page(category_t category) {
	return categoryData[category].currentPage;
	}

uint8_t SelectCategory::pages(category_t category) {
	return categoryData[category].pagesCount;
	}

uint8_t SelectCategory::count(category_t category) {
	return categoryData[category].parameterCount;
	}

void SelectCategory::update() {
	if (state2nd || state3rd) return;
	if (digitalRead(pinIntens) != pinIntensLast) {
		if (pinIntensLast == false) {
			pinIntensLast = true;
			}
		else {
			pinIntensLast = false;
			currentCategory = INTENSITY;
			if (lastCategory != INTENSITY) {
				lastCategory = INTENSITY;
				}
			else {
				if (categoryData[INTENSITY].pagesCount == 0) return;
				if (categoryData[INTENSITY].currentPage < categoryData[INTENSITY].pagesCount) {
					categoryData[INTENSITY].currentPage++;
					}
				else {
					categoryData[INTENSITY].currentPage = 1;
					}
				}
			indexEncoder(INTENSITY);
			}
		return;
		}

	if (digitalRead(pinFocus) != pinFocusLast) {
		if (pinFocusLast == false) {
			pinFocusLast = true;
			}
		else {
			pinFocusLast = false;
			currentCategory = FOCUS;
			if (lastCategory != FOCUS) {
				lastCategory = FOCUS;
				}
			else {
				if (categoryData[FOCUS].pagesCount == 0) return;
				if (categoryData[FOCUS].currentPage < categoryData[FOCUS].pagesCount) {
					categoryData[FOCUS].currentPage++;
					}
				else {
					categoryData[FOCUS].currentPage = 1;
					}
				}
			indexEncoder(FOCUS);
			}
		return;
		}

	if (digitalRead(pinColor) != pinColorLast) {
		if (pinColorLast == false) {
			pinColorLast = true;
			}
		else {
			pinColorLast = false;
			currentCategory = COLOR;
			if (lastCategory != COLOR) {
				lastCategory = COLOR;
				}
			else {
				if (categoryData[COLOR].pagesCount == 0) return;
				if (categoryData[COLOR].currentPage < categoryData[COLOR].pagesCount) {
					categoryData[COLOR].currentPage++;
					}
				else {
					categoryData[COLOR].currentPage = 1;
					}
				}
			indexEncoder(COLOR);
			}
		return;
		}

	if (digitalRead(pinImage) != pinImageLast) {
		if (pinImageLast == false) {
			pinImageLast = true;
			}
		else {
			pinImageLast = false;
			currentCategory = IMAGE;
			if (lastCategory != IMAGE) {
				lastCategory = IMAGE;
				}
			else {
				if (categoryData[IMAGE].pagesCount == 0) return;
				if (categoryData[IMAGE].currentPage < categoryData[IMAGE].pagesCount) {
					categoryData[IMAGE].currentPage++;
					}
				else {
					categoryData[IMAGE].currentPage = 1;
					}
				}
			indexEncoder(IMAGE);
			}
		return;
		}

	if (digitalRead(pinForm) != pinFormLast) {
		if (pinFormLast == false) {
			pinFormLast = true;
			}
		else {
			pinFormLast = false;
			currentCategory = FORM;
			if (lastCategory != FORM) {
				lastCategory = FORM;
				}
			else {
				if (categoryData[FORM].pagesCount == 0) return;
				if (categoryData[FORM].currentPage < categoryData[FORM].pagesCount) {
					categoryData[FORM].currentPage++;
					}
				else {
					categoryData[FORM].currentPage = 1;
					}
				}
			indexEncoder(FORM);
			}
		return;
		}

	if (digitalRead(pinShutter) != pinShutterLast) {
		if (pinShutterLast == false) {
			pinShutterLast = true;
			}
		else {
			pinShutterLast = false;
			currentCategory = SHUTTER;
			if (lastCategory != SHUTTER) {
				lastCategory = SHUTTER;
				}
			else {
				if (categoryData[SHUTTER].pagesCount == 0) return;
				if (categoryData[SHUTTER].currentPage < categoryData[SHUTTER].pagesCount) {
					categoryData[SHUTTER].currentPage++;
					}
				else {
					categoryData[SHUTTER].currentPage = 1;
					}
				}
			indexEncoder(SHUTTER);
			}
		return;
		}
	}

void SelectCategory::update(bool stateIntens, bool stateFocus, bool stateColor, bool stateImage, bool stateForm, bool stateShutter) {
	if (state2nd || state3rd) return;
	if (stateIntens == pinIntensLast) {
		if (pinIntensLast == false) {
			pinIntensLast = true;
			}
		else {
			pinIntensLast = false;
			currentCategory = INTENSITY;
			if (lastCategory != INTENSITY) {
				lastCategory = INTENSITY;
				}
			else {
				if (categoryData[INTENSITY].pagesCount == 0) return;
				if (categoryData[INTENSITY].currentPage < categoryData[INTENSITY].pagesCount) {
					categoryData[INTENSITY].currentPage++;
					}
				else {
					categoryData[INTENSITY].currentPage = 1;
					}
				}
			indexEncoder(INTENSITY);
			}
		return;
		}

	if (stateFocus == pinFocusLast) {
		if (pinFocusLast == false) {
			pinFocusLast = true;
			}
		else {
			pinFocusLast = false;
			currentCategory = FOCUS;
			if (lastCategory != FOCUS) {
				lastCategory = FOCUS;
				}
			else {
				if (categoryData[FOCUS].pagesCount == 0) return;
				if (categoryData[FOCUS].currentPage < categoryData[FOCUS].pagesCount) {
					categoryData[FOCUS].currentPage++;
					}
				else {
					categoryData[FOCUS].currentPage = 1;
					}
				}
			indexEncoder(FOCUS);
			}
		return;
		}

	if (stateColor == pinColorLast) {
		if (pinColorLast == false) {
			pinColorLast = true;
			}
		else {
			pinColorLast = false;
			currentCategory = COLOR;
			if (lastCategory != COLOR) {
				lastCategory = COLOR;
				}
			else {
				if (categoryData[COLOR].pagesCount == 0) return;
				if (categoryData[COLOR].currentPage < categoryData[COLOR].pagesCount) {
					categoryData[COLOR].currentPage++;
					}
				else {
					categoryData[COLOR].currentPage = 1;
					}
				}
			indexEncoder(COLOR);
			}
		return;
		}

	if (stateImage == pinImageLast) {
		if (pinImageLast == false) {
			pinImageLast = true;
			}
		else {
			pinImageLast = false;
			currentCategory = IMAGE;
			if (lastCategory != IMAGE) {
				lastCategory = IMAGE;
				}
			else {
				if (categoryData[IMAGE].pagesCount == 0) return;
				if (categoryData[IMAGE].currentPage < categoryData[IMAGE].pagesCount) {
					categoryData[IMAGE].currentPage++;
					}
				else {
					categoryData[IMAGE].currentPage = 1;
					}
				}
			indexEncoder(IMAGE);
			}
		return;
		}

	if (stateForm == pinFormLast) {
		if (pinFormLast == false) {
			pinFormLast = true;
			}
		else {
			pinFormLast = false;
			currentCategory = FORM;
			if (lastCategory != FORM) {
				lastCategory = FORM;
				}
			else {
				if (categoryData[FORM].pagesCount == 0) return;
				if (categoryData[FORM].currentPage < categoryData[FORM].pagesCount) {
					categoryData[FORM].currentPage++;
					}
				else {
					categoryData[FORM].currentPage = 1;
					}
				}
			indexEncoder(FORM);
			}
		return;
		}

	if (stateShutter != pinShutterLast) {
		if (pinShutterLast == false) {
			pinShutterLast = true;
			}
		else {
			pinShutterLast = false;
			currentCategory = SHUTTER;
			if (lastCategory != SHUTTER) {
				lastCategory = SHUTTER;
				}
			else {
				if (categoryData[SHUTTER].pagesCount == 0) return;
				if (categoryData[SHUTTER].currentPage < categoryData[SHUTTER].pagesCount) {
					categoryData[SHUTTER].currentPage++;
					}
				else {
					categoryData[SHUTTER].currentPage = 1;
					}
				}
			indexEncoder(SHUTTER);
			}
		return;
		}
	}

/*******************************************************************************
 * Parameter category dynamic handling class
 ******************************************************************************/

SelectDynamic::SelectDynamic(uint8_t pinIntens, uint8_t pinFocus, uint8_t pinColor, uint8_t pinImage, uint8_t pinForm, uint8_t pinShutter, uint8_t encoders) {
	this->pinIntens = pinIntens;
	pinMode(pinIntens, INPUT_PULLUP);
	pinIntensLast = digitalRead(pinIntens);
	this->pinFocus = pinFocus;
	pinMode(pinFocus, INPUT_PULLUP);
	pinFocusLast = digitalRead(pinFocus);
	this->pinColor = pinColor;
	pinMode(pinColor, INPUT_PULLUP);
	pinColorLast = digitalRead(pinColor);
	this->pinImage = pinImage;
	pinMode(pinImage, INPUT_PULLUP);
	pinImageLast = digitalRead(pinImage);
	this->pinForm = pinForm;
	pinMode(pinForm, INPUT_PULLUP);
	pinFormLast = digitalRead(pinForm);
	this->pinShutter = pinShutter;
	pinMode(pinShutter, INPUT_PULLUP);
	pinShutterLast = digitalRead(pinShutter);
	this->encoders = encoders;
	for (uint8_t i = 0; i < WHEELS_MAX; i++) {
		param[i].category = 0;
		param[i].wheel = 0;
		param[i].value = 0.0f;
		param[i].parameter = "";
		}
	idx = new int [encoders];
	for (uint8_t i = 0; i < encoders; i++) idx[i] = i;
	}

SelectDynamic::SelectDynamic(uint8_t encoders) {
	this->encoders = encoders;
	for (uint8_t i = 0; i < WHEELS_MAX; i++) {
		param[i].category = 0;
		param[i].wheel = 0;
		param[i].value = 0.0f;
		param[i].parameter = "";
		}
	idx = new int [encoders];
	for (uint8_t i = 0; i < encoders; i++) idx[i] = i;
	categoryData[INTENSITY].currentPage = 1;
	}

void SelectDynamic::callback(cbptr call) {
	this->call = call;
	}

string SelectDynamic::parameter(uint8_t encoder) {
	if (idx[encoder - 1] == -1) return "";
	if (encoder > 0 && encoder <= encoders) {
		return param[idx[encoder - 1]].parameter;
		}
	return "";
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
	for(uint8_t i = 0; i < 7; i++) {
		categoryData[i].parameterCount = 0;
		categoryData[i].start = 0;
		categoryData[i].end = 0;
		categoryData[i].pages = 0;
		categoryData[i].currentPage = 1;
		}
	int loop = 1;
	for (; loop < WHEELS_MAX; loop++) {
		if (param[loop].category == INTENSITY) {
			++categoryData[INTENSITY].parameterCount;
			categoryData[INTENSITY].end = loop;
			}
		else break;
		}
	for (; loop < WHEELS_MAX; loop++) {
		if (param[loop].category == FOCUS) {
			++categoryData[FOCUS].parameterCount;
			categoryData[FOCUS].end = loop;
			}
		else break;
		}
	for (; loop < WHEELS_MAX; loop++) {
		if (param[loop].category == COLOR) {
			++categoryData[COLOR].parameterCount;
			categoryData[COLOR].end = loop;
			}
		else break;
		}
	for (; loop < WHEELS_MAX; loop++) {
		if (param[loop].category == IMAGE) {
			++categoryData[IMAGE].parameterCount;
			categoryData[IMAGE].end = loop;
			}
		else break;
		}
	for (; loop < WHEELS_MAX; loop++) {
		if (param[loop].category == FORM) {
			++categoryData[FORM].parameterCount;
			categoryData[FORM].end = loop;
			}
		else break;
		}
	for (; loop < WHEELS_MAX; loop++) {
		if (param[loop].category == SHUTTER) {
			++categoryData[SHUTTER].parameterCount;
			categoryData[SHUTTER].end = loop;
			}
		else break;
		}

	if (categoryData[INTENSITY].parameterCount == 0) {
		categoryData[INTENSITY].start = 0;
		categoryData[INTENSITY].pages = 0;
		}
	else {
		categoryData[INTENSITY].start = categoryData[INTENSITY].end - categoryData[INTENSITY].parameterCount + 1;
		if (categoryData[INTENSITY].parameterCount > encoders) { 
			if ((categoryData[INTENSITY].parameterCount % encoders) == 0) {
				categoryData[INTENSITY].pages = categoryData[INTENSITY].parameterCount / encoders;
				}
			else {
				categoryData[INTENSITY].pages = categoryData[INTENSITY].parameterCount / encoders + 1;
				}
			}
		else categoryData[INTENSITY].pages = 1;
		}

	if (categoryData[FOCUS].parameterCount == 0) {
		categoryData[FOCUS].start = 0;
		categoryData[FOCUS].pages = 0;
		}
	else {
		categoryData[FOCUS].start = categoryData[FOCUS].end - categoryData[FOCUS].parameterCount + 1;
		if (categoryData[FOCUS].parameterCount > encoders) {
			if ((categoryData[FOCUS].parameterCount % encoders) == 0) {
				categoryData[FOCUS].pages = categoryData[FOCUS].parameterCount / encoders;
				}
			else {
				categoryData[FOCUS].pages = categoryData[FOCUS].parameterCount / encoders + 1;
				}
			}
		else categoryData[FOCUS].pages = 1;
		}

	if (categoryData[COLOR].parameterCount == 0) {
		categoryData[COLOR].start = 0;
		categoryData[COLOR].pages = 0;
		}
	else {
		categoryData[COLOR].start = categoryData[COLOR].end - categoryData[COLOR].parameterCount + 1;
		if (categoryData[COLOR].parameterCount > encoders) {
			if ((categoryData[COLOR].parameterCount % encoders) == 0) {
				categoryData[COLOR].pages = categoryData[COLOR].parameterCount / encoders;
				}
			else {
				categoryData[COLOR].pages = categoryData[COLOR].parameterCount / encoders + 1;
				}
			}
		else categoryData[COLOR].pages = 1;
		}

	if (categoryData[IMAGE].parameterCount == 0) {
		categoryData[IMAGE].start = 0;
		categoryData[IMAGE].pages = 0;
		}
	else {
		categoryData[IMAGE].start = categoryData[IMAGE].end - categoryData[IMAGE].parameterCount + 1;
		if (categoryData[IMAGE].parameterCount > encoders) {
			if ((categoryData[IMAGE].parameterCount % encoders) == 0) {
				categoryData[IMAGE].pages = categoryData[IMAGE].parameterCount / encoders;
				}
			else {
				categoryData[IMAGE].pages = categoryData[IMAGE].parameterCount / encoders + 1;
				}
			}
		else categoryData[IMAGE].pages = 1;
		}

	if (categoryData[FORM].parameterCount == 0) {
		categoryData[FORM].start = 0;
		categoryData[FORM].pages = 0;
		}
	else {
		categoryData[FORM].start = categoryData[FORM].end - categoryData[FORM].parameterCount + 1;
		if (categoryData[FORM].parameterCount > encoders) {
			if ((categoryData[FORM].parameterCount % encoders) == 0) {
				categoryData[FORM].pages = categoryData[FORM].parameterCount / encoders;
				}
			else {
				categoryData[FORM].pages = categoryData[FORM].parameterCount / encoders + 1;
				}
			}
		else categoryData[FORM].pages = 1;
		}

	if (categoryData[SHUTTER].parameterCount == 0) {
		categoryData[SHUTTER].start = 0;
		categoryData[SHUTTER].pages = 0;
		}
	else {
		categoryData[SHUTTER].start = categoryData[SHUTTER].end - categoryData[SHUTTER].parameterCount + 1;
		if (categoryData[SHUTTER].parameterCount > encoders) {
			if ((categoryData[SHUTTER].parameterCount % encoders) == 0) {
				categoryData[SHUTTER].pages = categoryData[SHUTTER].parameterCount / encoders;
				}
			else {
				categoryData[SHUTTER].pages = categoryData[SHUTTER].parameterCount / encoders + 1;
				}
			}
		else categoryData[SHUTTER].pages = 1;
		}
	}

void SelectDynamic::indexEncoder(category_t category) {
	for (uint8_t i = 0; i < encoders; i++) {
		if (categoryData[category].parameterCount) {
			uint8_t wheel = categoryData[category].start + i;
			wheel = wheel + encoders * (categoryData[category].currentPage - 1);
			if (wheel > categoryData[category].end) wheel = 0;
			idx[i] = wheel;
			}
		else idx[i] = -1; // 0 or -1 ?
		}
	if (call != nullptr) call(); // execute callback
	}

uint8_t SelectDynamic::parse() {
	// following cases
	// 0. check if channel selection changed
	// 1. category is 0 -> reindex after timeout
	// 2. wheel is not the wheel of the current parameter -> reindex after time out
	// 3. parameter and wheel is correct -> only update the encoders
	// 4. parameter is correct but wheel is different -> update data then reindex
	if (millis() > markTime + MARK_TIME && markFlag == true) {
		indexWheel();
		markFlag = false;
		}

	if (osc.getPattern().find("/eos/out/active/wheel/") == 0) {
		uint16_t wheel = stoi(osc.getPattern().substr(osc.getPattern().rfind('/') + 1));
		// TODO vector push_back if wheel > param.size
		uint8_t category = osc.getInt(2);
		if (category == 0) {
			for (int i = 0; i < WHEELS_MAX; i++) {
				if (param[i].wheel == wheel) {
					param[i].category = category;
					param[i].value = 0.0f;
					param[i].wheel = 0;
					param[i].parameter = "";
					
					if (markFlag == false) {
						markFlag = true;
						markTime = millis();
						return 0;
						}
					
					//for (uint8_t j = 0; j < encoders; j++) {
					//	if (i == idx[j]) return j + 1;
					//	}
					}
				}
			return 0;
			}
		
		string parameter = osc.getString(1).substr(0, osc.getString(1).rfind('[') - 2);
		for (int i = 0; i < WHEELS_MAX; i++) {
			if (parameter != param[i].parameter && param[i].wheel == wheel) {
				param[i].category = category;
				param[i].value = 0.0f;
				param[i].wheel = 0;
				param[i].parameter = "";
				
				if (markFlag == false) {
					markFlag = true;
					markTime = millis();
					return 0;
					}
				
				//for (uint8_t j = 0; j < encoders; j++) {
				//	if (i == idx[j]) return j + 1;
				//	}
				}
			if (param[i].parameter == parameter) {
				param[i].category = category;
				param[i].value = osc.getFloat(3);
				param[i].wheel = wheel;
				param[i].parameter = parameter;
				/*
				if (markFlag == false) {
					markFlag = true;
					markTime = millis();
					return 0;
					}
				*/
				for (uint8_t j = 0; j < encoders; j++) {
					if (i == idx[j]) return j + 1;
					}
				}
			}
		}
		return 0;
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
	return categoryData[category].parameterCount;
}

void SelectDynamic::update() {
	if (state2nd || state3rd) return;
	if (digitalRead(pinIntens) != pinIntensLast) {
		if (pinIntensLast == false) {
			pinIntensLast = true;
			}
		else {
			pinIntensLast = false;
			currentCategory = INTENSITY;
			if (lastCategory != INTENSITY) {
				lastCategory = INTENSITY;
				}
			else {
				if (categoryData[INTENSITY].currentPage < categoryData[INTENSITY].pages) {
					categoryData[INTENSITY].currentPage++;
					}
				else {
					categoryData[INTENSITY].currentPage = 1;
					}
				}
			indexEncoder(INTENSITY);
			}
		return;
		}

	if (digitalRead(pinFocus) != pinFocusLast) {
		if (pinFocusLast == false) {
			pinFocusLast = true;
			}
		else {
			pinFocusLast = false;
			currentCategory = FOCUS;
			if (lastCategory != FOCUS) {
				lastCategory = FOCUS;
				}
			else {
				if (categoryData[FOCUS].currentPage < categoryData[FOCUS].pages) {
					categoryData[FOCUS].currentPage++;
					}
				else {
					categoryData[FOCUS].currentPage = 1;
					}
				}
			indexEncoder(FOCUS);
			}
		return;
		}

	if (digitalRead(pinColor) != pinColorLast) {
		if (pinColorLast == false) {
			pinColorLast = true;
			}
		else {
			pinColorLast = false;
			currentCategory = COLOR;
			if (lastCategory != COLOR) {
				lastCategory = COLOR;
				}
			else {
				if (categoryData[COLOR].currentPage < categoryData[COLOR].pages) {
					categoryData[COLOR].currentPage++;
					}
				else {
					categoryData[COLOR].currentPage = 1;
					}
				}
			indexEncoder(COLOR);
			}
		return;
		}

	if (digitalRead(pinImage) != pinImageLast) {
		if (pinImageLast == false) {
			pinImageLast = true;
			}
		else {
			pinImageLast = false;
			currentCategory = IMAGE;
			if (lastCategory != IMAGE) {
				lastCategory = IMAGE;
				}
			else {
				if (categoryData[IMAGE].currentPage < categoryData[IMAGE].pages) {
					categoryData[IMAGE].currentPage++;
					}
				else {
					categoryData[IMAGE].currentPage = 1;
					}
				}
			indexEncoder(IMAGE);
			}
		return;
		}

	if (digitalRead(pinForm) != pinFormLast) {
		if (pinFormLast == false) {
			pinFormLast = true;
			}
		else {
			pinFormLast = false;
			currentCategory = FORM;
			if (lastCategory != FORM) {
				lastCategory = FORM;
				}
			else {
				if (categoryData[FORM].currentPage < categoryData[FORM].pages) {
					categoryData[FORM].currentPage++;
					}
				else {
					categoryData[FORM].currentPage = 1;
					}
				}
			indexEncoder(FORM);
			}
		return;
		}

	if (digitalRead(pinShutter) != pinShutterLast) {
		if (pinShutterLast == false) {
			pinShutterLast = true;
			}
		else {
			pinShutterLast = false;
			currentCategory = SHUTTER;
			if (lastCategory != SHUTTER) {
				lastCategory = SHUTTER;
				}
			else {
				if (categoryData[SHUTTER].currentPage < categoryData[SHUTTER].pages) {
					categoryData[SHUTTER].currentPage++;
					}
				else {
					categoryData[SHUTTER].currentPage = 1;
					}
				}
			indexEncoder(SHUTTER);
			}
		return;
		}
	}

void SelectDynamic::update(bool stateIntens, bool stateFocus, bool stateColor, bool stateImage, bool stateForm, bool stateShutter) {
	if (state2nd || state3rd) return;
	if (stateIntens != pinIntensLast) {
		if (pinIntensLast == false) {
			pinIntensLast = true;
			currentCategory = INTENSITY;
			if (lastCategory != INTENSITY) {
				lastCategory = INTENSITY;
				}
			else {
				if (categoryData[INTENSITY].currentPage < categoryData[INTENSITY].pages) {
					categoryData[INTENSITY].currentPage++;
					}
				else {
					categoryData[INTENSITY].currentPage = 1;
					}
				}
			indexEncoder(INTENSITY);
			}
		else {
			pinIntensLast = false;
			}
		return;
		}

	if (stateFocus != pinFocusLast) {
		if (pinFocusLast == false) {
			pinFocusLast = true;
			currentCategory = FOCUS;
			if (lastCategory != FOCUS) {
				lastCategory = FOCUS;
				}
			else {
				if (categoryData[FOCUS].currentPage < categoryData[FOCUS].pages) {
					categoryData[FOCUS].currentPage++;
					}
				else {
					categoryData[FOCUS].currentPage = 1;
					}
				}
			indexEncoder(FOCUS);
			}
		else {
			pinFocusLast = false;
			}
		return;
		}

	if (stateColor != pinColorLast) {
		if (pinColorLast == false) {
			pinColorLast = true;
			currentCategory = COLOR;
			if (lastCategory != COLOR) {
				lastCategory = COLOR;
				}
			else {
				if (categoryData[COLOR].currentPage < categoryData[COLOR].pages) {
					categoryData[COLOR].currentPage++;
					}
				else {
					categoryData[COLOR].currentPage = 1;
					}
				}
			indexEncoder(COLOR);
			}
		else {
			pinColorLast = false;
			}
		return;
		}

	if (stateImage != pinImageLast) {
		if (pinImageLast == false) {
			pinImageLast = true;
			currentCategory = IMAGE;
			if (lastCategory != IMAGE) {
				lastCategory = IMAGE;
				}
			else {
				if (categoryData[IMAGE].currentPage < categoryData[IMAGE].pages) {
					categoryData[IMAGE].currentPage++;
					}
				else {
					categoryData[IMAGE].currentPage = 1;
					}
				}
			indexEncoder(IMAGE);
			}
		else {
			pinImageLast = false;
			}
		return;
		}

	if (stateForm != pinFormLast) {
		if (pinFormLast == false) {
			pinFormLast = true;
			currentCategory = FORM;
			if (lastCategory != FORM) {
				lastCategory = FORM;
				}
			else {
				if (categoryData[FORM].currentPage < categoryData[FORM].pages) {
					categoryData[FORM].currentPage++;
					}
				else {
					categoryData[FORM].currentPage = 1;
					}
				}
			indexEncoder(FORM);
			}
		else {
			pinFormLast = false;
			}
		return;
		}

	if (stateShutter != pinShutterLast) {
		if (pinShutterLast == false) {
			pinShutterLast = true;
			currentCategory = SHUTTER;
			if (lastCategory != SHUTTER) {
				lastCategory = SHUTTER;
				}
			else {
				if (categoryData[SHUTTER].currentPage < categoryData[SHUTTER].pages) {
					categoryData[SHUTTER].currentPage++;
					}
				else {
					categoryData[SHUTTER].currentPage = 1;
					}
				}
			indexEncoder(SHUTTER);
			}
		else {
			pinShutterLast = false;
			}
		return;
		}
	}

/*******************************************************************************
 * Special parser classes
 ******************************************************************************/

Softkey::Softkey() {
	}

uint8_t Softkey::parse() {
	if (osc.getPattern().compare("/eos/out/softkey/") == 0) {
		uint8_t idx = stoi(osc.getPattern().substr(osc.getPattern().rfind('/') + 1));
		softkey[idx - 1] = osc.getString(1);
		return idx;
		}
	return 0;
	}

string Softkey::label(uint8_t sk) {
	return softkey[sk - 1];
	}

PanTilt::PanTilt() {}

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

bool XYZ::parse() {
	if (osc.getPattern().compare("/eos/out/xyz") == 0) {
		if (osc.getTagSize()) activ = true;
		else activ = false;
		xVal = osc.getFloat(1);
		yVal = osc.getFloat(2);
		zVal = osc.getFloat(3);
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

bool HueSat::parse() {
	if (osc.getPattern().compare("/eos/out/color/hs") == 0) {
		if (osc.getTagSize()) activ = true;
		else activ = false;
		hueVal = osc.getFloat(1);
		saturationVal = osc.getFloat(2);
		HStoRGB(hueVal, saturationVal);
		color565Val = col565(redVal, greenVal, blueVal);
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

bool Channel::parse() {
	if (osc.getPattern().compare("/eos/out/active/chan") == 0) {
		channelString = osc.getString(1);
		if (channelString.size() == 0) {
			select = "";
			val = "";
			typ = "";
			dmx = "";
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
			return true;
			}
		// check if typ is empty -> EOS Channel List BUG 
		if (bracketClose == (int)channelString.size() - 1) {
			typ = "";
			dmx = "";
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

bool Command::parse() {
	if (osc.getPattern().compare("/eos/out/cmd") == 0) {
		commandString = osc.getString(1);
		return true;
		}
	return false;
	}

string Command::command() {
	return commandString;
	}

Cue::Cue() {
	}

cue_t Cue::parse() {
	if (osc.getPattern().compare("/eos/out/active/cue/text") == 0) {
		cuedata[0].cueText = osc.getString(1);
		parseData(0);
		return ACTIVE;
		}

	if (osc.getPattern().compare("/eos/out/pending/cue/text") == 0) {
		cuedata[0].cueText = osc.getString(1);
		parseData(1);
		return PENDING;
		}
	
	if (osc.getPattern().compare("/eos/out/previous/cue/text") == 0) {
		cuedata[0].cueText = osc.getString(1);
		parseData(2);
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

void Version::version() {
	osc.message("/eos/get/version");
	}

bool Version::parse() {
	if (osc.getPattern().compare("/eos/out/get/version") == 0) {
		softwareVersion = osc.getString(1);
		libraryVersion = osc.getString(2);
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

bool User::parse() {
	if (osc.getPattern().compare("/eos/out/user") == 0) {
		userNumber = osc.getInt(1);
		return true;
		}
	return false;
	}

uint16_t User::user() {
	return userNumber;
	}

Show::Show() {}

bool Show::parse() {
	if (osc.getPattern().compare("/eos/out/show/name") == 0) {
		showName = osc.getString(1);
		return true;
		}
	return false;
	}

string Show::name() {
	return showName;
	}

EventState::EventState() {}

bool EventState::parse() {
	if (osc.getPattern().compare("/eos/out/event/state") == 0 ) {
		eventState = osc.getInt(1);
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
 ******************************************************************************/

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
		case SNAP: {
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

string patternDS(button_t type, uint8_t count, uint8_t index, uint16_t page, bool flexi) {
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
		case SNAP: {
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
		case PIXMAP: {
			dsInit += "/pixmap/";
			break;
			}
		default: {}
		}
	if (flexi) dsInit += "flexi/";
	dsInit += to_string(page);
	dsInit += '/';
	dsInit += to_string(count);
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
 ******************************************************************************/

string ftos(float float32, uint8_t digits) {
	return to_string(float32).substr(0, to_string(float32).find(".") + digits + 1);
	}

/*******************************************************************************
 * OSC handling
 ******************************************************************************/

OSC::OSC() {}

void OSC::begin() {
	interfaceType = OSCUSB;
	Serial.begin(115200);
	while (!Serial) {};
	sendHandshake();
	}

void OSC::begin(UDP &udp, IPAddress ip, uint16_t portUdpTx, uint16_t portUdpRx) {
	interfaceType = OSCUDP;
	this->udp = &udp;
	this->ip = ip;
	this->portUdpRx = portUdpRx;
	this->portUdpTx = portUdpTx;
	while(!udp.begin(portUdpRx)) {};
	}

void OSC::begin(Client &tcp, IPAddress ip, uint16_t portTcp) {
	interfaceType = OSCTCP;
	this->tcp = &tcp;
	this->ip = ip;
	this->portTcp = portTcp;
	while(!tcp.connect(ip, portTcp)) {};
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
			// Serial.write(bufferSend.data(), bufferSend.size()); // TODO test
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
				tcp->stop();
				while(!tcp->connect(ip, portTcp)) {};
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

void OSC::callback(cbptr call) {
	this->call = call;
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
				if (call != nullptr) call();
				return true;
				}
			break;
		case OSCTCP:
			if (receiveTCP()) {
				parse();
				if (call != nullptr) call();
				return true;
				}
			break;
		case OSCUSB:
			if (receiveUSB()) {
				parse();
				if (call != nullptr) call();
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
				case END: { // TODO check if "/eos/"
						if (!bufferReceive.compare("ETCOSC?")) {
							sendHandshake();
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
				case END: { // TODO check for "/eos/"
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
