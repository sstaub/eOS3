# eOS3 for ETC EOS Family (v3.1 and later) using OSC over USB and Ethernet/WLAN with UDP/TCP
This is a new Arduino Framework for EOS v3.x, it is written from scratch and replaces the old eOS library.<br>
Most improvements are

- Using my own OSC implementation
- 2nd/3rd layer for additional button functions
- Virtual inputs for external I/O and A/D expanders and touchscreens
- Fader page handling and parser
- DS page handling and parser
- Parsers for different data types
- New improved Button object
- Parameter control by simple list or categories (list and dynamic handler)
- Support for TCP
- Also many other changes and simplifications

eOS3 is an object orientated library for Arduino to control ETCs EOS Family Consoles (v3.1 and later) with OSC over USB or Ethernet UDP/TCP. The goal of the library is to have a smart toolbox to create your own boxes which covers your needing by endless combinations of hardware elements.
The library support hardware elements like encoders, fader, buttons with some helper functions. The library allows you to use hardware elements as an object and with the use of the helper functions, code becomes much easier to write and read and to understand.<br>
Please refer to the EOS manual for more information about OSC.<br>
If you have wishes for other functions or classes use the discussion tab. If you find bugs make an issue, nobody is perfect.

**!!! This library is licensed under CC BY-NC-SA 4.0 [https://creativecommons.org/licenses/by-nc-sa/4.0/](https://creativecommons.org/licenses/by-nc-sa/4.0/)<br>
For commercial use a license is needed !!!**

## eOS3
The new main class is `eOS3`.
You get additional new partners (general callbacks) for `setup()`and `loop()`,
- `maintain()`<br>
which is called when there is new OSC data
- `connected()`<br>
when a connected is established
- `disconnected()`<br>
when a connection failed

## Installation
1. Download from Releases
2. Follow the instruction on the Arduino website https://www.arduino.cc/en/Guide/Libraries
You can import the .zip file from the IDE with *Sketch / Include Library / Add .ZIP Library...*
3. For PlatformIO Unzip and move the folder to the lib folder of your project.

## Library
The library itself use the `std::string` and `std::vector` C++ library so only modern Microcontrollers which enough RAM and C++ Standard Libraries are supported<br>
Therefore you must use `.c_str()` functionality to convert it to conventional char arrays<br>
Recommanded libraries for your projects
- Ethernet
	- Ethernet3 https://github.com/sstaub/Ethernet3 for WIZnet 5500 chips with MAC support for Pico
	- WIZnet W55RP20 https://github.com/WIZnet-ioNIC/W55RP20-Ethernet3
	- QNEthernet https://github.com/ssilverman/QNEthernet for Teensy 4.1
- LCD Displays
	- HD44780 https://github.com/sstaub/LCD-HD44780
	- HD44780 with I2C https://github.com/sstaub/LCD-I2C-HD44780
- Touchscreen
- - NextionX2 https://github.com/sstaub/NextionX2 for Nextion Touchscreens
- I/O Expanders
	- A/D MCP3208 (SPI) https://github.com/sstaub/MCP_3208
	- I/O MCP23017 (I2C) https://github.com/sstaub/MCP_23017
- Adafruit Seesaw Endocder and Keys
  - https://github.com/sstaub/Seesaw

## IDE's

- VS Code with PlatformIO [https://platformio.org](https://platformio.org) is the a recommanded IDE
- As an alternative TODO
- VS Code with "Arduino Maker Workshop" Plugin https://marketplace.visualstudio.com/items?itemName=TheLastOutpostWorkshop.arduino-maker-workshop
- Arduino IDE 2.x [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)

## Boards

**!!! There is no support for AVR (UNO, MEGA) !!!**<br>
The main development platform is the Raspberry Pi Pico(2) based on the RP2040/RP2350 microcontroller.<br>
Only boards with native USB are supported, in the moment this is RaspeberryPI Pico(2) and Teensy (3.5 and 4.1),
these boards have also naitve `printf` support.<br>
From Arduino only the GIGA board is supported.<br>

**Pro of the Picos**

- cheap, starting at $4.- for the original, $3.- for clones with USB-C
- powerfull 133MHz Clock, Dual Core
- 264kB RAM, up to 16MB Flash
- WIZnet have many boards with Ethernet, the price is about 10.-$/€

**Contra**

- old style MicroUSB port, but there are many boards with USB C connector on the market e.g. from Adafruit.
- only 3 analog inputs, you should use an external AD converter e.g. MCP3208

It is recommended to use the Earl E. Philhower Core [https://github.com/earlephilhower/arduino-pico](https://github.com/earlephilhower/arduino-pico) instead of the mBed based Arduino core.

## Hardware

### Adafruit Seesaw

Support for Adafruits Seesaw I2C modules which makes hardware setup very simple.<br>
- Encoder https://www.adafruit.com/product/5880
- NeoKey 1x4 https://www.adafruit.com/product/4980

### Buttons and Encoders

Put a 100nF ceramic capacitor between GND and Pins of buttons and encoders (A/B).
This avoids spikes which can cause a double click.
Encoder type should EC11xxx from Bourns or ALPS, the are many variants on the market. They have different shaft sizes and types, rotation steps and also with an additional push button. So it is not easy to find the right one.

### Faders

The faders should a linear type with 10kOhm, from Bourns or ALPS and can be 45/60/80/100mm long. Put a 10nF ceramic capacitor between ground and fader leveler to prevent analog noise.

**Raspberry Pi Pico**
Use ADC_VREF instead of +3.3V to the top (single pin) of the fader (100%)<br>
Use AGND instead the normal GND to the center button pin (2 pins, the outer pin is normally for the leveler) of the fader (0%)

**TEENSY**
+3.3V to the top (single pin) of the fader (100%)<br>
Use ANALOG GND instead of the normal GND to the center button pin (2 pins, the outer pin is normally for the leveler) of the fader (0%)

## Ethernet Usage

This library and examples are a replacement for the #lighthack project, it allows the use of Ethernet/WLAN additional to USB so the library does not depend on the board type.
The in the examples used Ethernet library only supports the WIZnet 5500 chip, used on Ethernet Shield 2 or the popular USR-ES1 module.<br>
There is also a cheap board available directly from **WIZnet**, the W5500-EVB-Pico(2) which combines a Raspberry Pi Pico with a W5500 ethernet chip.
[https://www.wiznet.io/product-item/w5500-evb-pico/](https://www.wiznet.io/product-item/w5500-evb-pico/)

**Following libraries must downloaded for use with Ethernet<br>
!!! Beware, the Ethernet libraries have different init procedures !!!**

**WIZNet w5500 boards like Ethernet Shield 2**

- An Arduino compatible Ethernet library like Ethernet3 [https://github.com/sstaub/Ethernet3](https://github.com/sstaub/Ethernet3) or the Arduino Ethernet library [https://github.com/arduino-libraries/Ethernet](https://github.com/arduino-libraries/Ethernet)
- Optional for Teensy MAC address [https://github.com/sstaub/TeensyID](https://github.com/sstaub/TeensyID)
- WizNet W5500 a hard reset should performed to avoid connection problems with switches, if this can't done by board hardware. This must done before `Ethernet.begin()`
		- for Ethernet3 library use `Ethernet.hardreset()`
		- Arduino Ethernet library should do this with following code 

```cpp
void hardreset(uint8_t pinRST) {
	pinMode(pinRST, OUTPUT);
	digitalWrite(pinRST, HIGH);
	digitalWrite(pinRST, LOW);
	delay(1);
	digitalWrite(pinRST, HIGH);
	delay(150);
	}
```

**Teensy 4.1 with build in Ethernet**<br>

- [https://github.com/vjmuzik/NativeEthernet](https://github.com/vjmuzik/NativeEthernet)
- [https://github.com/vjmuzik/FNET](https://github.com/vjmuzik/FNET)

**STM32duino [https://github.com/stm32duino](https://github.com/stm32duino)**<br>
**!!! This works not as expected, there is a bug in the LWIP library which is causing a massive lost of packages**

- [https://github.com/stm32duino/STM32Ethernet](https://github.com/stm32duino/STM32Ethernet)
- [https://github.com/stm32duino/LwIP](https://github.com/stm32duino/LwIP)

## Connections
There are 3 interface available
- USB
- UDP
- TCP Slip over port 3037

## USB configuration and initialization
For USB connection add the simple interface function `init()` in ```setup()```

```cpp
#include "eOS3.h"
eOS3 eos;
void setup() {
	eos.init();
	}
```

## Ethernet configuration and initialization
The Ethernet functionality is independent from the hardware port (e.g. WIFI or other Ethernet hardware than WizNet W5500) and libraries. Behind the scenes it uses the virtual Arduino UDP/Client classes.

Before using Ethernet there a some things that must be done. It can be different between the diverse libraries.

1. Import the necessary #defines e.g.

```cpp
#include "Ethernet3.h"
```

2. You need to define IP addresses and ports 

- **mac** - You need a unique MAC address, for Teensy you can use the TeensyID library on this GitHub site,
for Pico a autogenerated MAC address is available inside Ethernet3 library
- **localIP** - You need a static IP address for your Arduino in the subnet range of network system
- **subnet** - A subnet range is necessary
- **eosIP** - This is the console IP address
- **localPort** - Optional, this is the destination port of your Arduino
- **eosPort** - Optional, this is the destination port of the EOS console

**Example**

```cpp
// configuration example, must done before setup()
uint8_t mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x14, 0x48};
IPAddress localIP(10, 101, 1, 201);
IPAddress subnet(255, 255, 0, 0);
uint16_t localPort = 8001; // on this port Arduino listen for data
// in EOS Setup > System > Show Control > OSC > OSC UDP TX Port
IPAddress eosIP(10, 101, 1, 100);
uint16_t eosPort = 8000; // on this port EOS listen for data
// in EOS Setup > System > Show Control > OSC > OSC UDP RX Port
```

1. Use the `init()` function in ```setup()```

- for UDP

```cpp
#include "Ethernet3.h"
#include "eOS3.h"
EthernetUDP udp;
eOS3 eos;
void setup () {
	Ethernet.begin(mac, localIP, subnet);
	eos.init(udp, eosIP, eosPort, localPort);
	eos.init(udp, eosIP); // for using default ports
	}
```

- for TCP

```cpp
#include "Ethernet3.h"
#include "eOS3.h"
EthernetClient tcp;
eOS3 eos;
void setup () {
	Ethernet.begin(mac, localIP, subnet);
	eos.init(tcp, eosIP);
	}
```

## Examples
[TODO]


# Documention eOS3 class basics

You must include the header file in the beginning of the sketch.<br>
`#include "eOS3.h`

## init() USB

Set the USB interface.

```cpp
void init();
```

**Example**

```cpp
#include "eOS3.h"
eOS3 eos;
void setup() {
	// ...
	eos.init(();
	// ...
	}
```

## interface() UDP
Set the UDP interface
```cpp
void intit(UDP &udp, IPAddress ip, uint16_t udpRxPort = 8000, uint16_t udpTxPort = 8001);
```
- **ip** the IP address of the console / Nomad
- **&udp** UDP interface
- **udpRxPort** EOS UDP RX receive port, default 8000
- **udpTxPort** EOS UDP TX transmit port, default 8001

**Example**
```cpp
#include "eOS3.h"
eOS3 eos;
void setup() {
	// ...
	eos.init(udp, eosIP, eosPort, localPort);
	eos.init(udp, eosIP); // for using default ports
	// ...
	}
```

## intit() TCP
Set the TCP interface on port 3037
```cpp
void init(Client &tcp, IPAddress ip);
```
- **ip** the IP address of the console / Nomad
- **&tcp** TCP interface

**Example**
```cpp
#include "eOS3.h"
eOS3 eos;
void setup() {
	// ...
	eos.init(eosIP, tcp);
	// ...
	}
```

## update()
Update the handlers for maintain(), connected() and disconnected(), must done in `loop()`
```cpp
void update();
```

**Example**
```cpp
#include "eOS3.h"
eOS3 eos;

void setup() {
	// ...
	}

void loop() {
	// ...
	eos.update();
	// ...
	}
```

## shift()



# eOS3 Helper functions

Small functions to improve workflow.

## filter()
With a Filter you get only messages from the console which you can use for proceeding informations.<br>
This should done in the ```connected()``` callback.
```cpp
void filter(string pattern[]);
```
- **pattern** filter pattern

**Example**
```cpp
eos.filter("/eos/out/param/*"); // only parameter messages will send to the endpoint
```

## filterRemove()
Remove a specific filter.
```cpp
void filterRemove(string pattern[]);
```
- **pattern** filter pattern

**Example**
```cpp
eos.filterRemove("/eos/out/param/*"); // only parameter messages will send to the endpoint
```

## filterClear()
Delete all added filters.
```cpp
void filterClear();
```

**Example**
```cpp
eos.filterClear();
```

## subscription()

With subscription you can get special informations about the dedicated parameters.

```cpp
void subscription(const char parameter[], int32_t subscribe = SUBSCRIBE);
```

- **parameter** parameter name you want to subscribe
- **subscribe** you can SUBCRIBE (default) or UNSUBSCRIBE a parameter

**Example**

```cpp
subscription("Pan"); // on Pan change EOS will send ```/eos/out/param/Pan=<value>```
```

## ping()

With a ping you can get a reaction from the console which helps you to identify your box and if is alive. You should send a ping regularly with message to identify your box on the console.

```cpp
void ping(); // send a ping without a message
void ping(const char msg[]); // send a ping with an additional message 
```

- **msg** additional message to send

**Example**

```cpp
ping("hello OSC"); // will send a ping with a message
```

## reset()

This function reset the OSC settings

```cpp
void reset();
```

**Example**

```cpp
reset();
```

## user()

This function allows you to change the user ID e.g.

```cpp
void user(int16_t userID);
```

- **userID** the ID of the user
		- **0** is the background user
		- **-1** is the current user
		- or any other user

**Example**

```cpp
user(2); // send messages to user 2
```

## shiftButton()

This function allows you to assign a hardware button as a **Shift** button. **Shift** set the encoder and wheel messages to the **Fine** mode or for opposite the acceleration of the **Intens** parameter. This must done in ```setup()```

```cpp
void shiftButton(uint8_t pin);
```

-**pin** number of the pin you want to use for SHIFT

**Example**

```cpp
void setup() {
	// ...
	shiftButton(4); // use pin 4 for the SHIFT button
	// ...
	}
```

## accelerationButton()

This function allows you to assign a hardware button as a **Accelaration** button. **Accelaration** set the encoder and wheel messages to a multiplier mode to accelerate the output. ```#define BUTTON_ACC 8``` controls the multiplier.

```cpp
void accelarationButton(uint8_t pin);
```

-**pin** number of the pin you want to use for Acceleration

**Example**

```cpp
void setup() {
	// ...
	shiftButton(5); // use pin 5 for the Acceleration button
	// ...
	}
```

# Classes

## OscButton

With this new universal class you can create generic buttons. In the moment following function types are available:
- RAW send a raw message (without arguments)
- KEY send a key press
- MACRO fires a macro

### Constructor

Create a new OSC button object. This should done before ```setup()```

```cpp
OscButton(uint8_t pin, button_t type, const char strng[]);
OscButton(uint8_t pin, button_t type, int number);
```

- **pin** the connection pin for the button hardware
- **type** the function type, RAW, KEY or MACRO
- **strng** optional RAW message or KEY name
- **number** optional for MACRO number

**Example**

```cpp
OscButton next(2, KEY, "Next"); // make a new osc button on Pin 2
OscButton last(3, KEY, "Last"); // make a new osc button on Pin 3
```

### Methods

#### osc2nd()

Add a second layer for an additional OSC button. Use SHIFT + button press.

```cpp
void osc2nd(button_t type, const char strng[]);
void osc2nd(button_t type, int number);
```

- **type** the function type, RAW, KEY or MACRO
- **strng** optional RAW message or KEY name
- **number** optional for MACRO number

This must done in ```setup()``` 

**Example**
```cpp
void setup() {
	// ...
	next.osc2nd(MACRO, 801); // SHIFT + Next button fires macro 801
	last.osc2nd(MACRO, 802); // SHIFT + Last button fires macro 802
	// ...
	}
```

#### update()

To get the current button state you must call inside the ```loop()```

```cpp
void update();
```

**Example**

```cpp
void loop() {
	// ...
	next.update();
	last.update();
	// ...
	}
```

## Encoder

This class allows to construct an encoder object to control parameter values.

### Constructor

The Encoder class creates an encoder object which allows to control parameters,
this should done before the ```setup()```

```cpp
Encoder(uint8_t pinA, uint8_t pinB, uint8_t direction = FORWARD);
```

- **pinA** and **pinB** are the connection Pins for the encoder hardware
- **direction** is used for changing the direction of the encoder to clockwise if pinA and pinB are swapped. The directions are FORWARD (standard) or REVERSE

**Example**

```cpp
Encoder encoder1(A0, A1, REVERSE);
```

### Methods

#### button()

If the Encoder have an extra push button, you can add it with following class method. This must done in ```setup()``` before assigning a parameter.
A button press post the parameter name to the command line. SHIFT + button press set the parameter to the HOME value.

```cpp
void button(uint8_t buttonPin);
```

- **buttonPin** is the pin for the encoder push button

**Example**

```cpp
void setup() {
	// ...
	encoder1.button(A1);
	// ...
	}
```

#### parameter()

Before using the encoder you must assign the parameter you want control. This should done in ```setup()```

```cpp
void parameter(const char param[]);
```

- **param** is the Parameter which you want assign

**Example **

```cpp
void setup() {
	// ...
	encoder1.parameter("Pan");
	// ...
	}
```

#### update()

To get the actual encoder state you must call inside the ```loop()```

```cpp
void update();
```

**Example**

```cpp
void loop() {
	// ...
	encoder1.update();
	// ...
	}
```

## SelectParameter

This class allows you to control a parameter list with an Up/Down button to step through. The buttons have a wrap behavior. 

### Constructor

Create parameter selection object. This should done before ```setup()```

```cpp
SelectParameter(uint8_t pinUp, uint8_t pinDown, uint8_t encoders, uint8_t parameters);
```

- **pinUp** pin for the button increasing the index
- **pinDown** pin for the button decreasing the index
- **encoders** number of the encoders you use in the application
- **parameters** number of the parameters in the list

**Example**

```cpp
SelectParameter selection(6, 7, 2, 9); 
// create an select parameter object with up button on pin 6, down button on pin 7, 
// using two encoders and a parameter list with 9 items
```

### Methods

#### osc2ndBtnUP() and osc2ndBtnDown()

Add a second layer for an additional OSC button. Use SHIFT + button press.

```cpp
void osc2ndBtnUp(button_t type, const char strng[]);
void osc2ndBtnUp(button_t type, int number);
void osc2ndBtnDown(button_t type, const char strng[]);
void osc2ndBtnDown(button_t type, int number);
```

- **type** the function type, RAW, KEY or MACRO
- **strng** optional RAW message or KEY name
- **number** optional for MACRO number

**Example**

```cpp
selection.osc2ndBtnDown(KEY, "Next");
selection.osc2ndBtnUp(KEY, "Last");
```

#### parameter()

Set the name of a parameter by index, this must done in ```setup()```

```cpp
void parameter(uint8_t index, const char name[]);
```

- **index** position of the parameter in the list, start with 1
- **name** name of the parameter, names must EOS parameter names

**Example**

```cpp
void setup() {
	// ...
	selection.parameter(1, "Intens");
	selection.parameter(2, "Iris");
	selection.parameter(3, "Edge");
	selection.parameter(4, "Zoom");
	selection.parameter(5, "Pan");
	selection.parameter(6, "Tilt");
	selection.parameter(7, "Red");
	selection.parameter(8, "Blue");
	selection.parameter(9, "Green");
	// ...
	}

#### callback()

Add a callback function, the callback is triggered when page is changed by the Up/Down buttons. The callback can used to update the display. This must done in ```setup()```

```cpp
void callback(cbptr callback);
```

- **callback** pointer to the callback function

**Example**

```cpp
void setup() {
	// ...
	selection.callback(updateEncoder);
	// the function updateEncoder() will called on page change.
	// ...
	}
```

#### parse()

This function parse the incoming OSC messages if there are parameter value updates. Returns the encoder number to update the value, 0 if there is no visible update<br>
It must used with receiveOSC() function in ```loop()```

```cpp
uint8_t parse();
```

**Example**

```cpp
void loop() {
	if (receiveOSC()) {
		if (selection.parse()) updateDisplay(); // does a display update 
		}
	}
```

#### parameter() return

Get the name of a parameter by encoder. This is used when making display updates.

```cpp
const char* parameter(uint8_t encoder);
```

- **encoder** number of the encoder

#### value()

Returns the value of an encoder. This is used when making display updates. 

```cpp
float value(uint8_t encoder);
```

- **encoder** number of the encoder

#### pages()

Return the number of all pages. 

```cpp
uint8_t pages();
```

**Example**

```cpp
uint8_t numPages = selection.pages();
```

#### page()

Return the actual selected page number. 

```cpp
uint8_t page();
```

**Example**

```cpp
uint8_t currentPage = selection.page();
```


#### update()

To get the actual state of the page number you must call inside the ```loop()```

```cpp
void update();
```

**Example**

```cpp
void loop() {
	// ...
	selection.update();
	// ...
	}
```

## Submaster

This class allows you to control a submaster with a hardware (slide) potentiometer as a fader and a bump button. See also the hardware advices above.

### Constructor

Create a new Submaster object. This should done before ```setup()```

```cpp
Submaster(uint8_t analogPin, uint16_t sub, uint8_t firePin = NO_PIN);
```

- **analogPin** are the connection Analog Pin for the fader leveler
- **sub** is the submaster number you want to control
- **firePin** is the Pin number for an optional bump button.

**Example**

```cpp
Submaster submaster1(A1, 1); // leveler is Analog Pin A1, submaster 1 controlled, no bump button
```

### Methods

#### update()

To get the actual button state you must call inside the ```loop()```

```CPP
void update();
```

**Example**

```cpp
void loop() {
	// ...
	submaster1.update();
	// ...
	}
```

## FaderTool

This class allows you configure the faders. It gives you fader page control functions and also parsing for fader informations.

### Constructor




## Fader

This class allows you to control a fader containing optional Fire/Stop/Load control buttons, all  functions configured in EOS Tab 36, with a hardware (slide) potentiometer as a fader and buttons.<br>
Before using Faders you must call **initFaders(page, faders, bank);**<br>
See also the hardware advices above.

### Initialise Faders

Before you can use the Fader class you must initialize them, this should done in ```setup()```

```cpp
void initFaders(uint8_t page = 1, uint8_t faders = 10, uint8_t bank = 1);
```

The **initFaders()** function is basic configuration and must use before you can use your Fader objects.
- **page** the fader page on your console
- **fader** is the number of fader on you console page
- **bank** is the virtuell OSC fader bank

**Example**

```
void setup() {
	// ...
	initFaders(); // without a parameter gives you a configuration for use your faders on page 1 of your console
	//...
	}
``` 

### Constructor

Create a new Fader object. This should done before ```setup()```

```cpp
Fader(uint8_t analogPin, uint8_t fader, uint8_t bank);
```
- **analogPin** are the connection Analog Pin for the fader leveler
- **fader** is the fader number of the fader page you want to control
- **bank** is the internal OSC bank number

**Example**

```cpp
Fader fader1(A1, 1, 1); // leveler is Analog Pin A1, fader number 1 is controlled, bank number is 1
```

### Methods

#### fireButton()

Add a button for an action on the GO button. This must done in ```setup()```

```cpp
void fireButton(uint8_t firePin);
```

- **firePin** is the Pin number for an additional fire button. Set to 0 if you don't need it.

**Example**

```
void setup() {
	// ...
	fader1.fireButton(2); // add a fire button on pin 2 
	//...
	}
``` 

#### stopButton()

Add a button for an action on the BACK button. This must done in ```setup()```

```cpp
void stopButton(uint8_t stopPin);
```

- **stopPin** is the Pin number for an additional stop button.
- 
**Example**

```
void setup() {
	// ...
	fader1.stopButton(3); // add a fire button on pin 3
	//...
	}
```

#### loadButton()

Add a button for an action on the LOAD button. This must done in ```setup()```

```cpp
void loadButton(uint8_t loadPin);
```

- **loadPin** is the Pin number for an additional load button.

**Example**

```
void setup() {
	// ...
	fader1.loadButton(4); // add a fire button on pin 4 
	//...
	}
``` 


#### update()

To get the actual button state you must call inside the ```loop()```

```
void update();
```
**Example**

```cpp
void loop() {
	// ...
	fader1.update();
	// ...
	}
```

### value()

Return the value (0...100%) of the fader.

```cpp
uint8_t value();
```

**Example**
```cpp
uint8_t value = fader1.value();
```

### fetch()

Lock the sending of OSC fader data until the value defined in fetch() is reached.<br>
This functionality is intended for page changing. So you need to fetch the fader before you can use it.

```cpp
void fetch(uint8_t value);
```

- **value*** unlock value

**Example**
```cpp
fader201.fetch(0); // set fetch value to 0
```

### lock()

Get or set the state of the fetch function, can used for indication of the fader state or force a new state.

```cpp
bool lock();
void lock(bool state);
```

- ```true``` locked fader
- ```false``` unlocked fader

**Example**

```cpp
bool state = fader1.lock(); // get the lock state
fader1.lock(false); // set the lock state
```

### jitter()

This functionality is a helper function for fetching.<br> 
e.g. if ```fetch(20)``` and ```jitter(2)``` the unlock value expand to a range from ```18 ... 22```

```cpp
void jitter(uint8_t delta);
```

- **delta** +/- value range

Example
```cpp
fader1.jitter(2); // set fetch range to +/- 2
```