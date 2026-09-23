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
If you have wishes for other functions or classes use the discussion tab. If you find bugs make an issue, nobody is perfect. For questions use the discussion page.

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

## Examples
There are differnet examples available:
- start, for beginners
- usbtest
- Using Adafruit Feather RP2350, NeoKey 1x4, Stemma QT Rotary Encoder and 4x20 I2C LCD Display
	- BOX1_S_LCD_I2C with a simple parameter list
	- BOX2_S_LCD_I2C with catagory and fixed parameter list
	- BOX3_S_LCD_I2C with catagory and dynamic parameters
- Using Raspberry Pi Pico, hardware buttons, encoders and 4x20 LCD
	- BOX4_C_LCD with catagory and fixed parameter list
	- BOX5_C_LCD with catagory and dynamic parameters
- TBD examples for Touchscreens and Ethernet

## Library
I used the Raspberry Pi Pico(2) for development, let me know if there are problem with other boards / microcontrollers.<br>
The library itself use the `std::string` and `std::vector` C++ library so only modern Microcontrollers which enough RAM and C++ Standard Libraries are supported<br>
Therefore you must use `.c_str()` functionality to convert it to conventional char arrays.<br>

Recommanded libraries for your projects
- Ethernet
	- Ethernet3 [https://github.com/sstaub/Ethernet3](https://github.com/sstaub/Ethernet3) for WIZnet 5500 chips with MAC support for Pico
	- QNEthernet [https://github.com/ssilverman/QNEthernet](https://github.com/ssilverman/QNEthernet) for Teensy 4.1
- LCD Displays
	- HD44780 [https://github.com/sstaub/LCD-HD44780]([https://github.com/sstaub/LCD-HD44780](https://github.com/sstaub/LCD-HD44780))
	- HD44780 with I2C [https://github.com/sstaub/LCD-I2C-HD44780](https://github.com/sstaub/LCD-I2C-HD44780)
- Touchscreen Nextion
	- NextionX3 [https://github.com/sstaub/NextionX3](https://github.com/sstaub/NextionX3) for Nextion Touchscreens
- I/O Expanders
	- A/D MCP3208 (SPI) [https://github.com/sstaub/MCP_3208](https://github.com/sstaub/MCP_3208)
	- I/O MCP23017 (I2C) [https://github.com/sstaub/MCP_23017](https://github.com/sstaub/MCP_23017)
- Adafruit Seesaw Endcoder and Keys [https://github.com/sstaub/Seesaw](https://github.com/sstaub/Seesaw)


## IDE's
- VS Code with PlatformIO [https://platformio.org](https://platformio.org) is the a recommanded IDE
- VS Code with "pioarduino" [https://marketplace.visualstudio.com/items?itemName=pioarduino.pioarduino-ide](https://marketplace.visualstudio.com/items?itemName=pioarduino.pioarduino-ide) extension which allows clang support [https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)
- VS Code with "Arduino Maker Workshop" [https://marketplace.visualstudio.com/items?itemName=TheLastOutpostWorkshop.arduino-maker-workshop](https://marketplace.visualstudio.com/items?itemName=TheLastOutpostWorkshop.arduino-maker-workshop) extension
- You could try also the Theia IDE [https://theia-ide.org](https://theia-ide.org)
- Arduino IDE 2.x [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)

## Boards
**!!! There is no support for AVR (UNO, MEGA) !!!**<br>
The main development platform is the Raspberry Pi Pico(2) based on the RP2040/RP2350 microcontroller.<br>
Only boards with native USB are supported, in the moment this is RaspeberryPI Pico(2) and Teensy (3.5 and 4.1),
these boards have also native `printf` support.<br>
From Arduino it should work with boards like GIGA or Arduino Uno Q.<br>

**Pro of the Picos**

- cheap, starting at $4.- for the original, $3.- for clones with USB-C
- powerfull 133MHz Clock, Dual Core
- 264kB (520kB Pico2) RAM, up to 16MB Flash and external RAM (Pico2)
- WIZnet [https://wiznet.io](https://wiznet.io) have many boards with Ethernet, the price is about 10.-$/€

**Contra**

- old style MicroUSB port, but there are many boards with USB C connector on the market e.g. from Adafruit.
- only 3 analog inputs, you should use an external AD converter e.g. MCP3208

It is recommended to use the Earl E. Philhower Core [https://github.com/earlephilhower/arduino-pico](https://github.com/earlephilhower/arduino-pico) instead of the mBed based Arduino core.

## Hardware

### Adafruit Seesaw
Support for Adafruits Seesaw I2C modules which makes hardware setup very simple.<br>
- Encoder [https://www.adafruit.com/product/4991](https://www.adafruit.com/product/4991)

- NeoKey 1x4 [https://www.adafruit.com/product/4980](https://www.adafruit.com/product/4980)

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
- Ethernet3 library have also a function to generate a local MAC Address automatic
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
- TCP Slip over port 3037 (default)


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

###**Example**

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

1. Use the `init()` function in `setup()`

- for UDP

```cpp
#include "Ethernet3.h"
#include "eOS3.h"
EthernetUDP udp;
eOS3 eos;
void setup () {
  Ethernet.begin(mac, localIP, subnet);
  eos.begin(udp, eosIP, eosPort, localPort);
  // eos.begin(udp, eosIP); // for using default ports
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
  eos.begin(tcp, eosIP);
  }
```

## Examples
TBD

# Documention eOS3 class basics
You must include the header file in the beginning of the sketch.<br>
`#include "eOS3.h"`

## begin() USB
Set the USB interface. !Hint! Because of an EOS Bug [EOS-54520] you will not receive all necessary data until you reselect your channels.

```cpp
void begin();
```

###**Example**

```cpp
#include "eOS3.h"
eOS3 eos;
void setup() {
  // ...
  eos.begin(();
  // ...
  }
```

## begin() UDP
Set the UDP interface
```cpp
void begin(UDP &udp, IPAddress ip, uint16_t udpRxPort = 8000, uint16_t udpTxPort = 8001);
```
- **ip** the IP address of the console / Nomad
- **&udp** UDP interface
- **udpRxPort** EOS UDP RX receive port, default 8000
- **udpTxPort** EOS UDP TX transmit port, default 8001

###**Example**
```cpp
#include "eOS3.h"
eOS3 eos;
void setup() {
  // ...
  eos.begin(udp, eosIP, eosPort, localPort);
  // eos.begin(udp, eosIP); // for using default ports
  // ...
  }
```

## begin() TCP
Set the TCP interface on port 3037
```cpp
void begin(Client &tcp, IPAddress ip, uint16_t tcpPort = 3037);
```
- **ip** the IP address of the console / Nomad
- **&tcp** TCP interface
- **&tcpPort** TCP port, default 3037

###**Example**
```cpp
#include "eOS3.h"
eOS3 eos;
void setup() {
	// ...
	eos.begin(eosIP, tcp);
	// ...
	}
```

## update()
Update the handlers for maintain(), connected() and disconnected(), must done in `loop()`
```cpp
void update();
```

###**Example**
```cpp
#include "eOS3.h"
eOS3 eos;

void setup() {
  eos.begin();
  // ...
  }

void loop() {
  // ...
  eos.update();
  // ...
  }
```

## reboot()
Reboot your microcontroller, this is necessary to clear all data after connection lost. Only available for Pico and Teensy (not tested), must done in `disconnect()`
```cpp
void update();
```

###**Example**
```cpp
#include "eOS3.h"
eOS3 eos;

void setup() {
  // ...
  }

void disconnect() {
  // ...
  eos.reboot();
  // ...
  }
```


# eOS3 Helper functions
Small functions to improve workflow.

## filter()
With a Filter you get only messages from the console which you can use for proceeding informations.<br>
This should done in the ```connected()``` callback.
```cpp
void filter(string pattern);
```
- **pattern** filter pattern

###**Example**
```cpp
eos.filter("/eos/out/param/*"); // only parameter messages will send to the endpoint
```

## filterRemove()
Remove a specific filter.
```cpp
void filterRemove(string pattern);
```
- **pattern** filter pattern

###**Example**
```cpp
eos.filterRemove("/eos/out/param/*");
```

## filterClear()
Delete all added filters.
```cpp
void filterClear();
```

###**Example**
```cpp
eos.filterClear();
```

## subscription()
With subscription you can get special informations about the dedicated parameters.

```cpp
void subscription(int32_t subscribe = SUBSCRIBE);
void subscription(string parameter, int32_t subscribe = SUBSCRIBE);
```

- **parameter** optional parameter name you want to subscribe
- **subscribe** you can SUBCRIBE (default) or UNSUBSCRIBE a parameter

###**Example**

```cpp
eos.subscription("Pan"); // on Pan change EOS will send ```/eos/out/param/Pan=<value>```
```

## ping()
With a ping you can get a reaction from the console which helps you to identify your box and if is alive. You should send a ping regularly with message to identify your box on the console.

```cpp
void ping(); // send a ping without a message
void ping(string message); // send a ping with a string
void ping(int32_t number); // send a ping with a number
```

- **message** otional string to send
- **number** otional number to send

###**Example**

```cpp
eos.ping("hello EOS"); // will send a ping with a message
```

## reset()
This function reset the OSC settings

```cpp
void reset();
```

###**Example**

```cpp
eos.reset();
```

## command()
Send a string to the existing command line

```cpp
void command(string cmd); // send a command
```

###**Example**

```cpp
eos.command("Chan 5);
```

## newCommand()
Send a string to the command line and clear it before

```cpp
void newCommand(string cmd); // send a command
```

###**Example**

```cpp
eos.newCommand("Chan 5);
```

## user()
This function allows you to change the user ID e.g.

```cpp
void user(int16_t userID);
```

- **userID** the ID of the user
	- **0** is the background user
	- **-1** is the current user
	- or any other user ID

###**Example**

```cpp
eos.user(2); // set user 2
```

## initDS()
Initialise a Direct Select bank, this must done after an established connection, so this method should used inside the `connected()` function.

```cpp
void initDS(button_t type, uint8_t count, uint8_t index = 1, uint16_t page = 1, bool flexi = false);
```

- **type** DS type, CHAN, GROUP, IP, FP, CP, BP, PRESET, MACRO, FX, SNAP, MS, SCENE, PIXMAP, CURVE, SC 
- **count** number of DS buttons to init, default 10
- **init** index (bank) number, default 1
- **page** fader page, default 1
- **flexi** set flexi state, default false

###**Example**

```cpp
void connected() {
  // ...
  eos.initDS(MACRO, 10); // init 10 macro buttons
  // ...
  }
```

# Classes

## Shift()
This function allows you to assign a hardware button as a **Shift** button. **Shift** set the encoder and wheel messages to the **Fine** mode.
For the **Intens** parameter it works in opposite and does accelerate the ticks. This should done before `setup()`.

### Constructor
```cpp
Shift(uint8_t pin);
Shift(); // for virtual devices
```
- **pin** number of the pin you want to use for SHIFT, not needed for virtual devices

### Methods

#### update()
Update must done regulary in `loop()` to check the state of the shift button.
```cpp
void update();
void update(bool state); // for virtual devices
```

#### mode()
Mode allows you set the shift button in Toggle mode
```cpp
void mode(buttonMode_t modus);
```
- **modus** change between PUSH and TOGGLE

#### tick()
Because Shift does also works as an acceleration button for Intens parameter, tick() allows to change the tick rate.

```cpp
void tick(uint8_t tick = 8);
```
- **tick** tick rate, default 8 ticks

#### state()
Get the current state of the **Shift** button.
```cpp
bool state();
```

###**Example**
```cpp
Shift shift(3) // init class and use pin 3 for the SHIFT button
void setup() {
  shift.mode(TOGGLE);
  shift.tick(4); // set tick rate to 4 ticks
  }
void loop() {
  shift.update();
  }
```

## Acceleration
This function allows you to assign a hardware button as a **Accelaration** button. **Accelaration** set the encoder and wheel messages to a multiplier mode to accelerate the output.

### Constructor
```cpp
Acceleration(uint8_t pin);
Acceleration();
```
- **pin** number of the pin you want to use for Acceleration, not needed for virtual devices

### Methods

#### update()
Update must done regulary in `loop()` to check the state of the shift button.
```cpp
void update();
void update(bool state); // for virtual devices
```

#### mode()
Mode allows you set the shift button in Toggle mode
```cpp
void mode(buttonMode_t modus);
```
- **modus** change between PUSH and TOGGLE

#### tick()
Change the tick rate.

```cpp
void tick(uint8_t tick = 8);
```
- **tick** tick rate, default 8 ticks

#### state()
Get the current state of the **Acceleration** button.
```cpp
bool state();
```

###**Example**
```cpp
Acceleration acc(4) // init class and use pin 4 for the Acceleration button
void setup() {
  acc.mode(TOGGLE);
  acc.tick(4); // set tick rate to 4 ticks
  }
void loop() {
  acc.update();
  }
```


## Button
With this new universal class you can create generic buttons. In the moment following function types are available:
- RAW send a raw message (without arguments)
- KEY send a key press
- MACRO fires a macro
- IP, CP, FP, BP palettes
- PRESET
- CHAN
- GROUP
- FX (Effect)
- PIXMAP
- CURVE
- SNAPSHOT
- SCENE
- SC (show control)

### Constructor
Create a new **Button** object. This should done before `setup()`
```cpp
Button(uint8_t pin, button_t type, strng);
Button(uint8_t pin, button_t type, int number);
Button(button_t type, strng);
Button(button_t type, int number);
```
- **pin** pin for the button hardware, not needed for virtual devices
- **type** the function type, RAW, KEY, MACRO, IP, CP, FP, BP, PRESET, CHAN, GROUP, FX, PIXMAP, CURVE, SNAPSHOT, SCENE, SC
- **strng** for RAW message (without an argument) or KEY name
- **number** for e.g. MACRO number


### Methods

#### update()
To get the current button state you must call inside the `loop()`
```cpp
void update();
void update(bool state); // for virtual devices
```

###**Example**
```cpp
Button next(2, KEY, "Next"); // make a new osc button on Pin 2
Button last(3, KEY, "Last"); // make a new osc button on Pin 3
void setup() {
  // ...
  }
void loop() {
  // ...
  next.update();
  last.update();
  // ...
  }
```

## Button2nd, Button3rd
This classes allows up to two additional underlaying **Button** objects. They will controlled by the **Control2nd** and **Control3rd** classes. It works the same way as the **Button** object. It works with **Button** objects but also with all other control buttons exept **Fader** and **Submaster** classes.

### Constructor
Create a new **Button2nd** or **Button3rd** object. This should done before `setup()`
```cpp
Button2nd(uint8_t pin, button_t type, strng);
Button2nd(uint8_t pin, button_t type, int number);
Button2nd(button_t type, strng);
Button2nd(button_t type, int number);

Button3rd(uint8_t pin, button_t type, strng);
Button2rd(uint8_t pin, button_t type, int number);
Button3rd(button_t type, strng);
Button3rd(button_t type, int number);
```
- **pin** pin for the button hardware, not needed for virtual devices
- **type** the function type, RAW, KEY, MACRO, IP, CP, FP, BP, PRESET, CHAN, GROUP, FX, PIXMAP, CURVE, SNAPSHOT, SCENE, SC
- **strng** for RAW message (without an argument) or KEY name
- **number** for e.g. MACRO number

### Methods

#### update()
To get the current button state you must call inside the `loop()`
```cpp
void update();
void update(bool state); // for virtual devices
```

## Control2nd, Control3rd
This classes allow you to control the underlaying **Button2nd** and **Button3rd** functionality. You can do it with an extra pin but also in conjunction with the **Shift** and **Acceleration** classes and works similar to them.
So you can use the same pin e.g. for **Shift** and **Control2nd**.

### Constructor
Create a new OSC button object. This should done before `setup()`
```cpp
Control2nd(uint8_t pin);
Control2nd();
Control3rd(uint8_t pin);
Control3rdd();
```
- **pin** pin for the control button hardware, not needed for virtual devices

### Methods

#### update()
Update must done regulary in `loop()` to check the state of the control button.
```cpp
void update();
void update(bool state); // for virtual devices
```

#### mode()
Mode allows you set the control button in Toggle mode
```cpp
void mode(buttonMode_t modus);
```
- **modus** change between PUSH and TOGGLE

#### state()
Get the current state of the **Acceleration** button.
```cpp
bool state();
```

###**Example**
```cpp
Control2nd button2nd(3) // init class and use pin 3 for the control2nd button
void setup() {
  button2nd.mode(PUSH);
  }
void loop() {
  button2nd.update();
  }
```

## Encoder
This class allows to construct an **encoder** objects to control parameter values.

### Constructor
Creates an **encoder** object,
this should done before the `setup()`
```cpp
Encoder(uint8_t pinA, uint8_t pinB, uint8_t direction = FORWARD);
Encoder(direction_t direction = FORWARD);
```
- **pinA** and **pinB** are the connection Pins for the encoder hardware, not needed for virtual devices
- **direction** is used for changing the direction of the encoder to clockwise if pinA and pinB are swapped. The directions are FORWARD (default) or REVERSE

### Methods

#### parameter()
Before using the encoder you must assign the parameter you want control. This should done in `setup()` or in control functions for parameters.
```cpp
void parameter(string param);
```
- **param** is the Parameter which you want assign

#### parse()
Parse allows you to get parameter data. This can only done when none of the parameter control classes are used.
If there is an new value, the function return `true`.
**parse()** must used inside `maintain()`.
```cpp
bool parse();
```

#### value()
Get the value of a parameter as an float.
```cpp
float value();
```

#### active()
Check if there is an active value for the choosen parameter.
This allows you to supress the output on displays.
If there is a value, the function return `true`.
You must use **parse()** before.
```cpp
bool active();
```

#### callback()
You can add a callback function, which is triggered when there is a new parameter value inside the **parse()** function.
This should done in `setup()`.
```cpp
void callback(cbptr call);
```

#### update()
To check the actual encoder state you must call inside the `loop()`
```cpp
void update();
void update(bool stateA, bool stateB);
void update(int32_t motion);
```
- **stateA** optional for virtual devices
- **stateA** optional for virtual devices
- **motion** optional for direct input of the encoder motion, e.g. for Seesaw Encoders

###**Example**
```cpp
Encoder encoder1(A0, A1, REVERSE);
void setup() {
  // ...
  encoder1.parameter("Pan");
  // ...
  }
void loop() {
  // ...
  encoder1.update();
  // ...
  }
```

## Wheel

## Direct Select


## Submaster
This class allows you to control a submaster with a hardware (slider) potentiometer as a fader and a bump button. See also the hardware advices above.

### Constructor
Create a new Submaster object. This should done before `setup()`
```cpp
Submaster(uint8_t analogPin, uint16_t sub;
Submaster(uint16_t sub);
```
- **analogPin** are the connection Analog Pin for the fader leveler, not needed for virtual devices
- **sub** is the submaster number you want to control

### Methods

#### fireButton()
Configure an optional fire button.
```cpp
void fireButton(uint8_t firePin);
void fireButton();
```
- **firePin** is the Pin number for an optional bump button.

#### callback()
Optional callback when value changed.
```cpp
void callback(cbptr call);
``` 

#### value()
Get the current value from 0...100
```cpp
uint8_t value();
```

#### updateAnalog()
Update the analog input, must inside `loop()`
```CPP
void update();
```

#### updateAnalog()
Update the input for virtual devices, must inside `loop()`
```CPP
void updateAnalog(int analog);
```
- **analog** analog value 10bit

#### updateValue()
Update the input for virtual devices, must inside `loop()`
```CPP
void updateValue(uint8_t value);
```
- **analog** analog value 0...100

#### updateFire()
Update the fire button, must inside `loop()`
```CPP
void updateFire();
void updateFire(bool fireState);
```
- **fireState** state of the fire button, optional for virtual devices

###**Example**
```cpp
Submaster submaster1(A1, 1); // leveler is Analog Pin A1, submaster 1
void setup() {
  // ...
  subnaster.fireButton(2); // fire button on pin 2
  // ...
  }
void loop() {
  // ...
  submaster1.update();
  submaster1.updateFire();
  // ...
  }
```

## Fader
This class allows you to control a fader containing optional Fire/Stop/Load control buttons, all  functions configured in EOS Tab 36, with a hardware (slide) potentiometer as a fader and buttons.<br>
Before using Faders you must call **initFaders(faders, index, page)**<br>
See also the hardware advices above.

## initFaders(), part of the eOS3 helper class
Initialise a fader bank, this must done after an established connection, so this method should used inside the `connected()` function.

```cpp
void initFaders(uint8_t faders = 10, uint8_t index = 1, uint8_t page = 1);
```
- **faders** number of faders to init, default 10
- **init** index (bank) number, default 1
- **page** fader page, default 1

### Constructor
Create a new Fader object. This should done before `setup()`

```cpp
Fader(uint8_t analogPin, uint8_t fader, uint8_t index = 1);
Fader(uint8_t fader, uint8_t index = 1);
```
- **analogPin** are the connection Analog Pin for the fader leveler, not needed for vitual devices
- **fader** is the fader number of the fader page you want to control
- **index** is the internal OSC index(bank) number

### Methods

#### fireButton()
Add a button for an action on the GO button. This must done in `setup()`
```cpp
void fireButton(uint8_t firePin);
void fireButton();
```
- **firePin** pin number for an additional fire button, not needed for virtual devices

#### stopButton()
Add a button for an action on the STOP/BACK button. This must done in `setup()`
```cpp
void stopButton(uint8_t stopPin);
void stopButton();
```
- **stopPin** pin number for an additional stop button, not needed for virtual devices

#### loadButton()
Add a button for an action on the LOAD button. This must done in `setup()`
```cpp
void loadButton(uint8_t loadPin);
void loadButton();
```
- **loadPin** pin number for an additional load button, not needed for virtual devices

#### callback()
Optional callback when value changed.
```cpp
void callback(cbptr call);
``` 

#### value()
Get the current value from 0...100
```cpp
uint8_t value();
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

**Example**
```cpp
fader1.jitter(2); // set fetch range to +/- 2
```

#### updateAnalog()
Update the analog input, must inside `loop()`
```CPP
void update();
```

#### updateAnalog()
Update the input for virtual devices, must inside `loop()`
```CPP
void updateAnalog(int analog);
```
- **analog** analog value 10bit

#### updateValue()
Update the input for virtual devices, must inside `loop()`
```CPP
void updateValue(uint8_t value);
```
- **analog** analog value 0...100

#### updateFire()
Update the fire button, must inside `loop()`
```CPP
void updateFire();
void updateFire(bool fireState);
```
- **fireState** state of the fire button, optional for virtual devices

#### updateStop()
Update the fire button, must inside `loop()`
```CPP
void updateStop();
void updateStop(bool fireState);
```
- **stopState** state of the stop button, optional for virtual devices

#### updateLoad()
Update the load button, must inside `loop()`
```CPP
void updateLoad();
void updateLoad(bool fireState);
```
- **loadState** state of the load button, optional for virtual devices

###**Example**
```cpp
Fader fader1(A1, 1, 1); // leveler is Analog Pin A1, fader number 1 is controlled, bank number is 1
void setup() {
  fader1.fireButton(2); // add a fire button on pin 2
  fader1.stopButton(3); // add a fire button on pin 3
  }
void loop() {
  fader1.updateAnalog();
  fader1.updateFire();
  fader1.updateStop();
  }
void connected() {
  // ...
  eos.initFaders(5); // init 5 faders
  // ...
  }
```

## FaderTool
This class allows you configure the faders. It gives you fader page control functions and also parsing for fader informations.

### Constructor
Create a FaderTool object. This should done before `setup()`
```cpp

```

# Parameter handling classes
There are 3 ways to handle parameters.
- **SelectParameter** allows you to make a simple parameter list, with an Up/Down button to step through the list.
- **SelectCategory** allows you to make a parameter list organized in categories. It use 6 buttons for each parameter category.
- **SelectDyn** works dynamic where only parameters are available depending from the channel selection. It use also 6 buttons for each parameter category.

## SelectParameter
This class allows you to control a parameter list with an Up/Down button to step through. The buttons have a wrap behavior. 

### Constructor
Create parameter selection object. This should done before `setup()`
```cpp
SelectParameter(uint8_t pinUp, uint8_t pinDown, uint8_t encoders, uint8_t parameters);
```
- **pinUp** pin for the button increasing the index
- **pinDown** pin for the button decreasing the index
- **encoders** number of the encoders you use in the application
- **parameters** number of the parameters in the list

###**Example**
```cpp
SelectParameter selection(6, 7, 2, 9); 
// create an select parameter object with up button on pin 6, down button on pin 7, 
// using two encoders and a parameter list with 9 items
```

### Methods

#### parameter()
Set the name of a parameter by index, this must done in `setup()`
```cpp
void parameter(uint8_t index, string name);
```
- **index** position of the parameter in the list, start with 1
- **name** name of the parameter, names must EOS parameter names

###**Example**
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
```

#### callback()
Add a callback function, the callback is triggered when page is changed by the Up/Down buttons. The callback can used to update the display. This must done in `setup()`
```cpp
void callback(cbptr callback);
```
- **callback** pointer to the callback function

###**Example**
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
It must used with receiveOSC() function in `loop()`
```cpp
uint8_t parse();
```

#### parameter()
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

###**Example**
```cpp
uint8_t numPages = selection.pages();
```

#### page()
Return the actual selected page number. 
```cpp
uint8_t page();
```

###**Example**
```cpp
uint8_t currentPage = selection.page();
```

#### update()
To get the actual state of the page number you must call inside the `loop()`
```cpp
void update();
```

###**Example**
```cpp
void loop() {
	// ...
	selection.update();
	// ...
	}
```


# Special parser classes
The special parser classes allows to proceed additional data sended by EOS.

## Softkey
This class allows you to get **Softkey** data.

### Constructor
Create a new **Softkey** object. This should done before `setup()`
```cpp
Softkey();
```

### Methods

#### parse()
Parse check for new **Softkey** data. This must done in `maintain()`. Return the number (1 - 12) of the last parsed softkey, 0 if there is no data.
```cpp
uint8_t parse();
```

#### callback()
Optional callback when new data arrived. The callbackfunktion use `function(uint8_t)` prototype, so you get also the softkey number.
```cpp
void callback(cbptr2 call);
``` 

#### label()
Get the label of a softkey as a string.
```cpp
string label(uint8_t sk);
```
- **sk** number (1 - 12) of the softkey

###**Example**
```cpp
softkey softkey;
void sk(uint8_t sk) { // callback function
  string label = softkey.label(sk);
  }
void setup() {
  softkey.callback(sk); // function pointer to the callback function
  }
void maintain() {
  softkey.parse();
  }
```

## PanTilt
This class allows you to get **PanTilt** data.

### Constructor
Create a new **PanTilt** object. This should done before `setup()`
```cpp
PanTilt();
```

### Methods

#### parse()
Parse check for new **PanTilt** data. This must done in `maintain()`. Returns `true` if new data.
```cpp
bool parse();
```

#### callback()
Optional callback when new data arrived.
```cpp
void callback(cbptr call);
``` 

#### active()
Check if there are valid data. Return `true` if active data.
```cpp
bool active();
```

#### pan(), panMin(), panMax(), tilt(), tiltMin(), tiltMax()
Get the **PanTilt datas** `float`.
```cpp
float pan();
float panMin();
float panMax();
float tilt();
float tiltMin();
float tiltMax();
```

###**Example**
```cpp
PanTilt pantilt;
float pan;
float tilt;
void data() {
  if (pantilt.active()) {
	pan = pantilt.pan();
	tilt = pantilt.tilt();
	}
  }
void setup() {
  pantilt.callback(data); // function pointer to the callback function
  }
void maintain() {
  pantilt.parse();
  }
```

## XYZ
This class allows you to get **XYZ** data.

### Constructor
Create a new **XYZ** object. This should done before `setup()`
```cpp
XYZ();
```

### Methods

#### parse()
Parse check for new **XYZ** data. This must done in `maintain()`. Returns `true` if new data.
```cpp
bool parse();
```

#### callback()
Optional callback when new data arrived.
```cpp
void callback(cbptr call);
``` 

#### active()
Check if there are valid data. Return `true` if active data.
```cpp
bool active();
```

#### x(), y(), z()
Get the **XYZ** datas `float`.
```cpp
float x();
float y();
float z();
```

###**Example**
```cpp
XYZ xyz;
float x;
float y;
float z;
void data() {
  if (xyz.active()) {
	x = xyz.x();
	y = xyz.x();
	z = xyz.x();
	}
  }
void setup() {
  xyz.callback(data); // function pointer to the callback function
  }
void maintain() {
  xyz.parse();
  }
```

## HueSat
This class allows you to get **HueSat** data.

### Constructor
Create a new **HueSat** object. This should done before `setup()`
```cpp
HueSat();
```

### Methods

#### parse()
Parse check for new **HueSat** data. This must done in `maintain()`. Returns `true` if new data.
```cpp
bool parse();
```

#### callback()
Optional callback when new data arrived.
```cpp
void callback(cbptr call);
``` 

#### active()
Check if there are valid data. Return `true` if active data.
```cpp
bool active();
```

#### hue(), saturation()
Get the **HueSat** datas `float`.
```cpp
float hue();
float saturation();
```

#### red(), green(), blue(), color565()
Get converted **HueSat** datas.
```cpp
uint8_t red();
uint8_t green();
uint8_t blue();
uint16_t color565();
```

###**Example**
```cpp
HueSat huesat;
float hue;
float sat;
void data() {
  if (huesat.active()) {
	hue = huesat.hue();
	sat = huesat.saturation();
	}
  }
void setup() {
  huesat.callback(data); // function pointer to the callback function
  }
void maintain() {
  huesat.parse();
  }
```

## Channel
This class allows you to get **Channel** data.

### Constructor
Create a new **Channel** object. This should done before `setup()`
```cpp
Channel();
```

### Methods

#### parse()
Parse check for new **Channel** data. This must done in `maintain()`. Returns `true` if new data.
```cpp
bool parse();
```

#### callback()
Optional callback when new data arrived. It also return the cue type.
```cpp
void callback(cbptr call);
```

#### channel()
Get the complete channel data as a string.
```cpp
string channel();
```

#### selection()
Get the channel selection as a string.
```cpp
string selection();
```

#### value()
Get the value as a string.
```cpp
string value();
```

#### type()
Get the type of the first selected channel as a string.
```cpp
string type();
```

#### address()
Get the dmx address of the first selected channel as a string.
```cpp
string address();
```

###**Example**
```cpp
Channel chan;
string channel;
void data() { // callback function
  channel = channel.channel();
  }
void setup() {
  chan.callback(data); // function pointer to the callback function
  }
void maintain() {
  chan.parse();
  }
```


## Command
This class allows you to get **Command** data.

### Constructor
Create a new **Command** object. This should done before `setup()`
```cpp
Command();
```

### Methods

#### parse()
Parse check for new **Command** data. This must done in `maintain()`. Returns `true` if new data.
```cpp
bool parse();
```

#### callback()
Optional callback when new data arrived. It also return the cue type.
```cpp
void callback(cbptr call);
```

#### cammand()
Return the commend line as a string.
```cpp
string command();
```

###**Example**
```cpp
Command cmd;
string cmdline;
void data() { // callback function
  cmdline = cmd.command();
  }
void setup() {
  cmd.callback(data); // function pointer to the callback function
  }
void maintain() {
  cmd.parse();
  }
```


## Cue
This class allows you to get **Cue** data.

### Constructor
Create a new **Cue** object. This should done before `setup()`
```cpp
Cue();
```

### Methods

#### parse()
Parse check for new **Cue** data. This must done in `maintain()`. Returns the cue type if new data.
```cpp
cue_t parse();
```

#### callback()
Optional callback when new data arrived. It also return the cue type.
```cpp
void callback(cbptrC call);
```

#### cue()
Return the complete cue text as a string.
```cpp
string cue(cue_t type);
```
- **type** cue type PREVIOUS, ACTIVE or PENDING

#### cueList()
Return the cue list as a string.
```cpp
string cueList(cue_t type);
```
- **type** cue type PREVIOUS, ACTIVE or PENDING

#### cueNumber()
Return the cue number as a string.
```cpp
string cueNumber(cue_t type);
```
- **type** cue type PREVIOUS, ACTIVE or PENDING

#### label()
Return the cue label as a string.
```cpp
string label(cue_t type);
```
- **type** cue type PREVIOUS, ACTIVE or PENDING

#### duration()
Return the duration time as a string.
```cpp
string cueList(cue_t type);
```
- **type** cue type PREVIOUS, ACTIVE or PENDING

#### progress()
Return the progress as a string.
```cpp
string cueList(cue_t type);
```
- **type** cue type PREVIOUS, ACTIVE or PENDING

###**Example**
```cpp
Cue cue;
string cuePrevious;
string cueActive;
string cuePending;
void data(cue_t type) { // callback function
  if (type == PREVIOUS) 
	cuePrevious = cue.cue(PREVIOUS);
  else if (type == ACTIVE)
	cueActive = cue.cue(ACTIVE);
  else if (type == PENDING) 
	cuePending = cue.cue(PENDING);
  }
void setup() {
  cue.callback(data); // function pointer to the callback function
  }
void maintain() {
  cue.parse();
  }
```


## Version
This class allows you to get **Version** data.

### Constructor
Create a new **Version** object. This should done before `setup()`
```cpp
Version();
```

### Methods

#### version()
This is an explicit getter function to tell EOS to send version datas.
```cpp
void version();
```

#### parse()
Parse check for new **Version** data. This must done in `maintain()`. Returns `true` if new data.
```cpp
bool parse();
```

#### callback()
Optional callback when new data arrived.
```cpp
void callback(cbptr call);
```

#### software()
Returns the software version as a string.
```cpp
string software();
```

#### library()
Returns the library version name as a string.
```cpp
string library();
```

###**Example**
```cpp
Version version;
string software;
string library;
void data() { // callback function
  software = show.software();
  library = show.library();
  }
void setup() {
  version.callback(data); // function pointer to the callback function
  version.version(); //getter for version
  }
void maintain() {
  version.parse();
  }
```


## User
This class allows you to get **User** data.

### Constructor
Create a new **User** object. This should done before `setup()`
```cpp
User();
```

### Methods

#### parse()
Parse check for new **User** data. This must done in `maintain()`. Returns `true` if new data.
```cpp
bool parse();
```

#### callback()
Optional callback when new data arrived.
```cpp
void callback(cbptr call);
```

#### user()
Returns the **User** ID.
```cpp
uint16_t user();
```

###**Example**
```cpp
User user;
uint16_t user;
void data() { // callback function
  user = user.user();
  }
void setup() {
  user.callback(data); // function pointer to the callback function
  }
void maintain() {
  user.parse();
  }
```


## Show
This class allows you to get **Show** data.

### Constructor
Create a new **Show** object. This should done before `setup()`
```cpp
Show();
```

### Methods

#### parse()
Parse check for new **Show** data. This must done in `maintain()`. Returns `true` if new data.
```cpp
bool parse();
```

#### callback()
Optional callback when new data arrived.
```cpp
void callback(cbptr call);
```

#### name()
Returns the **Show** name as a string.
```cpp
string name();
```

###**Example**
```cpp
Show show;
string showname;
void data() { // callback function
  showname = show.name();
  }
void setup() {
  show.callback(data); // function pointer to the callback function
  }
void maintain() {
  show.parse();
  }
```


## EventState
This class allows you to get **EventState** data.

### Constructor
Create a new **EventState** object. This should done before `setup()`
```cpp
EventState();
```

### Methods

#### parse()
Parse check for new **EventState** data. This must done in `maintain()`. Returns `true` if new data.
```cpp
bool parse();
```

#### callback()
Optional callback when new data arrived.
```cpp
void callback(cbptr call);
``` 

#### state()
Get the state of the **EventState** as a string, this can LIVE or BLIND.
```cpp
string state();
```

###**Example**
```cpp
EventState eventState;
string event;
void data() { // callback function
  event = eventstate.state();
  }
void setup() {
  eventstate.callback(data); // function pointer to the callback function
  }
void maintain() {
  eventstate.parse();
  }
```