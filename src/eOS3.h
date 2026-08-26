/*
eOS3 for arduino by Stefan Staub (c)2026 is licensed under CC BY-NC-SA 4.0
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

// TODO fader fetch/lock up/down marker
// TODO 2nd -> shift, 3rd -> accelaration test

#ifndef EOS3_H
#define EOS3_H

/*******************************************************************************
 * Library includes
 ******************************************************************************/

#include "Arduino.h"

#include "Udp.h"
#include "Client.h"

#include <cstdint>
#include <string>
#include <vector>
using namespace std;

/*******************************************************************************
 * Defines
 ******************************************************************************/

// defines for SLIP
const uint8_t END = 0xC0;
const uint8_t ESC = 0xDB;
const uint8_t ESC_END = 0xDC;
const uint8_t ESC_ESC = 0xDD;

// sepecial pins
#define NO_PIN      0xFF
#define VIRTUAL_PIN 0xFE

// subscription
#define SUBSCRIBE   (int32_t)1
#define UNSUBSCRIBE (int32_t)0

// fader definitions
#define BUTTON_PRESS         (int32_t)1
#define BUTTON_RELEASE       (int32_t)0
#define FADER_UPDATE_RATE_MS 40 // update each 40ms
#define FADER_THRESHOLD      4 // Jitter threshold of the faders

// callback
typedef void (*cbptr)();

/*******************************************************************************
 * General handlers functions
 ******************************************************************************/

void maintain();
void connected();
void disconnected();

/*******************************************************************************
 * Enums und Structs
 ******************************************************************************/

typedef enum Debounce {
	SOFTWARE,
	HARDWARE
	} debounce_t;

typedef enum Interface {
	EOSUSB,
	EOSUDP,
	EOSTCP,
	} interface_t;

/**
 * @brief Button modes
 * 
 */
typedef enum ButtonMode {
	PUSH,
	TOGGLE
	} buttonMode_t;

/**
 * @brief Encoder direction
 * 
 */
typedef enum EncoderDirection {
	FORWARD,
	REVERSE
	} direction_t;

/**
 * @brief Button types
 * 
 */
typedef enum ButtonTypes {
	CHAN,
	GROUP,
	IP,
	FP,
	CP,
	BP,
	PRESET,
	MACRO,
	FX,
	SNAP,
	MS,
	SCENE,
	PIXMAP,
	CURVE,
	SC,
	RAW,
	KEY
	} button_t;

/**
 * @brief Parameter level modes
 * 
 */
typedef enum Levels {
	NONE,
	PARAMETER,
	HOME,
	OUT,
	LEVEL,
	FULL,
	MIN,
	MAX,
	MINUS,
	PLUS,
	} levels_t;

/**
 * @brief Parameter category
 * 
 */
typedef enum CategoryTypes {
	INTENSITY,
	FOCUS,
	COLOR,
	IMAGE,
	FORM,
	SHUTTER
	} category_t;

/**
 * @brief CueTypes
 * 
 */
typedef enum CueType {
	NODATA,
	ACTIVE,
	PENDING,
	PREVIOUS
	} cue_t;

/**
 * @brief CueTypes
 * 
 */
typedef enum FaderType {
	UNMAPPED, // unmapped fader
	SUB, // "S <x>"
	GM, // "GM"
	QL, // "L<x>"
	PR, // "Pr <x> <label>"
	IPR, // "IP <x> <label>"
	FPR, // "FP <x> <label>"
	CPR, // "CP <x> <label>"
	BPR, // "BP <x> <label>" 
	GFX, // "Global FX"
	MANUAL // "Man Time"
	} fader_t;


/*******************************************************************************
 * The master class eOS3
 ******************************************************************************/

/**
 * @brief Class definitions for a general interface
 * 
 */
class eOS3 {
	public:
		/**
		 * @brief Construct a new EOS object
		 * 
		 */
		eOS3();
	
		/**
 		* @brief Initialise the USB interface
 		* 
 		*/
		void begin();

		/**
		* @brief Initialise an UDP interface
		* 
		* @param ip EOS console IP address
		* @param udp UDP interface
		* @param udpRxPort EOS UDP RX receive port
		* @param udpTxPort EOS UDP TX transmit port
		*/
		void begin(UDP &udp, IPAddress ip, uint16_t udpRxPort = 8000, uint16_t udpTxPort = 8001);

		/**
		* @brief Initialise a TCP interface
		* 
		* @param ip EOS console IP address
		* @param tcp TCP interface
		* @param tcpPort TCP port, default 3037
		*/
		void begin(Client &tcp, IPAddress ip, uint16_t tcpPort = 3037);

		/**
		 * @brief Update the handlers for maintain(), connected() and disconnected(), must done in the while() loop
		 * 
		 */
		void update();

		/**
		 * @brief Filter for messages you want receive
		 * 
		 * 
		 * @param message you want receive
		 */
		void filter(string pattern);

		/**
		 * @brief Remove a single filter from the filter list
		 * 
		 * @param pattern OSC pattern you want receive
		 */
		void filterRemove(string pattern);

		/**
		 * @brief Clear all added filters
		 * 
		 */
		void filterClear();

		/**
		 * @brief Subscribe or unsubscribe EOS data updates
		 * 
		 * @param subscribe SUBSCRIBE or UNSUBSCRIBE
		 */
		void subscription(int32_t subscribe = SUBSCRIBE);

		/**
		 * @brief Subscribe or unsubscribe a parameter for receive data
		 * 
		 * @param parameter for subscription
		 * @param subscribe SUBSCRIBE or UNSUBSCRIBE
		 */
		void subscription(string parameter, int32_t subscribe = SUBSCRIBE);

		/**
		 * @brief Reset OSC settings
		 * 
		 */
		void reset();

		/**
		 * @brief send a ping without a message
		 * 
		 */
		void ping();

		/**
		 * @brief send a ping with a string meassage
		 * 
		 * @param message 
		 */
		void ping(string message);

		/**
		 * @brief send a ping with a number
		 * 
		 * @param message 
		 */
		void ping(int32_t number);

		/**
		 * @brief send a string to the command line
		 * 
		 * @param cmd command line String
		 */
		void command(string cmd);

		/**
		 * @brief send a new command line string
		 * 
		 * @param cmd command line String
		 */
		void newCommand(string newCmd);

		/**
		 * @brief set the user
		 * 
		 * @param userID user
		 */
		void user(int16_t userID);

		/**
		 * @brief Initialise a fader bank, this must done after an established connection
		 * 
		 * @param page fader page of the console, default is 1
		 * @param faders number of faders, standard is 10
		 * @param bank number of the OSC fader bank, default is 1
		 */
		void initFaders(uint8_t faders = 10, uint8_t index = 1, uint8_t page = 1);

		/**
		 * @brief Initialise a direct select bank, this must done after an established connection
		 * 
		 * @param type button type 
		 * @param count number of DS buttons
		 * @param index index number, default = 1
		 * @param page page number, default = 1
		 * @param flexi flexi state, default = false
		 */
		void initDS(button_t type, uint8_t count, uint8_t index = 1, uint16_t page = 1, bool flexi = false);

	private:

	};

/*******************************************************************************
 * Shift button class
 ******************************************************************************/

/**
 * @brief Shift button handler object
 * 
 */
class Shift {
	public:
		/**
		 * @brief Construct a new Shift button object
		 * 
		 * @param pin button pin, not needed for virtual devices
		 */
		Shift(uint8_t pin);
		Shift();

		/**
		 * @brief Set Intens ticks, only for encoder class
		 * 
		 * @param tick intens ticks, default 8 ticks
		 */
		void tick(uint8_t tick = 8);

		/**
		 * @brief Set the mode of the Shift button
		 * 
		 * @param modus PUSH or TOGGLE, default PUSH
		 */
		void mode(buttonMode_t modus);

		/**
		 * @brief Get the state of the shift button
		 * 
		 * @return true if pressed or in toggle
		 * @return false if not pressed
		 */
		bool state();

		/**
		 * @brief Update the state of the shift button, must done in the while() loop
		 * 
		 * @param state optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool state);

	private:
		uint8_t pin;
		uint8_t last;
		buttonMode_t modus = PUSH;
	};

/*******************************************************************************
 * Accelaration button class
 ******************************************************************************/

/**
 * @brief Accelaration button handler object
 * 
 */
class Acceleration {
	public:
		/**
		 * @brief Construct a new Accelaration button object
		 * 
		 * @param pin button pin, not needed for virtual devices
		 * @param mode PUSH or TOGGLE mode
	 */
		Acceleration(uint8_t pin);
		Acceleration();

		/**
		 * @brief Set accelaration ticks
		 * 
		 * @param tick accelaration ticks, default 8 ticks
		 */
		void tick(uint8_t tick = 8);

		/**
		 * @brief Set the mode of the accelaration button
		 * 
		 * @param modus PUSH or TOGGLE, default PUSH
		 */
		void mode(buttonMode_t modus);

		/**
		 * @brief Get the state of the accelaration button
		 * 
		 * @return true if pressed or in toggle
		 * @return false if not pressed
		 */
		bool state();

		/**
		 * @brief Update the state of the accelaration button, must done in the while() loop
		 * 
		 * @param state optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool state);

	private:
		uint8_t pin;
		uint8_t last;
		buttonMode_t modus = PUSH;
	};

/*******************************************************************************
 * 2nd button control class
 ******************************************************************************/

/**
 * @brief 2nd button control handler object
 * 
 */
class Control2nd {
	public:
		/**
		 * @brief Construct a new 2nd button control object
		 * 
		 * @param pin button pin, not needed for virtual devices
		 * @param mode PUSH or TOGGLE mode
	 */
		Control2nd(uint8_t pin);
		Control2nd();

		/**
		 * @brief Set the mode of the 2nd button control
		 * 
		 * @param modus PUSH or TOGGLE, default PUSH
		 */
		void mode(buttonMode_t modus);

		/**
		 * @brief Get the state of the accelaration button
		 * 
		 * @return true if pressed or in toggle
		 * @return false if not pressed
		 */
		bool state();

		/**
		 * @brief Update the state of the 2nd button control, must done in the while() loop
		 * 
		 * @param state optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool state);

	private:
		uint8_t pin;
		uint8_t last;
		buttonMode_t modus = PUSH;
	};

/*******************************************************************************
 * 3rd button control class
 ******************************************************************************/

/**
 * @brief 3rd button control handler object
 * 
 */
class Control3rd {
	public:
		/**
		 * @brief Construct a new 3rd button control object
		 * 
		 * @param pin button pin, not needed for virtual devices
		 * @param mode PUSH or TOGGLE mode
	 */
		Control3rd(uint8_t pin);
		Control3rd();

		/**
		 * @brief Set the mode of the 3rd button control
		 * 
		 * @param modus PUSH or TOGGLE, default PUSH
		 */
		void mode(buttonMode_t modus);

		/**
		 * @brief Get the state of the 3rd button control
		 * 
		 * @return true if pressed or in toggle
		 * @return false if not pressed
		 */
		bool state();

		/**
		 * @brief Update the state of the 3rd button control, must done in the while() loop
		 * 
		 * @param state optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool state);

	private:
		uint8_t pin;
		uint8_t last;
		buttonMode_t modus = PUSH;
	};

/*******************************************************************************
 * Button class
 ******************************************************************************/

/**
 * @brief Universal button object
 * 
 */
class Button {
	public:
		/**
		 * @brief Construct a new OSC button object
		 * 
		 * @param pin button pin, not needed for virtual devices
		 * @param type KEY, MACRO, IP, CP, FP, BP, PRESET, CHAN, GROUP, SUB, FX, PIXMAP, CURVE, SNAP, SCENE
		 * @param strng optional for KEY name and RAW message
		 * @param number optional number for MACRO
		 */
		Button(uint8_t pin, button_t type, string strng);
		Button(uint8_t pin, button_t type, uint16_t number);

		/**
		 * @brief Construct a new Osc Button object for vitual methods like I/O expanders and touchscreens
		 * 
		 * @param type RAW, KEY or MACRO
		 * @param strng strng optional for KEY name and RAW message
		 * @param number optional number for MACRO
		 */
		Button(button_t type, string strng);
		Button(button_t type, uint16_t number);

		/**
		 * @brief Update the state of the button, must done in the while() loop
		 * 
		 * @param state optional for virtual devices, TRUE if button press
		 */
		void update();
		void update2();
		void update(bool state);

	private:
		uint8_t pin;
		uint8_t last;
		string pattern;
	};

/*******************************************************************************
 * 2nd Button class
 ******************************************************************************/

/**
 * @brief universal 2nd button object for use with 2nd button control key
 * 
 */
class Button2nd {
	public:
		/**
		 * @brief Construct a new OSC button2nd object
		 * 
		 * @param pin button pin, not needed for virtual devices
		 * @param type RAW, KEY or MACRO
		 * @param strng optional for KEY name and RAW message
		 * @param number optional number for MACRO
		 */
		Button2nd(uint8_t pin, button_t type, string strng);
		Button2nd(uint8_t pin, button_t type, uint16_t number);

		/**
		 * @brief Construct a new Osc Button object for vitual methods like I/O expanders and touchscreens
		 * 
		 * @param type RAW, KEY or MACRO
		 * @param strng strng optional for KEY name and RAW message
		 * @param number optional number for MACRO
		 */
		Button2nd(button_t type, string strng);
		Button2nd(button_t type, uint16_t number);

		/**
		 * @brief Update the state of the OSC button, this must done in loop()
		 * 
		 * @param state optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool state);

	private:
		uint8_t pin;
		bool last;
		bool state;
		string pattern;
	};

/*******************************************************************************
 * 3rd Button class
 ******************************************************************************/

/**
 * @brief universal 2nd button object for use with 2nd button control key
 * 
 */
class Button3rd {
	public:
		/**
		 * @brief Construct a new OSC button2nd object
		 * 
		 * @param pin button pin, not needed for virtual devices
		 * @param type RAW, KEY or MACRO
		 * @param strng optional for KEY name and RAW message
		 * @param number optional number for MACRO
		 */
		Button3rd(uint8_t pin, button_t type, string strng);
		Button3rd(uint8_t pin, button_t type, uint16_t number);

		/**
		 * @brief Construct a new Osc Button object for vitual methods like I/O expanders and touchscreens
		 * 
		 * @param type RAW, KEY or MACRO
		 * @param strng strng optional for KEY name and RAW message
		 * @param number optional number for MACRO
		 */
		Button3rd(button_t type, string strng);
		Button3rd(button_t type, uint16_t number);

		/**
		 * @brief Update the state of the OSC button, this must done in loop()
		 * 
		 * @param state optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool state);

	private:
		uint8_t pin;
		bool last;
		bool state;
		string pattern;
	};

/*******************************************************************************
 * Wheel class
 ******************************************************************************/

/**
 * @brief Class definitions for Encoder controlling parameters by their current wheel number
 * 
 */
class Wheel {
	public:
		/**
		 * @brief Construct a new Encoder object
		 * 
		 * @param pinA pin A of the encoder, not needed for virtual devices
		 * @param pinB pin B of the encoder, not needed for virtual devices
		 * @param dir the direction for the wheel can be FORWARD or REVERSE
		 */
		Wheel(uint8_t pinA, uint8_t pinB, direction_t direction = FORWARD);
		Wheel(direction_t direction = FORWARD);

		/**
		 * @brief Set the wheel number which should controlled by the encoder
		 * 
		 * @param param parameter name as a String
		 */
		void wheelNumber(uint8_t wheel);

		/**
		 * @brief Parse for the parameter value
		 * 
		 * @return true parameter match
		 * @return false no parameter data
		 */
		bool parse();

		/**
		 * @brief Get the parameter assigned from the wheel
		 * 
		 * @return String 
		 */
		string parameter();

		/**
		 * @brief Get the parameter value
		 * 
		 * @return float 
		 */
		float value();

		/**
		 * @brief Test if the value is activ
		 * 
		 * @return true if parameter is active
		 * @return false if parameter is inactiv
		 */
		bool activ();

		/**
		 * @brief Update the output of the Encoder, must be in the loop()
		 * 
		 * @param stateA optional for virtual devices, TRUE for A click
		 * @param stateB optional for virtual devices, TRUE for B click
		 * @param stateButton optional for virtual devices, TRUE for button press
		 * @param motion optional for virtual devices, can < 0 for left encoder turnand  > 0 right encoder turn
		 */
		void update();
		void update(bool stateA, bool stateB);
		void update(int32_t motion); // this must handled directly

	private:
		uint8_t wheel;
		uint8_t pinA;
		uint8_t pinB;
		uint8_t pinALast;
		uint8_t pinACurrent;
		int32_t encoderMotion;
		int32_t motionLast;
		direction_t direction;
		bool act;
		float val;
		string param;
	};

/*******************************************************************************
 * Encoder class
 ******************************************************************************/

/**
 * @brief Class definitions for Encoder controlling parameters by their name
 * 
 */
class Encoder {
	public:
		/**
		 * @brief Construct a new Encoder object
		 * 
		 * @param pinA pin A of the encoder, not needed for virtual devices
		 * @param pinB pin B of the encoder, not needed for virtual devices
		 * @param direction the direction for the wheel can be FORWARD or REVERSE
		 */
		Encoder(uint8_t pinA, uint8_t pinB, direction_t direction = FORWARD);
		Encoder(direction_t direction = FORWARD);

		/**
		 * @brief set the parameter which should controlled by the encoder
		 * 
		 * @param param parameter name as a string
		 */
		void parameter(string param, string alias = "");

		/**
		 * @brief get the parameter which is controlled by the encoder
		 * 
		 * @return String parameter
		 */
		string parameter();

		/**
		 * @brief get the parameter alias name which is controlled by the encoder
		 * 
		 * @return String alias parameter name
		 */
		string alias();

		/**
		 * @brief Parse for the parameter value
		 * 
		 * @return true parameter match
		 * @return false no parameter data
		 */
		bool parse();

		/**
		 * @brief Get the parameter value
		 * 
		 * @return float 
		 */
		float value();

		/**
		 * @brief Test if the value is activ
		 * 
		 * @return true if parameter is active
		 * @return false if parameter is inactiv
		 */
		bool active();

		/**
		 * @brief update the output of the encoder, must be in the loop()
		 * 
		 * @param stateA optional for virtual devices, TRUE for A click
		 * @param stateB optional for virtual devices, TRUE for B click
		 * @param stateButton optional for virtual devices, TRUE for button press
		 * @param motion optional for virtual devices, can < 0 for left encoder turnand  > 0 right encoder turn
		 */
		void update();
		void update(bool stateA, bool stateB);
		void update(int32_t motion); // this must handled directly
	
	private:
		string param;
		string paramAlias;
		uint8_t pinA;
		uint8_t pinB;
		uint8_t lastA;
		uint8_t currentA;
		uint8_t buttonPinLast;
		direction_t direction;
		int32_t encoderMotion;
		int32_t motionLast;
		bool act;
		uint16_t wheel;
		float val;
	};

/*******************************************************************************
 * Absolute levels class
 ******************************************************************************/

/**
 * @brief Absolute levels functions
 * 
 */
class AbsoluteLevels {
	public:
		/**
		 * @brief Construct a new AbsolutLevels object
		 * 
		 * @param pin button pin, not needed for virtual devices
		 * @param function parameter function PARAMETER, FULL, HOME, MIN, MAX, LEVEL, MINUS, PLUS
		 * @param function2nd second parameter function PARAMETER, FULL, HOME, MIN, MAX, LEVEL, MINUS, PLUS
		 */
		AbsoluteLevels(uint8_t pin, levels_t function = HOME, levels_t function2nd = PARAMETER);
		AbsoluteLevels(levels_t function = HOME, levels_t function2nd = PARAMETER);

		/**
		 * @brief Set the parameter which function should controlled
		 * 
		 * @param param parameter name as a String
		 */
		void parameter(string param);

		/**
		 * @brief Update the state of the button, must done in  loop()
		 * 
		 * @param state optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool state);

	private:
		uint8_t pin;
		levels_t function;
		levels_t function2nd;
  	uint8_t last;
		bool stateLast;
		string param;
		string patternFunction;
		string patternFunction2nd;
	};

/*******************************************************************************
 * Direct select class
 ******************************************************************************/

/**
 * @brief Button object for direct selects, initDS() must done before use
 * 
 */
class DS {
	public:
		/**
		 * @brief Construct a new DS object
		 * 
		 * @param pin DS button pin, not needed for virtual devices
		 * @param number DS button number
		 * @param bank DS bank
		 */
		DS(uint8_t pin, uint8_t number, uint8_t index = 1);
		DS(uint8_t number, uint8_t index = 1);

		/**
		 * @brief Update the button state
		 * 
		 * @param state DS button state, optional for virtual input, TRUE if button press
		 */
		void update();
		void update(bool state);

	private:
		uint8_t pin;
		uint8_t last;
		uint8_t number;
		uint8_t index;
		string pattern;
	};


/*******************************************************************************
 * Direct select handle class
 ******************************************************************************/

class DSTool {
	public:
		
		/**
		 * @brief Construct a new DSTool object
		 * 
		 * @param pinUp pin of the Up button, not needed for virtual devices
		 * @param pinDown pin of the Down button, not needed for virtual devices
		 * @param index 
		 */
		DSTool(uint8_t pinUp, uint8_t pinDown);
		DSTool();

		/**
		 * @brief Add an additional flexi button
		 * 
		 * @param pinFlexi pin of the Flexi button, not needed for virtual devices
		 */
		void flexiPin(uint8_t pinFlexi);

		/**
		 * @brief Initialise a direct select bank, this must done after an established connection
		 * 
		 * @param type button type 
		 * @param count 
		 * @param page 
		 * @param index 
		 * @param flexi 
		 */
		void init(button_t type, uint8_t count, uint8_t index = 1);

		/**
		 * @brief Set DS button type
		 * 
		 * @param type 
		 */
		void typeDS(button_t type);

		/**
		 * @brief Return the type name of the DS bank given by EOS
		 * 
		 * @return string 
		 */
		string typeDS();

		/**
		 * @brief Return the current page number
		 * 
		 * @return uint8_t page number
		 */
		uint16_t page();

		/**
		 * @brief Get the current flexi state
		 * 
		 * @return true 
		 * @return false 
		 */
		bool flexi();

		/**
		 * @brief Parse for DS feedback, must done after receiveOSC()
		 * 
		 * @return number of the parsed DS button, 0 if no data, -1 if page data
		 */
		int8_t parse();

		/**
		 * @brief Return the label of the DS button
		 * 
		 * @param number of the DS button
		 * @return string button name
		 */
		string label(uint8_t number); // TODO without number

		/**
		 * @brief Return the number of the DS
		 * 
		 * @param number of the DS button
		 * @return string of the DS number
		 */
		string number(uint8_t number);

		/**
		 * @brief Check for updated page, must be in the loop()
		 * 
		 * @param upState up button state, optional for virtual input, TRUE if button press
		 * @param downState down button state, optional for virtual input, TRUE if button press
		 * @param flexiState flexi button state, optional for virtual input, TRUE if button press
		 */
		void update();
		void update(bool stateUp, bool stateDown, bool stateFlexi);

	private:
		uint8_t pinUp;
		uint8_t pinUpLast;
		uint8_t pinDown;
		uint8_t pinDownLast;
		uint8_t pinFlexi;
		uint8_t pinFlexiLast;
		uint8_t count;
		uint8_t index = 1;
		button_t type;
		uint16_t currentPage = 1;
		bool flexiState = false;
		uint16_t currentPageLast[12] = {1}; // size
		bool flexiStateLast[12] = {false};
		string typeName;
		string patternUp;
		string patternDown;
		string patternSearchButton;
		string patternSearchPage;
		struct DSData {
			string number;
			string label;
			};
		struct DSData* dsData;
	};


/*******************************************************************************
 * Submaster class
 ******************************************************************************/

/**
 * @brief Submaster object
 * 
 */
class Submaster {
	public:
		/**
		 * @brief Construct a new submaster object
		 * 
		 * @param analogPin fader leveler pin, not needed for virtual devices
		 * @param sub number of the submaster
		 */
		Submaster(uint8_t analogPin, uint16_t sub);
		Submaster(uint16_t sub);

		/**
		 * @brief Setup a fire button
		 * 
		 * @param firePin not needed for virtual devices
		 */
		void fireButton(uint8_t firePin);
		void fireButton();

		/**
		 * @brief Add an optional callback function, the callback is triggered when value changed
		 * 
		 * @param callback 
		 */
		void callback(cbptr call);

		/**
		 * @brief Return the fader value
		 * 
		 * @return uint8_t fader value 0...100
		 */
		uint8_t value();

		/**
		 * @brief Update the state of the Key button, must in loop()
		 * 
		 */
		void update();

		/**
		 * @brief Update the virtual analog input
		 * 
		 * @param analog input 10 bits
		 */
		void update(int analog, bool fireState = false);

		/**
		 * @brief Update the virtual value input
		 * 
		 * @param value input 0...100
		 */
		void updateValue(uint8_t value, bool fireState = false);
	
	private:
		uint16_t sub;
		uint8_t val;
		uint8_t firePin = NO_PIN;
		uint8_t fireLast;
		uint8_t analogPin = NO_PIN;
		int16_t analogLast;
		uint8_t valLast;
		uint32_t updateTime;
		string patternSub;
		string patternFire;
		cbptr call = nullptr;
	};


/*******************************************************************************
 * Fader class
 ******************************************************************************/

/**
 * @brief Fader object
 * 
 */
class Fader {
	public:
		/**
		 * @brief Construct a new Fader object
		 * 
		 * @param analogPin fader leveler pin
		 * @param fader number of the fader inside the bank
		 * @param bank number of the OSC fader bank, default is 1
		 */
		Fader(uint8_t analogPin, uint8_t fader, uint8_t index = 1);
		Fader(uint8_t fader, uint8_t index = 1);

		/**
		 * @brief Setup a fire button
		 * 
		 * @param firePin 
		 */
		void fireButton(uint8_t firePin);
		void fireButton();

		/**
		 * @brief Setup a stop button
		 * 
		 * @param stopPin 
		 */
		void stopButton(uint8_t stopPin);
		void stopButton();

		/**
		 * @brief Setup a load button
		 * 
		 * @param loadPin 
		 */
		void loadButton(uint8_t loadPin);
		void loadButton();

		/**
		 * @brief Add an optional callback function, the callback is triggered when value changed
		 * 
		 * @param callback 
		 */
		void callback(cbptr call);

		/**
		 * @brief Returns the actual fader value
		 * 
		 * @return uint8_t fader value in %
		 */
		uint8_t value();

		/**
		 * @brief Stop sending OSC values until the given value is fetched
		 * 
		 * @param value fetch value in %
		 */
		void fetch(uint8_t value);

		/**
		 * @brief Helper for fetch
		 * 
		 * @param delta range of fetch jitter in %
		 */
		void jitter(uint8_t delta);

		/**
		 * @brief Return the lock state
		 * 
		 * @return true OSC locked
		 * @return false OSC unlocked
		 */
		bool lock();

		/**
		 * @brief Set the lock state
		 * 
		 * @param state 
		 */
		void lock(bool state);

		/**
		 * @brief Update the state of the fader and fire/stop/load buttons, must in while() loop
		 * 
		 */
		void update();

		/**
		 * @brief Update the state of the fader and fire/stop/load buttons, must in while() loop
		 * @brief For use with virtual inputs
		 * 
		 * @param value virtual input 10 bits
		 */
		void update(int value);

	private:
		uint8_t analogPin = NO_PIN;
		uint8_t fader;
		uint8_t index;
		int16_t analogLast;
		int16_t valLast;
		uint8_t val;
		bool lockState = false;
		uint8_t fetchValue;
		uint8_t delta;
		uint32_t updateTime;
		uint8_t firePin = NO_PIN;
		uint8_t fireLast;
		uint8_t stopPin = NO_PIN;
		uint8_t stopLast;
		uint8_t loadPin = NO_PIN;
		uint8_t loadLast;
		string patternFader;
		string patternFire;
		string patternStop;
		string patternLoad;
		cbptr call = nullptr;
	};


/*******************************************************************************
 * Fader handling class
 ******************************************************************************/

/**
 * @brief Class for helpers, control and data handling for faders
 * 
 */
class FaderTool {
	public:
		/**
		 * @brief Construct a new Fader Tool object
		 * 
		 * @param pinUp pin of the Up button, not needed for virtual devices
		 * @param pinDown pin of the Up button, not needed for virtual devices
		 */
		FaderTool(uint8_t pinUp, uint8_t pinDown);
		FaderTool();

/**
		 * @brief Initialise a fader bank
		 * 
		 * @param index fader bank, default 1
		 * @param faders number of faders, default 10
		 */
		void init(uint8_t faders = 10, uint8_t index = 1);

		/**
		 * @brief Parse for Fader feedback, must done after receiveOSC()
		 * 
		 * @return int8_t number of actual parsed fader, -1 if page number, 0 no data
		 */
		int8_t parse();

		/**
		 * @brief Get the console value of a fader, needed for synchronize with console
		 * 
		 * @return int8_t number of actual parsed fader value, 0 no data
		 */
		int8_t parseValue();

		/**
		 * @brief Return the current page number
		 * 
		 * @return uint8_t page number
		 */
		uint8_t page();

		/**
		 * @brief Return the value of a fader
		 * 
		 * @param fader number
		 * @return uint8_t value 0 ... 100
		 */
		uint8_t value(uint8_t fader);

		/**
		 * @brief Return the label of the fader
		 * 
		 * @param fader number
		 * @return const char* fader name
		 */
		string label(uint8_t fader);

		/**
		 * @brief Get the type of the fader
		 * 
		 * @param fader number
		 * @return fader_t e.g. S (Submaster) ...
		 */
		fader_t type(uint8_t fader);

	/**
		 * @brief Get the type of the fader
		 * 
		 * @param fader number
		 * @return fader_t e.g. S (Submaster) ...
		 */
		string typeString(uint8_t fader);

		/**
		 * @brief Return the range minimum
		 * 
		 * @param fader number
		 * @return uint8_t minimum range, for GlobalFX this can > 0
		 */
		uint16_t rangeMin(uint8_t fader);

		/**
		 * @brief Return the range maximum
		 * 
		 * @param fader number
		 * @return uint8_t maximum range, for ManTime is ms eg 10000 for 10s
		 */
		uint16_t rangeMax(uint8_t fader);

		/**
		 * @brief Check for updated page, must be in the loop()
		 * 
		 */
		void update();
		void update(bool stateUp, bool stateDown);

	private:
		uint8_t pinUp;
		uint8_t pinDown;
		uint8_t pinUpLast;
		uint8_t pinDownLast;
		uint8_t index = 1;
		uint8_t faders = 10;
		uint8_t currentPage = 1;
		struct FaderData {
			fader_t faderType;
			uint8_t value;
			uint16_t typeNumber;
			int32_t min;
			int32_t max;
			string label;
			};
		struct FaderData* faderData;
		cbptr call = nullptr;
		string patternUp;
		string patternDown;
		string patternSearchName;
		string patternSearchRange;
		string patternSearchValue;
		string patternSearchPage;
	};

/*******************************************************************************
 * Parameter list handling class
 ******************************************************************************/

/**
 * @brief Class for control the parameter selection
 * 
 */
class SelectParameter {
	public:
		/**
		 * @brief Construct a new Select Parameter object
		 * 
		 * @param pinUp button for increasing the index, not needed for virtual devices
		 * @param pinDown button for decreasing the index, not needed for virtual devices
		 * @param encoders number of encoders in use
		 */
		SelectParameter(uint8_t pinUp, uint8_t pinDown, uint8_t encoders);
		SelectParameter(uint8_t encoders);

		/**
		 * @brief Set the name of a parameter by index
		 * 
		 * @param name parameter name
		 * @param alias parameter alias name
		 */
		void parameter(string parameter, string alias = "");

		/**
		 * @brief Add an optional callback function, the callback is triggered when page is changed
		 * 
		 * @param callback pointer to the callback function 
		 */
		void callback(cbptr callback);

		/**
		 * @brief Parse the incoming OSC message for value updates use subsription()
		 * 
		 * @return uint8_t returns the encoder number to update the value, 0 if there is no visible update
		 */
		uint8_t parse();

		/**
		 * @brief Get the name of a parameter by encoder
		 * 
		 * @param encoder number of the encoder
		 * @return string parameter name
		 */
		string parameter(uint8_t encoder);

		/**
		 * @brief Get the alias name of a parameter by encoder
		 * 
		 * @param encoder number of the encoder
		 * @return string parameter name
		 */
		string alias(uint8_t encoder);

		/**
		 * @brief Get the value of a parameter by encoder
		 * 
		 * @param encoder number of the encoder
		 * @return float value of the parameter
		 */
		float value(uint8_t encoder);

		/**
		 * @brief Get the active state of the parameter by encoder
		 * 
		 * @param encoder number of the encoder
		 * @return true if parameter is active
		 * @return false if parameter is inactive
		 */
		bool active(uint8_t encoder);

		/**
		 * @brief Get the wheel number by encoder
		 * 
		 * @param encoder number of the encoder
		 * @return int wheel number
		 */
		int wheel(uint8_t encoder);

		/**
		 * @brief Get the number of pages available
		 * 
		 * @return uint8_t pages
		 */
		uint8_t pages();

		/**
		 * @brief Get the actual page number
		 * 
		 * @return uint8_t page number
		 */
		uint8_t page();

		/**
		 * @brief Check for updated selection, must be in the loop()
		 * 
		 * @param stateUp optional for virtual devices, TRUE if button press
		 * @param stateDown optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool stateUp, bool stateDown);

	private:
		void indexEncoder();
		uint8_t pinUp;
		uint8_t pinUpLast;
		uint8_t pinDown;
		uint8_t pinDownLast;
		uint8_t encoders;
		int parameterCount;
		int *idx;
		struct Wheel {
			bool active;
			int wheel;
			float value;
			string parameter;
			string alias;
			};
		vector<Wheel> param;
		uint8_t pagesCount;
		uint8_t currentPage;
		cbptr call = nullptr;
	};

/*******************************************************************************
 * Parameter category list handling class
 ******************************************************************************/

/**
 * @brief Class for control the parameter selection
 * 
 */
class SelectCategory {
	public:
		/**
		 * @brief Construct a new category selection object
		 * 
		 * @param pinIntens pin of the Intens button, not needed for virtual devices
		 * @param pinFocus pin of the Focus button, not needed for virtual devices
		 * @param pinColor pin of the Color button, not needed for virtual devices
		 * @param pinImage pin of the Image button, not needed for virtual devices
		 * @param pinForm pin of the Form button, not needed for virtual devices
		 * @param pinShutter pin of the Shutter button, not needed for virtual devices
		 * @param encoders number of encoders in use
		 */
		SelectCategory(uint8_t pinIntens, uint8_t pinFocus, uint8_t pinColor, uint8_t pinImage, uint8_t pinForm, uint8_t pinShutter, uint8_t encoders);
		SelectCategory( uint8_t encoders);

		/**
		 * @brief Set the name of a parameter by index
		 * 
		 * @param name parameter name
		 * @param alias parameter alias name
		 */
		void parameter(category_t category, string parameter, string alias = "");

		/**
		 * @brief Add an optional callback function, the callback is triggered when page is changed
		 * 
		 * @param callback pointer to the callback function 
		 */
		void callback(cbptr callback);

		/**
		 * @brief Parse the incoming OSC message for value updates use subsription()
		 * 
		 * @return uint8_t returns the encoder number to update the value, 0 if there is no visible update
		 */
		uint8_t parse();

		/**
		 * @brief Get the name of a parameter by encoder
		 * 
		 * @param encoder number of the encoder
		 * @return string parameter name
		 */
		string parameter(uint8_t encoder);

		/**
		 * @brief Get the alias name of a parameter by encoder
		 * 
		 * @param encoder number of the encoder
		 * @return string parameter name
		 */
		string alias(uint8_t encoder);

		/**
		 * @brief Get the value of a parameter by encoder
		 * 
		 * @param encoder
		 * @return float 
		 */
		float value(uint8_t encoder);

		/**
		 * @brief Get the active state of the parameter
		 * 
		 * @return true if parameter is active
		 * @return false if parameter is inactive
		 */
		bool active(uint8_t encoder);

		/**
		 * @brief Get the number of pages available
		 * 
		 * @return uint8_t pages
		 */
		uint8_t pages();

		/**
		 * @brief Get the actual page number
		 * 
		 * @return uint8_t page number
		 */
		uint8_t page();

		/**
		 * @brief Get curent page of a category
		 * 
		 * @param category 
		 * @return uint8_t cureent page
		 */
		uint8_t page(category_t category);

		/**
		 * @brief Get the number of pages of a category
		 * 
		 * @param category 
		 * @return uint8_t pages
		 */
		uint8_t pages(category_t category);

		/**
		 * @brief Get the number of items of a category
		 * 
		 * @param category 
		 * @return uint8_t items count
		 */
		uint8_t count(category_t category);

		/**
		 * @brief Get the current category
		 * 
		 * @return category_t 
		 */
		category_t category();

		/**
		 * @brief Get the current category as a string
		 * 
		 * @return const char* category as a string
		 */
		string categoryName();

		/**
		 * @brief check for updated selection, must done in loop()
		 * 
		 * @param stateIntens optional for virtual devices, TRUE if button press
		 * @param stateFocus optional for virtual devices, TRUE if button press
		 * @param stateColor optional for virtual devices, TRUE if button press
		 * @param stateImage optional for virtual devices, TRUE if button press
		 * @param stateForm optional for virtual devices, TRUE if button press
		 * @param stateShutter optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool stateIntens, bool stateFocus, bool stateColor, bool stateImage, bool stateForm, bool stateShutter);
		
		/**
		 * @brief force an update for a specific catagory, this should not done in loop()
		 * 
		 * @param category 
		 */
		void update(category_t category);

	private:
		void indexEncoder(category_t category);
		uint8_t pins[6];
		uint8_t pinsLast[6];
		uint8_t encoders;
		category_t currentCategory = INTENSITY;
		category_t lastCategory = INTENSITY;
		int *idx;
		struct Index {
			int parameters;
			int pages;
			int currentPage;
			};
		struct Index categoryData[6];
		struct Wheel {
			bool active;
			int wheel;
			float value;
			string parameter;
			string alias;
			};
		vector<Wheel> param[6];
		cbptr call = nullptr;
	};

/*******************************************************************************
 * Parameter category dynamic handling class
 ******************************************************************************/

/**
 * @brief Class for control the parameter selection
 * 
 */
class SelectDynamic {
	public:
		/**
		 * @brief Construct a new category selection object
		 * 
		 * @param pinIntens pin of the Intens button, not needed for virtual devices
		 * @param pinFocus pin of the Focus button, not needed for virtual devices
		 * @param pinColor pin of the Color button, not needed for virtual devices
		 * @param pinImage pin of the Image button, not needed for virtual devices
		 * @param pinForm pin of the Form button, not needed for virtual devices
		 * @param pinShutter pin of the Shutter button, not needed for virtual devices
		 * @param encoders number of encoders in use
		 */
		SelectDynamic(uint8_t pinIntens, uint8_t pinFocus, uint8_t pinColor, uint8_t pinImage, uint8_t pinForm, uint8_t pinShutter, uint8_t encoders);
		SelectDynamic(uint8_t encoders);

		/**
		 * @brief Get the name of a parameter by encoder
		 * 
		 * @param encoder 
		 * @return const char* 
		 */
		string parameter(uint8_t encoder);

		/**
		 * @brief Add an alias for parameter names
		 * 
		 * @param parameter name to replace
		 * @param alias new alais name
		 */
		void alias(string parameter, string alias);

		/**
		 * @brief Get the information if there are data on a specific encoder wheel
		 * 
		 * @param encoder 
		 * @return true if the encoder have values
		 * @return false if not
		 */
		bool active(uint8_t encoder); // if (idx[encoder] == -1) return false else true;

		/**
		 * @brief Get the wheel number by encoder
		 * 
		 * @return uint8_t wheel number
		 */
		uint8_t wheel(uint8_t encoder);

		/**
		 * @brief Get the value of a parameter by encoder
		 * 
		 * @param encoder
		 * @return float 
		 */
		float value(uint8_t encoder);

		/**
		 * @brief Add an optinal callback function, the callback is triggered when page or category is changed
		 * 
		 * @param callback 
		 */
		void callback(cbptr call);

		/**
		 * @brief Parse the incoming OSC message for value updates
		 * 
		 * @return uint8_t returns the encoder for update the value, 0 if there is no visible update
		 */
		uint8_t parse();

		/**
		 * @brief Get the current category
		 * 
		 * @return category_t 
		 */
		category_t category();

		/**
		 * @brief Get the current category as a string
		 * 
		 * @return const char* category as a string
		 */
		string categoryName();

		/**
		 * @brief Get the current page of the selected category
		 * 
		 * @return uint8_t current page number
		 */
		uint8_t page();

		/**
		 * @brief Get the number of pages of the selected category
		 * 
		 * @return uint8_t number of pages for the current category
		 */
		uint8_t pages();

		/**
		 * @brief Get curent page of a category
		 * 
		 * @param category 
		 * @return uint8_t cureent page
		 */
		uint8_t page(category_t category);

		/**
		 * @brief Get the number of pages of a category
		 * 
		 * @param category 
		 * @return uint8_t pages
		 */
		uint8_t pages(category_t category);

		/**
		 * @brief Get the number of items of a cateegory
		 * 
		 * @param category 
		 * @return uint8_t items count
		 */
		uint8_t count(category_t category);

		/**
		 * @brief check for updated selection
		 * 
		 * @param stateIntens optional for virtual devices, TRUE if button press
		 * @param stateFocus optional for virtual devices, TRUE if button press
		 * @param stateColor optional for virtual devices, TRUE if button press
		 * @param stateImage optional for virtual devices, TRUE if button press
		 * @param stateForm optional for virtual devices, TRUE if button press
		 * @param stateShutter optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool stateIntens, bool stateFocus, bool stateColor, bool stateImage, bool stateForm, bool stateShutter);

		/**
		 * @brief force an update for a specific catagory, this should not done in loop()
		 * 
		 * @param category 
		 */
		void update(category_t category);

	private:
		void indexWheel();
		void indexEncoder(category_t category);
		void collect();
		void indexCollect();
		uint8_t pins[6];
		uint8_t pinsLast[6];
		uint8_t encoders;
		int *idx;
		category_t currentCategory = INTENSITY;
		category_t lastCategory = INTENSITY;
		bool markFlag;
		uint32_t collectTime;
		string select;
		string selectLast;
		struct Index {
			uint8_t parameters;
			uint8_t start;
			uint8_t end;
			uint8_t pages;
			uint8_t currentPage;
			};
		struct Index categoryData[6];
		struct Wheel {
			uint8_t category;
			float value;
			string parameter;
			};
		vector<Wheel> param;
		// TODO fill list
		vector<vector<string>> aliases;
			/*
			 = {
			{"Intensity Mode", "Int Mode"},
			{"Intensity Macros", "Macros"},
			{"Intensity Macro Rate", "MacroRate"},
			{"Intensity Macro Crossfade Rate", "MacroR X"},
			{"Position MSpeed", "Pos MSpd"},
			{"Position Blink", "Blink"},
			{"Color Select", "Color Sel"},
			{"Color Mix Mode", "Color MM"},
			{"Gobo Index/Speed", "Gobo I/S"},
			{"Gobo Select", "Gobo Sel"},
			{"Gobo Index/Speed 2", "Gobo2 I/S"},
			{"Gobo Select 2", "Gobo2 Sel"},
			{"Beam Fx Index/Speed", "BeamFx I/S"},
			{"Beam Fx Select", "BeamFx Sel"},
			{"Animation Select", "Anim Sel"},
			{"Effect Macros", "FX Macros"},
			{"Shutter Strobe", "Strobe"},
			{"Beam MSpeed", "Beam MSpd"},
			{"Edge Distance", "Edge Dist"},
			{"Global MSpeed", "G MSpeed"},
			{"LED Engine Effect Rate", "LedFX Rate"},
			{"LED Engine Effect Level", "LedFX Lev"},
			{"Light Diffusion", "Light Diff"},
			{"Frame Thrust A", "Frame A"},
			{"Frame Angle A", "Angle A"},
			{"Frame Thrust C", "Frame C"},
			{"Frame Angle C", "Angle C"},
			{"Frame Thrust B", "Frame B"},
			{"Frame Angle B", "Angle B"},
			{"Frame Thrust D", "Frame D"},
			{"Frame Angle D", "Angle D"},
			{"Frame Assembly", "Assembly"},
			{"", ""},
			};
			*/
		cbptr call = nullptr;
	};

/*******************************************************************************
 * Special parsers
 ******************************************************************************/

/**
 * @brief Class for parsing the softkey labels
 * 
 * 
 */
class Softkey {
	public:
		/**
		 * @brief Construct a new Softkey object
		 * 
		 */
		Softkey();

		/**
		 * @brief Parse for Softkeys, must done after receiveOSC()
		 * 
		 * @return uint8_t number of actual parsed softkey, return 0 if no data
		 */
		uint8_t parse();

		/**
		 * @brief Get the softkey label
		 * 
		 * @param sk softkey number
		 * @return string name of the softkey
		 */
		string label(uint8_t sk);

	private:
		string softkey[12];
	};

/**
 * @brief Class for parsing Pan/Tilt Min/Max values
 * 
 */
class PanTilt {
	public:
		/**
		 * @brief Construct a new Pan Tilt object
		 * 
		 */
		PanTilt();

		/**
		 * @brief Parse for Pan/Tilt vaues
		 * 
		 * @return true if there are pan/tilt data
		 * @return false if no data
		 */
		bool parse();

		/**
		 * @brief Get
		 * 
		 * @return true if parameters are active
		 * @return false if parameters are inactive
		 */
		bool active();

		/**
		 * @brief Get pan minimum data
		 * 
		 * @return float pan minimum valie
		 */
		float panMin();

		/**
		 * @brief Get pan maximum data
		 * 
		 * @return float pan maximum value
		 */
		float panMax();

		/**
		 * @brief Get pan value
		 * 
		 * @return float pan value
		 */
		float pan();

		/**
		 * @brief Get tilt minimum data
		 * 
		 * @return float tilt minimum value
		 */
		float tiltMin();

		/**
		 * @brief Get tilt maximum data
		 * 
		 * @return float tilt maximum value
		 */
		float tiltMax();

		/**
		 * @brief Get tilt value
		 * 
		 * @return float tilt value
		 */
		float tilt();

	private:
		bool activ;
		float panMinVal;
		float panMaxVal;
		float panVal;
		float tiltMinVal;
		float tiltMaxVal;
		float tiltVal;
	};

/**
 * @brief 
 * 
 */
class XYZ {
	public:
		/**
		 * @brief Construct a new XYZ object
		 * 
		 */
		XYZ();

		/**
		 * @brief Parse for XYZ data
		 * 
		 * @return true if there are xyz data
		 * @return false if no data
		 */
		bool parse();

		/**
		 * @brief Get
		 * 
		 * @return true if parameters are active
		 * @return false if parameters are inactive
		 */
		bool active();

		/**
		 * @brief Get X data
		 * 
		 * @return float x value
		 */
		float x();

		/**
		 * @brief Get Y data
		 * 
		 * @return float y value
		 */
		float y();

		/**
		 * @brief Get Z data
		 * 
		 * @return float z value
		 */
		float z();

	private:
		bool activ;
		float xVal;
		float yVal;
		float zVal;
	};

/**
 * @brief Class for parsing Hue / Saturation values
 * 
 */
class HueSat {
	public:
		/**
		 * @brief Construct a new Hue Sat object
		 * 
		 */
		HueSat();

		/**
		 * @brief Parse for Hue / Saturation values
		 * 
		 * @return true if there are hue / saturation data
		 * @return false if no data
		 */
		bool parse();

		/**
		 * @brief Get
		 * 
		 * @return true if parameters are active
		 * @return false if parameters are inactive
		 */
		bool active();

		/**
		 * @brief Get Hue data
		 * 
		 * @return float hue value
		 */
		float hue();

		/**
		 * @brief Get Saturation data
		 * 
		 * @return float saturation value
		 */
		float saturation();

		/**
		 * @brief Get color red data
		 * 
		 * @return uint8_t red [0...255]
		 */
		uint8_t red();

		/**
		 * @brief Get color green data
		 * 
		 * @return uint8_t green [0...255]
		 */
		uint8_t green();

		/**
		 * @brief Get color blue data
		 * 
		 * @return uint8_t blue [0...255]
		 */
		uint8_t blue();

		/**
		 * @brief Get color as 565 format, e.g. for Nextion Displays
		 * 
		 * @return uint16_t 565 color format
		 */
		uint16_t color565();

	private:
		void HStoRGB(float h, float s);
		bool activ;
		uint16_t col565(uint8_t red, uint8_t green, uint8_t blue);
		uint8_t redVal;
		uint8_t blueVal;
		uint8_t greenVal;
		uint16_t color565Val;
		float hueVal;
		float saturationVal;
	};

/**
 * @brief Class for parsing Channel data
 * 
 */
class Channel {
	public:
		/**
		 * @brief Construct a new Channel object
		 * 
		 */
		Channel();

		/**
		 * @brief Parse for Channel data
		 * 
		 * @return true if there are channel data
		 * @return false if no data
		 */
		bool parse();

		/**
		 * @brief Get the Channel string
		 * 
		 * @return string channel string
		 */
		string channel();

		/**
		 * @brief Get the Channel Selection string
		 * 
		 * @return string selection string
		 */
		string selection();

		/**
		 * @brief Get the Value as a string
		 * 
		 * @return string value string
		 */
		string value();

	/**
		 * @brief Get the Channel Type as a string
		 * 
		 * @return string type string
		 */
		string type();

	/**
		 * @brief Get the Channel Address as a string
		 * 
		 * @return string address string
		 */
		string address();

	private:
		string channelString;
		string select;
		string val;
		string typ;
		string dmx;
	};

/**
 * @brief Class for parsing the Command line
 * 
 * @return 
 */
class Command {
	public:
		/**
		 * @brief Construct a new Command object
		 * 
		 */
		Command();

		/**
		 * @brief Parse for the Command Line
		 * 
		 * @return true if there is a commmand line
		 * @return false if no data
		 */
		bool parse();

		/**
		 * @brief Get the Command Line string
		 * 
		 * @return string command line string
		 */
		string command();

	private:
		string commandString;
	};

/**
 * @brief Class for parsing cue data
 * 
 */
class Cue {
	public:
		/**
		 * @brief Construct a new Cue object
		 * 
		 */
		Cue();

		/**
		 * @brief Parse for Cue data, must done after receiveOSC()
		 * 
		 * @return cue_t 
		 */
		cue_t parse();

		/**
		 * @brief Get the complete cue text
		 * 
		 * @param type of cue date ACTIVE / PENDING / PREVIOUS
		 * @return string cue string
		 */
		string cue(cue_t type);

		/**
		 * @brief Get the cue list number
		 * 
		 * @param type of cue date ACTIVE / PENDING / PREVIOUS
		 * @return string cuelist number as a string
		 */
		string cueList(cue_t type);

		/**
		 * @brief Get the cue number
		 * 
		 * @param type of cue date ACTIVE / PENDING / PREVIOUS
		 * @return string cue number as a string
		 */
		string cueNumber(cue_t type);

		/**
		 * @brief Get the cue label
		 * 
		 * @param type of cue date ACTIVE / PENDING / PREVIOUS
		 * @return string cue label
		 */
		string label(cue_t type);

		/**
		 * @brief Get the duration of the cue
		 * 
		 * @param type of cue date ACTIVE / PENDING / PREVIOUS
		 * @return string cue duration as a string
		 */
		string duration(cue_t type);

		/**
		 * @brief Get the progress of the cue
		 * 
		 * @param type of cue date ACTIVE / PENDING / PREVIOUS
		 * @return string cue progress as a string
		 */
		string progress(cue_t type);

	private:
		void parseData(uint8_t data);
		struct CueData {
			string cueText;
			string cueList;
			string cueNumber;
			string label;
			string duration;
			string progress;
			cue_t type;
  	};
		struct CueData cuedata[3];
	};

/**
 * @brief Class for parsing version data
 * 
 */
class Version {
	public:
		/**
		 * @brief Construct a new Version object
		 * 
		 */
		Version ();

		/**
		 * @brief Getter for version data
		 * 
		 */
		void version();

		/**
		 * @brief Get the software version as a string
		 * 
		 * @return string software version
		 */
		string software();

		/**
		 * @brief Get the library version as a string
		 * 
		 * @return string library version
		 */
		string library();

		/**
		 * @brief Parse for version
		 * 
		 * @return true if version data
		 * @return false no data
		 */
		bool parse();

	private:
		string softwareVersion;
		string libraryVersion;

	};

/**
 * @brief Class for getting the user
 * 
 */
class User {
	public:
		/**
		 * @brief Construct a new User object
		 * 
		 */
		User();

		/**
		 * @brief Parse for user number
		 * 
		 * @return true if user cahnged
		 * @return false if no data
		 */
		bool parse();

		/**
		 * @brief Get the user number
		 * 
		 * @return uint16_t user number
		 */
		uint16_t user();

	private:
		uint16_t userNumber;
};

/**
 * @brief Class for parsing the name of the showfile
 * 
 */
class Show {
	public:
		/**
		 * @brief Construct a new Show object
		 * 
		 */
		Show();

		/**
		 * @brief Parser for show name
		 * 
		 * @return true if there is a show name
		 * @return false if no data
		 */
		bool parse();

		/**
		 * @brief Get the show name
		 * 
		 * @return string show name
		 */
		string name();

	private:
		string showName;
	};

/**
 * @brief Class for parsing the Event state BLIND / LIVE
 * 
 */
class EventState {
	public:
		/**
		 * @brief Construct a new Event State object
		 * 
		 */
		EventState();

		/**
		 * @brief Parser for the event state
		 * 
		 * @return true if event state changed
		 * @return false if no data
		 */
		bool parse();

		/**
		 * @brief Get the event state as a string
		 * 
		 * @return string event state BLIND / LIVE
		 */
		string state();

	private:
		uint8_t eventState;
	};

/*******************************************************************************
 * Helpers for creating patterns
 ******************************************************************************/

/**
 * @brief Create a pattern based on button type and number
 * 
 * @param type MACRO, IP, CP, FP, BP, PRESET, CHAN, GROUP, SUB, FX, PIXMAP, CURVE, SNAP, SCENE, SC (Show Control)
 * @return string 
 */
string patternTypeNumber(button_t type, uint16_t number);

/**
 * @brief Create a pattern based on button type and string
 * 
 * @param type KEY, RAW
 * @param strng 
 * @return string 
 */
string patternTypeString(button_t type, string strng);

/**
 * @brief Create a DS pattern for initialization
 * 
 * @param type button type 
 * @param count number of DS buttons
 * @param index index number, default = 1
 * @param page page number, default = 1
 * @param flexi flexi state, default = false
 * @return string 
 */
string patternDS(button_t type, uint8_t count, uint8_t index = 1, uint16_t page = 1, bool flexi = false);

/**
 * @brief Create a fader pattern for initialization
 * 
 * @param page fader page of the console, default is 1
 * @param faders number of faders, standard is 10
 * @param bank number of the OSC fader bank, default is 1
 * @return string 
 */
string patternFader(uint8_t faders = 10, uint8_t index = 1, uint8_t page = 1);

/**
 * @brief Create a pattern for absolute functionality
 * 
 * @param function 
 * @param param 
 * @return string 
 */
string patternAbsolute(levels_t function, string param);

/*******************************************************************************
 * Helper for data conversion
 ******************************************************************************/

/**
 * @brief convert a float value to a string
 * 
 * @param float32 value to convert
 * @param digits default 3, common for parameter values
 * @return string result as string
 */
string ftos(float float32, uint8_t digits = 3);

/*******************************************************************************
 * OSC handling
 ******************************************************************************/

/**
 * @brief OSC class
 * 
 */
class OSC {
	public:
		typedef void (*cbptr)();

		/**
		 * @brief Construct a new OSC object
		 * 
		 */
		OSC();

		/**
		 * @brief Start communication with USB
		 * 
		 */
		void begin();

		/**
		 * @brief Start communication with Ethernet UDP
		 * 
		 * @param udp class object
		 * @param ip receiver IP
		 * @param portUdpTx transmit port number -> RX on receiver
		 * @param portUdpRx receive port number -> TX on receiver
		 */
		void begin(UDP &udp, IPAddress ip, uint16_t portUdpTx, uint16_t portUdpRx);

		/**
		 * @brief Start communication with Ethernet TCP Server as a Client
		 * 
		 * @param tcp client class object
		 * @param ip receiver IP
		 * @param portTcp server port number
		 */
		void begin(Client &tcp, IPAddress ip, uint16_t portTcp);

		/**
		 * @brief Create a simple message with no argument and send it
		 * 
		 * @param pattern OSC address
		 */
		void message(string pattern);

		/**
		 * @brief Create a simple message with one Int32 argument and send it
		 * 
		 * @param pattern OSC address
		 * @param int32 value
		 */
		void message(string pattern, int32_t int32);

		/**
		 * @brief Create a simple message with one Float argument and send it
		 * 
		 * @param pattern OSC address
		 * @param float32 value
		 */
		void message(string pattern, float float32);

		/**
		 * @brief Create a simple message with one String argument and send it
		 * 
		 * @param pattern OSC address
		 * @param strng String
		 */
		void message(string pattern, string strng);

		/**
		 * @brief Create a simple message with one MIDI argument and send it
		 * 
		 * @param pattern OSC address
		 * @param port MIDI port number
		 * @param status MIDI status byte
		 * @param data1 MIDI data 1 byte
		 * @param data2 MIDI data 2 byte
		 */
		void message(string pattern, uint8_t port, uint8_t status, uint8_t data1, uint8_t data2);

		/**
		 * @brief Create a simple message with one Timetag argument and send it
		 * 
		 * @param pattern OSC address
		 * @param timetag 64bit NTP time (32bit MSB seconds, 32bit LSB fractional)
		 */
		void message(string pattern, uint64_t timetag);

		/**
		 * @brief Send the OSC message
		 * 
		 */
		void send();

		/**
		 * @brief Receive an OSC message, must in 'loop()'
		 * 
		 * @return true if there is a new OSC message
		 * @return false no OSC message arrived
		 */
		bool receive();

		/**
		 * @brief Get the Pattern String
		 * 
		 * @return string Pattern String
		 */
		string getPattern();

		/**
		 * @brief Get the number of received Tags
		 * 
		 * @return int number of tags
		 */
		int getTagSize();

		/**
		 * @brief Get the Int32 data at given position x
		 * 
		 * @param pos position of the data, start with 1
		 * @return int32_t value
		 */
		int32_t getInt(uint8_t pos);

		/**
		 * @brief Get the Float data at given position x
		 * 
		 * @param pos position of the data, start with 1
		 * @return float value
		 */
		float getFloat(uint8_t pos);

		/**
		 * @brief Get the String data at given position x
		 * 
		 * @param pos position of the data, start with 1
		 * @return string String
		 */
		string getString(uint8_t pos);

		/**
		 * @brief Set an optional callback function if OSC message arrived
		 * 
		 * @param call callback function
		 */
		void callback(cbptr call);

		void sendHandshake();

	private:
		void parse();
		bool receiveUSB();
		bool receiveUDP();
		bool receiveTCP();
		void slipEncode();
		void fillZeros();
		IPAddress ip;
		uint16_t portUdpRx;
		uint16_t portUdpTx;
		UDP *udp;
		uint16_t portTcp;
		Client *tcp;
		cbptr call = nullptr;
		typedef enum Interface {
			OSCUSB,
			OSCUDP,
			OSCTCP,
			} interface_t;
		interface_t interfaceType;
		vector<uint8_t> bufferSend;
		string bufferReceive;
		struct MessageSend {
			vector<uint8_t> pattern;
			vector<uint8_t> tag;
			vector<uint8_t> data;
			} messageSend;
		struct MessageReceive {
			string pattern;
			string tag;
			int tagSize;
			struct Data {
				char tag;
				struct MIDI {
					uint8_t port;
					uint8_t status;
					uint8_t data1;
					uint8_t data2;
				} midi;
			int32_t int32;
			uint64_t timetag;
			string strng;
			float float32;
			};
			vector<Data> oscData;
		} messageReceive;
	};
	
#endif