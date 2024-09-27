
# esphome-pace-bms

This is an **ESPHome** component that supports "**paceic**" protocol **version 20 and 25** which is used by seemingly the majority of low-cost rack-mount Lithium Iron (LiFePO4) battery packs (but occasionally a different chemistry as well) manufactured in SE Asia.  The BMS can be communicated with over **RS485** or **RS232** and is manufactured by PACE (or is a clone).  It's used by many, many different manufacturers under different labels and branding.

The protocol is characterized by both requests and responses beginning with a '**~**' (tilde) character followed by two ASCII numbers (usually) either "**20**" or "**25**" and ending with a '**\r**' (carriage return) character.  

If you are a developer, the protocol implementation is fully portable with a clean interface in C++ and with no dependencies on ESPHome or any other libraries (it does require C++17 support due to use of `std::optional`, though that could easily be removed).  Feel free to use it for whatever you wish, but a heads-up would be appreciated just so I know what's happening with it :)

I strongly encourage you to read through this entire document, but here's a table of contents:
- [What Is Paceic Protocol Version 20](#What-Is-Paceic-Protocol-Version-20)
- [What Is Paceic Protocol Version 25](#What-Is-Paceic-Protocol-Version-25)
- [What Is Pace MODBUS Protocol](#What-Is-Pace-MODBUS-Protocol)
- [Supported BMS Sensors (read only)](#Supported-BMS-Sensors-read-only)
- [Supported BMS Configuration (read / write)](#Supported-BMS-Configuration-read--write)
- [Supported BMS Configuration (read / write) - **Protocol Version 25 ONLY**](#Supported-BMS-Configuration-read--write---Protocol-Version-25-ONLY)
- [What Battery Packs are Supported?](#What-Battery-Packs-are-Supported)
- [What ESPs are Supported?](#What-ESPs-are-Supported)
- [How do I wire my ESP to the RS485 port?](#How-do-I-wire-my-ESP-to-the-RS485-port)
- [How do I wire my ESP to the RS232 port?](#How-do-I-wire-my-ESP-to-the-RS232-port)
- [ESPHome configuration YAML](#ESPHome-configuration-YAML)
  - [A note on logging](#A-note-on-logging)
  - [8266-specific preamble](#8266-specific-preamble)
  - [external_components](#external_components)
  - [UART and pace_bms](#UART-and-pace_bms)
  - [Exposing the sensors (this is the good part!)](#Exposing-the-sensors-this-is-the-good-part)
    - [All read-only values](#All-read-only-values)
    - [Read-write values](#Read-write-values)
    - [Read-write values - Protocol Version 25 ONLY](#Read-write-values---Protocol-Version-25-ONLY)
  - [Example Config Files (in full)](#Example-Config-Files-in-full)
- [How to configure a battery pack that's not in the supported list (yet)](#how-to-configure-a-battery-pack-thats-not-in-the-supported-list-yet)
- [Decoding the Status Values (but you probably don't want to)](#decoding-the-status-values-but-you-probably-dont-want-to)
  - [Paceic Version 25 RAW Status Values](#Paceic-Version-25-RAW-Status-Values)
  - [Paceic Version 20 RAW Status Values: PYLON variant](#Paceic-Version-20-RAW-Status-Values-PYLON-variant)
  - [Paceic Version 20 RAW Status Values: SEPLOS variant](#Paceic-Version-20-RAW-Status-Values-SEPLOS-variant)
  - [Paceic Version 20 RAW Status Values: EG4 variant](#Paceic-Version-20-RAW-Status-Values-EG4-variant)
- [I'm having a problem using this component](#Im-having-a-problem-using-this-component)
- [Miscellaneous Notes](#Miscellaneous-Notes)
- [Helping Out](#Helping-Out)

Example PACE BMS board:
![Example PACE BMS board (front)](images/example-board-front.png)
![Example PACE BMS board (back)](images/example-board-back.png)


# What Is Paceic Protocol Version 20

This protocol version is spoken by older battery packs and has several variants, with firmware customized by individual manufacturers.  Three protocol variants are currently known/supported:
 - **EG4**
 - **PYLON**
 - **SEPLOS**

There is a high likelihood that one of these three version 20 protocol variants implemented by pace_bms will work for version 20 battery packs which are branded by a different manufacturer than the three listed.  I only named them for those manufacturers because I was able to find spec docs under those names - the same firmware / protocol variant may be used by many different brands.  But if you can find a spec doc for a new variant that behaves differently, I can probably add support for that too.  See [here](https://github.com/nkinnan/esphome-pace-bms/tree/main/protocol_documentation/paceic/0x20) for documentation on currently known version 20 protocol variants. 

Different manufacturers will have different BMS management software for version 20 battery packs.  It will not be PbmsTools, but one of a variety of manufacturer-specific affronts to good software design.  If your pack uses PbmsTools it's version 25.

The password for "Qualtech 48100 Test", the BMS management software for EG4 LIFEPOWER4 is "888888".  But there are legitimately some settings you shouldn't mess with - basically most of the stuff under the Config tab, but I wouldn't mess with overriding the firmware upgrade settings either).

These older BMSes will usually have two RS485 ports (looks like an ethernet socket) and may have an RS232 port (looks like a telephone socket).  They usually won't have a CAN bus port.

Example protocol version 20 BMS front-panel:
![EG4 Protocol 20 Front Panel](images/EG4-0x20.webp)

# What Is Paceic Protocol Version 25

This version seems more standardized, with an official protocol specification from PACE itself and a relatively consistent software management interface.  As far as I know, all newer battery packs speaking this protocol version should be supported.  See [here](https://github.com/nkinnan/esphome-pace-bms/tree/main/protocol_documentation/paceic/0x25) for documentation on protocol version 25.

These BMSes speaking paceic version 25 will invariably use PbmsTools for their BMS management software (or a rebadged version of it) which looks like this:

![PbmsTools Screenshot](images/PbmsTools.jpg)

The exact look isn't important, just that the tabs and general layout looks like this.  This is PbmsTools regardless of any specific brand badging or interface tweaks, and indicates that your BMS supports protocol version 25.  

The default password to unlock all settings is "123456" incidentally.  But there are legitimately some settings you shouldn't mess with (I didn't implement those in this component - basically just the calibration stuff under the System Config tab).

These BMSes will typically have two RS485 ports (looks like an ethernet socket) an RS232 port (looks like a telephone socket) and possibly a CAN bus port and an LCD display as well, especially if newer.

Example protocol version 25 BMS front-panel:
![Jakiper Protocol 25 Front Panel](images/Jakiper-0x25.png)

# What Is Pace MODBUS Protocol

Some BMS firmwares also support reading data via MODBUS protocol over the RS485 port.  I haven't looked into this yet.  It seems like it may co-exist with Paceic version 25.  Documentation can be found [here](https://github.com/nkinnan/esphome-pace-bms/tree/main/protocol_documentation/modbus).  I may add support for this later, but since documentation is available, ESPHome already has native support for MODBUS, and syssi has already created an [ESPHome configuration for it](https://github.com/syssi/esphome-pace-bms), it's low priority.

# Supported BMS Sensors (read only)

- All "Analog Information"
	- **Cell Count**
	- **Cell Voltage** (V) - up to x16 depending on your battery pack
	- **Temperature Count**
	- **Temperature** (°C) - up to x6 depending on your battery pack, typically this will be:
		-  *Cell Temperature* 1-4 
		- *MOSFET Temperature* 
		- *Environment Temperature*
	- **Total Voltage** (V)
	- **Current** (A) - positive or negative for charge/discharge
	- **Power** (W) - positive or negative for charge/discharge
	- **Remaining Capacity** (Ah)
	- **Full Capacity** (Ah)
	- **Design Capacity** (Ah)
	- **State of Charge** (%)
	- **State of Health** (%)
	- **Cycle Count**
	- **Minimum Cell Voltage** (V)
	- **Maximum Cell Voltage** (V)
	- **Average Cell Voltage** (V)
	- **Max Cell Differential** (V) - difference between minimum and maximum cell voltage
- All "Status Information" decoded to human-readable text format
	- **Warning Text** - A list of any warnings reported by the BMS
	- **Protection Text** - If the BMS has protected itself or the batteries, for example disabling charging if the temperature is too low, or a cell voltage is too high, it will be listed here
	- **Fault Text** - A list of any faults reported by the BMS
	- **System Text** - Current system status such as "Charging"
	- **Configuration Text** - System configuration such as "Warning Buzzer Enabled"
	- **Balancing Text** - If any cells are currently balancing, they will be listed here
	- (individual status flag values) - These are what the text fields are decoded from, and are documented separately.  You probably won't need them, but they are available.  There are a lot of them, and they vary by protocol version and variant.
- **Hardware Version** - The BMS hardware version (string)
- **Serial Number** - The BMS serial number (string)

# Supported BMS Configuration (read / write)

- **System Date and Time** - Allows access to the BMS internal real-time clock 
- **Shutdown** - A button which sends the shutdown command to the BMS

# Supported BMS Configuration (read / write) - **Protocol Version 25 ONLY**

It is difficult to find good documentation on either of these protocols.  All the references I have are incomplete.  For version 25 I was able to snoop on the exchanges between PbmsTools and my battery pack in order to decode all of the commands necessary for setting these configuration values.  However, the only battery pack I own which speaks version 20, is sending some very strange non-paceic commands for configuration settings.  Unfortunately I was unable to decode those, and even if I did, I'm not sure if it would apply to all brands of battery pack speaking version 20.  For that reason, I didn't pursue it further, and these settings are only applicable to battery packs speaking paceic version 25.

- Toggles (switches) that turn various features on/off
	- **Buzzer Alarm**
	- **LED Alarm**
	- **Charge Current Limiter**
	- **Charge MOSFET**
	- **Discharge MOSFET**
- Selects (drop-lists) that allow configuring various features
	- **Charge Current Limiter Gear** - set to High or Low
	- **Protocol (CAN)** - Allows selection of various protocols spoken on the CAN bus, typically to match your inverter
	- **Protocol (RS485)** - Allows selection of various protocols spoken on the RS485 bus, typically to match your inverter
	- **Protocol Type** - Auto or Manual
- Configuration
  - Cell Over Voltage
	- **Cell Over Voltage Alarm** (V)
	- **Cell Over Voltage Protection** (V)
	- **Cell Over Voltage Protection Release** (V)
	- **Cell Over Voltage Delay** (seconds)
  - Pack Over Voltage
	- **Pack Over Voltage Alarm** (V)
	- **Pack Over Voltage Protection** (V)
	- **Pack Over Voltage Protection Release** (V)
	- **Pack Over Voltage Delay** (seconds)
  - Cell Under Voltage
	- **Cell Under Voltage Alarm** (V)
	- **Cell Under Voltage Protection** (V)
	- **Cell Under Voltage Protection Release** (V)
	- **Cell Under Voltage Delay** (seconds)
  - Pack Under Voltage
	- **Pack Under Voltage Alarm** (V)
	- **Pack Under Voltage Protection** (V)
	- **Pack Under Voltage Protection Release** (V)
	- **Pack Under Voltage Delay** (seconds)
  - Discharge Over Current 1
	- **Discharge Over Current 1 Alarm** (A)
	- **Discharge Over Current 1 Protection** (A)
	- **Discharge Over Current 1 Delay** (seconds)
  - Discharge Over Current 2
	- **Discharge Over Current 2 Protection** (A)
	- **Discharge Over Current 2 Delay** (seconds)
  - Discharge Short Circuit
	- **Discharge Short Circuit Protection Delay** (milliseconds)
  - Cell Balancing
	- **Cell Balancing Threshold** (V)
	- **Cell Balancing Delta** (V)
  - Sleep
	- **Sleep Cell Voltage** (V)
	- **Sleep Delay** (minutes)
  - Full Charge
	- **Full Charge Voltage** (V)
	- **Full Charge Amps** (A)
  - Low Charge
	- **Low Charge Alarm** (%)
  - Charge Over Temperature
	- **Charge Over Temperature Alarm** (°C)
	- **Charge Over Temperature Protection** (°C)
	- **Charge Over Temperature Protection Release** (°C)
  - Discharge Over Temperature
	- **Discharge Over Temperature Alarm** (°C)
	- **Discharge Over Temperature Protection** (°C)
	- **Discharge Over Temperature Protection Release** (°C)
  - Charge Under Temperature
	- **Charge Under Temperature Alarm** (°C)
	- **Charge Under Temperature Protection** (°C)
	- **Charge Under Temperature Protection Release** (°C)
  - Discharge Under Temperature
	- **Discharge Under Temperature Alarm** (°C)
	- **Discharge Under Temperature Protection** (°C)
	- **Discharge Under Temperature Protection Release** (°C)
  - MOSFET Over Temperature
	- **MOSFET Over Temperature Alarm** (°C)
	- **MOSFET Over Temperature Protection** (°C)
	- **MOSFET Over Temperature Protection Release** (°C)
  - Environment Over Temperature
	- **Environment Over Temperature Alarm** (°C)
	- **Environment Over Temperature Protection** (°C)
	- **Environment Over Temperature Protection Release** (°C)
  - Environment Under Temperature
	- **Environment Under Temperature Alarm** (°C)
	- **Environment Under Temperature Protection** (°C)
	- **Environment Under Temperature Protection Release** (°C)

# What Battery Packs are Supported?

**As far as I know, many/most.**  Any not listed should simply require a slightly different configuration (or might re-use one of the existing ones).  

However, I'd like to keep a full list here if only for search engine discoverability, so if you find that it does work with your battery pack, please contact me with the configuration settings required, the make/model of battery pack (and a link to the exact model on the manufacturer's website if possible), and what it reports for the hardware version.

**If not listed**, for help figuring out the required settings to get your battery pack working, see [How to configure a battery pack that's not in the supported list (yet)](#how-to-configure-a-battery-pack-thats-not-in-the-supported-list-yet)

Known working protocol version 20 battery packs:
-

- **EG4 LIFEPOWER4**
  - hardware versions: 
	  - **QT-YS00-16SV100A-V3.6** aka QT_PBMS_EPBMS_48100_16S aka QT-EPBMS-48100-16S100Z0 aka QT-PBMS-48100
	  - Related hardware versions: 48100-22S100Z0, 48500-16S500Z0, 24200-8S100Z0, etc.
	    - ![EG4 LIFEPOWER4](images/EG4-0x20-320.png)
  - required `pace_bms` config: 
	  - `protocol_commandset: 0x20`
	  - `protocol_variant: "EG4"`
	  - `battery_chemistry: 0x4A`
  - notes:
	  - Although the protocol documentation I have is labeled EG4, the BMS is manufactured by [Qualtech](http://qualtech.com.cn/en/).  Any battery pack using the same BMS would be supported.
	  - The BMS is a bit slow, so don't reduce the timeouts too much. I have found the following settings prevent lockup from querying it too quickly:
		  -   `request_throttle: 200ms`
		  -   `response_timeout: 2000ms`


Known working protocol version 25 battery packs:
-

- **Jakiper JK48V100**
  - BMS hardware versions: 
	  - **P16S100A-1812-1.00**
	    - ![EG4 LIFEPOWER4](images/Jakiper-0x25-320.png)
  - required `pace_bms` config: 
	  - `protocol_commandset: 0x25`

# What ESPs are Supported?

Both ESP8266 and ESP32 are supported, though an ESP32 class device is recommended.  

Any board which gives you access to a hardware UART (both RX and TX) is fine.  Software UART on GPIO pins is not recommended.  

You cannot connect the UART RX/TX pins directly to either the RS232 or RS485 port, a converter chip for RS485 or RS232 signal levels is required.  Some boards may have such a converter chip built-in, or you can use a breakout board.  

RS485 will require at least one additional GPIO pin for flow control in addition to the UART RX and TX pins.  RS232 will require only the UART RX and TX.

If using an 8266, you will need to redirect serial logs to the second UART (which is TX only, but that's fine for logging).  An example config for that is included below in the YAML section.

# How do I wire my ESP to the RS485 port?

You will need a converter chip.  I have had success with the MAX485.  It's designed for 5v but I've had no issues using it at 3.3v with an ESP.  [Here](https://www.amazon.com/gp/product/B00NIOLNAG) is an example breakout board for the MAX485 chip.  You may be able to find ESP boards with such a chip already integrated.

![MAX485 Breakout Board](images/max485.jpg)

This example breakout board separates out the flow control pins **DE** and **R̅E̅**, but they need to be tied together which you can do by either bridging the solder blobs on the back of the pins, or otherwise wiring both pins together.  

Connect the breakout board to the **ESP**:
* **DI** (driver input) **->** ESP UART **TX** 
* **RO** (receiver output) **->** ESP UART **RX**
* **DE** (Driver Output Enable) and **R̅E̅** (Receiver Output Enable, active low) -> any ESP **GPIO**, from here out referred to as "the **Flow Control** pin"

Connect the breakout board to the **BMS**:
* **A** aka **D+** -> pin / blade **7** (white with a brown stripe if using ethernet cabling)
* **B** aka **D-** -> pin / blade **8** (solid brown if using ethernet cabling)

![RJ-45 Socket and Connector with Pin Numbers and Color Codes](images/rj45.png)

Lastly, don't forget to connect power (3.3v) and ground to the breakout board.

# How do I wire my ESP to the RS232 port?

You will need a converter chip.  I have had success with the SP232.  It's compatible with the ESP 3.3v power and signaling levels.  [Here](https://www.amazon.com/gp/product/B091TN2ZPY) is an example breakout board for the SP232 chip.  You may be able to find ESP boards with such a chip already integrated.

![SP232 Breakout Board](images/sp232.jpg)

Connect the breakout board to the **ESP**:
* TTL **RXD** **->** ESP UART **TX**
* TTL **TXD** **->** ESP UART **RX** 

Connect the breakout board to the **BMS**:
* RS232 **RXD** -> pin / blade **3** 
* RS232 **TXD** -> pin / blade **4** 
* RS232 **GND** -> pin / blade **5** 

**DON'T TRUST THE COLOR CODES** in this diagram, telephone cables are "straight through" and colors will be "mirrored" between the two ends of an extension cord.  Plus the wire colors aren't always standard.  **Use the pin/blade numbering** from the diagram for wiring the proper connections.  

Note that pin/blade **1 and 6 are usually left blank** but **STILL COUNT** for numbering!  

If cutting up a telephone extension cord, make sure it's "**dual line**" / has four wires and not just two.

![RJ-11 Socket and Connector with Pin Numbers and Color Codes](images/rj11.png)

Lastly, don't forget to connect power (3.3v) and ground to the breakout board.

# ESPHome configuration YAML

A full ESPHome configuration will consist of thee parts:

1. The basic stuff like board type, wifi credentials, api or mqtt configuration, web_server if used, and so on. 
3. Configuration of the UART and the pace_bms component to speak with your battery pack.
4. Exposing the sensors / configuration values that you want accessible via the web_server dashboard, homeassistant, or mqtt.

I won't go over 1 since that will be specific to your setup, except to say that if you want to use `web_server` then you should probably add `version: 3` and click the dark mode icon whenever you open it up.  It is a *significant* improvement over version 2, but not yet set as the default, and some of the color choices indicate it was designed in dark mode since they don't work as well with a white background - mainly the logging colors.

sub-sections:
- [A note on logging](#A-note-on-logging)
- [8266-specific preamble](#8266-specific-preamble)
- [external_components](#external_components)
- [UART and pace_bms](#UART-and-pace_bms)
- [Exposing the sensors (this is the good part!)](#Exposing-the-sensors-this-is-the-good-part)
  - [All read-only values](#All-read-only-values)
  - [Read-write values](#Read-write-values)
  - [Read-write values - Protocol Version 25 ONLY](#Read-write-values---Protocol-Version-25-ONLY)
- [Example Config Files (in full)](#Example-Config-Files-in-full)

## A note on logging

While initially setting up this component, I'd strongly recommend setting log level to VERY_VERBOSE.  You can reduce that back to INFO or higher once you confirm everything is working.  If you want to submit logs on an issue report, please gather them with log level VERY_VERBOSE as that will include the actual strings sent to/from the BMS over the UART.  You might want to remove many/most of the sensors when running at the VERY_VERBOSE level however, as a sensors publishing new values generates a **lot** of log output, and it's mainly the component logs that are important, not the sensor logs.

```yaml
logger:
  #level: INFO
  #level: DEBUG
  #level: VERBOSE
  level: VERY_VERBOSE
```
Additionally, if you want to get serial logs over USB on a C3, S2 and S3, you should add this to your logger config:

```yaml
logger:
  # needed for C3, S2 and S3 (and possibly others) to read logs via USB, depending on framework used
  hardware_uart: USB_CDC 
```

## 8266-specific preamble

1) If using an 8266 in conjunction with web_server, you will want to add this to your esphome config.  It **massively** speeds up how quickly the 8266 can speak with the web_server dashboard by correcting a bug in the web server code.  Once [this PR](https://github.com/esphome/ESPAsyncWebServer/pull/41) goes through these lines can be removed.
```yaml
esphome:
  libraries:
    # massive improvement to event throughput to the on-device web_server dashboard
    - ESPAsyncWebServer-esphome=https://github.com/nkinnan/ESPAsyncWebServer#async_event_source_yield
```
2) Since an 8266 only has 1.5 UARTs (a full UART 0 with rx+tx and half of a UART 1 with tx only) we need to redirect log output to UART 1 so we can fully utilize UART 0 for communication with the BMS.  You can do that like so:
```yaml
logger:
  hardware_uart: UART1 # using UART0 for BMS communications
```

## external_components

Before anything else, you will need to tell ESPHome where to find this component.  Add the following lines to your YAML:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/nkinnan/esphome-pace-bms
      ref: "main"
    components: [ pace_bms ]
    refresh: 1s
```
If you're exposing the BMS date/time sensor you might also need to include this:
```yaml
external_components:
  - source:
      type: git
      url: https://github.com/nkinnan/esphome
      ref: "make_time_dependency_optional"
    components: [ datetime ]
    refresh: 1s
```

This second source section is needed to work around a design bug in the ESPHome DateTime component, it will be removed / become unnecessary once [the PR](https://github.com/esphome/esphome/pull/7425) to fix that goes through.

## UART and pace_bms

Next, lets configure the UART and pace_bms component to speak with your BMS.

```yaml
uart:
  id: uart_0
  baud_rate: 9600 
  tx_pin: GPIO2
  rx_pin: GPIO1
  rx_buffer_size: 256
```
* **baud_rate:** The most common value for baud_rate is 9600, but some BMSes are reported to use 19200 as well.  You should know what this value is from previously communicating with the BMS using the manufacturer's recommended software.
* **tx_pin / rx_pin:** Self-explanatory, see previous sections on wiring your ESP to the RS232 or RS485 port. 
* **rx_buffer_size:** A minimum size of 256 is required for this component to function reliably.
```yaml
pace_bms:
  id: pace_bms_at_address_1
  address: 1
  uart_id: uart_0
  flow_control_pin: GPIO0 
  update_interval: 5s
  request_throttle: 200ms 
  response_timeout: 2000ms 

  protocol_commandset: 0x20 # example only
  protocol_variant: "EG4"   # example only
  protocol_version: 0x20    # example only
  battery_chemistry: 0x4A   # example only
```
* **address:** This is the address of your BMS, set with the DIP switches on the front next to the RS232 and RS485 ports.  **Important:** If you change the value of the DIP switches, you'll need to reset the BMS for the new address to take effect.  Either by flipping the breaker, or using something like a toothpick or push-pin to depress the recessed reset button.  The most common address values are 0 and 1, unless your battery packs are daisy chained, which is not currently supported by this component.
* **uart_id:** The ID of the UART you configured.  This component currently requires one UART per BMS, though I'm considering a design change that would allow it to read "daisy chained" BMSes in the future.
* **flow_control_pin:** If using RS232 this setting should be omitted.  If using RS485, this is required to be set, as it controls the direction of communication on the RS485 bus.  It should be connected to *both* the **DE** (Driver Output Enable) and **R̅E̅** (Receiver Output Enable, active low) pins on the RS485 adapter / breakout board.
* **update_interval:** How often to query the BMS and publish whatever updated values are read back.  What queries are sent to the BMS is determined by what values you have requested to be published in [the rest of your configuration](#Exposing-the-sensors-this-is-the-good-part).
* **request_throttle:** Minimum interval between sending requests to the BMS.  Increasing this may help if your BMS "locks up" after a while, it's probably getting overwhelmed.
* **response_timeout:** Maximum time to wait for a response before "giving up" and sending the next.  Increasing this may help if your BMS "locks up" after a while, it's probably getting overwhelmed.
* **protocol_commandset, protocol_variant, protocol_version,** and **battery_chemistry:** 
   - Consider these as a set.  Use values from the [known supported list](#What-Battery-Packs-are-Supported), or determine them manually by following the steps in [How to configure a battery pack that's not in the supported list (yet)](#how-to-configure-a-battery-pack-thats-not-in-the-supported-list-yet)

## Exposing the sensors (this is the good part!)

Next, lets go over making things available to the web_server dashboard, homeassistant, or mqtt.  This is going to differ slightly depending on what data you want to read back from the BMS, I will provide a complete example which you can pare down to only what you want to see.

### All read-only values
```yaml
sensor:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1

    cell_count:
      name: "Cell Count"

    cell_voltage_01:
      name: "Cell Voltage 01"
    cell_voltage_02:
      name: "Cell Voltage 02"
    cell_voltage_03:
      name: "Cell Voltage 03"
    cell_voltage_04:
      name: "Cell Voltage 04"
    cell_voltage_05:
      name: "Cell Voltage 05"
    cell_voltage_06:
      name: "Cell Voltage 06"
    cell_voltage_07:
      name: "Cell Voltage 07"
    cell_voltage_08:
      name: "Cell Voltage 08" 
    cell_voltage_09:
      name: "Cell Voltage 09"
    cell_voltage_10:
      name: "Cell Voltage 10"
    cell_voltage_11:
      name: "Cell Voltage 11"
    cell_voltage_12:
      name: "Cell Voltage 12"
    cell_voltage_13:
      name: "Cell Voltage 13"
    cell_voltage_14:
      name: "Cell Voltage 14"
    cell_voltage_15:
      name: "Cell Voltage 15"
    cell_voltage_16:
      name: "Cell Voltage 16"

    temperature_count:
      name: "Temperature Count"

    # Generally the first four temperatures are cell measurements and the last two are 
    # MOSFET / Environment or Environment / MOSFET with the order of those two depending on manufacturer
    temperature_01:
      name: "Cell Temperature 1"
    temperature_02:
      name: "Cell Temperature 2"
    temperature_03:
      name: "Cell Temperature 3"
    temperature_04:
      name: "Cell Temperature 4"
    temperature_05:
      name: "MOSFET Temperature"
    temperature_06:
      name: "Environment Temperature"

    total_voltage:
      name: "Total Voltage"
    current:
      name: "Current"
    power:
      name: "Power"

    remaining_capacity:
      name: "Remaining Capacity"
    full_capacity:
      name: "Full Capacity"
    design_capacity: # not available on EG4 protocol 0x20 variant
      name: "Design Capacity"

    state_of_charge:
      name: "State of Charge"
    state_of_health:
      name: "State of Health"
    cycle_count:
      name: "Cycle Count"

    min_cell_voltage:
      name: "Min Cell Voltage"
    max_cell_voltage:
      name: "Max Cell Voltage"
    avg_cell_voltage:
      name: "Avg Cell Voltage"
    max_cell_differential:
      name: "Max Cell Differential"

text_sensor:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1

    hardware_version:
      name: "Hardware Version"
    serial_number: # not available on EG4 protocol 0x20 variant
      name: "Serial Number"

    # pre-decoded human readable text strings that describe all of the specific status values and are suitable for display
    warning_status:
      name: "Warning Status"
    protection_status:
      name: "Protection Status"
    fault_status:
      name: "Fault Status"
    system_status:
      name: "System Status"
    configuration_status:
      name: "Configuration Status"
    balancing_status:
      name: "Balancing Status"

```

### Read-write values

```yaml
datetime:
 - platform: pace_bms
   pace_bms_id: pace_bms_at_address_1

   system_date_and_time:
     name: "System Date and Time"

button:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1

    shutdown:
      name: "Shutdown" # will actually "reboot" if the battery is charging/discharging - it only stays shut down if idle
```
### Read-write values - Protocol Version 25 ONLY

```yaml
switch:
 - platform: pace_bms
   pace_bms_id: pace_bms_at_address_1

   buzzer_alarm:
     name: "Buzzer Alarm"
   led_alarm:
     name: "Led Alarm"
   charge_current_limiter:
     name: "Charge Current Limiter"
   charge_mosfet:
     name: "Charge Mosfet"
   discharge_mosfet:
     name: "Discharge Mosfet"


select:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1

    charge_current_limiter_gear:
      name: "Charge Current Limiter Gear"

    # setting the protocol is possible on some version 25 BMSes but not all
    protocol_can:
      name: "Protocol (CAN)"
    protocol_rs485:
      name: "Protocol (RS485)"
    protocol_type:
      name: "Protocol (Type)"


number:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1
 
    cell_over_voltage_alarm:
      name: "Cell Over Voltage Alarm" 
    cell_over_voltage_protection:
      name: "Cell Over Voltage Protection" 
    cell_over_voltage_protection_release:
      name: "Cell Over Voltage Protection Release" 
    cell_over_voltage_protection_delay:
      name: "Cell Over Voltage Protection Delay" 
 
    pack_over_voltage_alarm:
      name: "Pack Over Voltage Alarm" 
    pack_over_voltage_protection:
      name: "Pack Over Voltage Protection" 
    pack_over_voltage_protection_release:
      name: "Pack Over Voltage Protection Release" 
    pack_over_voltage_protection_delay:
      name: "Pack Over Voltage Protection Delay" 
 
    cell_under_voltage_alarm:
      name: "Cell Under Voltage Alarm" 
    cell_under_voltage_protection:
      name: "Cell Under Voltage Protection" 
    cell_under_voltage_protection_release:
      name: "Cell Under Voltage Protection Release" 
    cell_under_voltage_protection_delay:
      name: "Cell Under Voltage Protection Delay" 
 
    pack_under_voltage_alarm:
      name: "Pack Under Voltage Alarm" 
    pack_under_voltage_protection:
      name: "Pack Under Voltage Protection" 
    pack_under_voltage_protection_release:
      name: "Pack Under Voltage Protection Release" 
    pack_under_voltage_protection_delay:
      name: "Pack Under Voltage Protection Delay" 
 
    charge_over_current_alarm:
      name: "Charge Over Current Alarm" 
    charge_over_current_protection:
      name: "Charge Over Current Protection" 
    charge_over_current_protection_delay:
      name: "Charge Over Current Protection Delay" 
 
    discharge_over_current1_alarm:
      name: "Discharge Over Current 1 Alarm" 
    discharge_over_current1_protection:
      name: "Discharge Over Current 1 Protection" 
    discharge_over_current1_protection_delay:
      name: "Discharge Over Current 1 Protection Delay" 
 
    discharge_over_current2_protection:
      name: "Discharge Over Current 2 Protection" 
    discharge_over_current2_protection_delay:
      name: "Discharge Over Current 2 Protection Delay" 
 
    short_circuit_protection_delay:
      name: "Short Circuit Protection Delay (Milliseconds)" 
 
    cell_balancing_threshold:
      name: "Cell Balancing Threshold"
    cell_balancing_delta:
      name: "Cell Balancing Delta"
 
    sleep_cell_voltage:
      name: "Sleep Cell Voltage"
    sleep_delay:
      name: "Sleep Delay (Minutes)"
 
    full_charge_voltage:
      name: "Full Charge Voltage"
    full_charge_amps:
      name: "Full Charge Amps"
    low_charge_alarm_percent:
      name: "Low Charge Alarm Percent"
 
    charge_over_temperature_alarm:
      name: "Charge Over Temperature Alarm"
    charge_over_temperature_protection:
      name: "Charge Over Temperature Protection"
    charge_over_temperature_protection_release:
      name: "Charge Over Temperature Protection Release"
 
    discharge_over_temperature_alarm:
      name: "Discharge Over Temperature Alarm"
    discharge_over_temperature_protection:
      name: "Discharge Over Temperature Protection"
    discharge_over_temperature_protection_release:
      name: "Discharge Over Temperature Protection Release"
 
    charge_under_temperature_alarm:
      name: "Charge Under Temperature Alarm"
    charge_under_temperature_protection:
      name: "Charge Under Temperature Protection"
    charge_under_temperature_protection_release:
      name: "Charge Under Temperature Protection Release"
 
    discharge_under_temperature_alarm:
      name: "Discharge Under Temperature Alarm"
    discharge_under_temperature_protection:
      name: "Discharge Under Temperature Protection"
    discharge_under_temperature_protection_release:
      name: "Discharge Under Temperature Protection Release"
 
    mosfet_over_temperature_alarm:
      name: "Mosfet Over Temperature Alarm"
    mosfet_over_temperature_protection:
      name: "Mosfet Over Temperature Protection"
    mosfet_over_temperature_protection_release:
      name: "Mosfet Over Temperature Protection Release"
 
    environment_over_temperature_alarm:
      name: "Environment Over Temperature Alarm"
    environment_over_temperature_protection:
      name: "Environment Over Temperature Protection"
    environment_over_temperature_protection_release:
      name: "Environment Over Temperature Protection Release"
 
    environment_under_temperature_alarm:
      name: "Environment Under Temperature Alarm"
    environment_under_temperature_protection:
      name: "Environment Under Temperature Protection"
    environment_under_temperature_protection_release:
      name: "Environment Under Temperature Protection Release"
```
## Example Config Files (in full)

If you already have a config for your board, you should use that, and then copy/paste/modify the relevant parts of [ESPHome configuration YAML](#ESPHome-configuration-YAML).  You'll need to read that anyway to understand what these files contain.  But here are some basic configs if starting from scratch.  The main difference between them is just the board declaration (and the 8266-specific settings as noted in [ESPHome configuration YAML](#ESPHome-configuration-YAML))

### Protocol 25

- ESP8266
	- todo
- ESP32
	- [esp32dev-0x25-full.yaml](esp32dev-0x25-full.yaml) - everything, including BMS configuration settings
	- [esp32dev-0x25-sensors_only.yaml](esp32dev-0x25-sensors_only.yaml) - use this if you only want to read the sensor values
- ESP32-C3
	- todo

### Protocol 20, EG4 variant

- ESP8266
	- todo
- ESP32
	- [esp32dev-0x20-EG4.yaml](esp32dev-0x20-EG4.yaml)
- ESP32-C3
	- todo

# How to configure a battery pack that's not in the supported list (yet)

Before proceeding through this section, please read the entire rest of this document first!  It assumes some familiarity and does not repeat steps like configuring the UART, but simply provides a guide on how to determine the specific protocol your BMS is speaking.

If your battery pack has a front panel that "looks like" a Pace BMS but is not in the "known supported" list, it probably is, and is probably supported.  Unless there are more version 20 variants out there than I've guessed, but even then you should be able to get some useful data back.  So you just need to figure out what settings will enable this component to speak with it.

Step 0 (shortcut): Just try protocol version 25 with defaults
-
This is the version "spoken" by (I believe) the majority of packs sold today.   You can short circuit any extra steps by simply trying this:

```yaml
pace_bms:
  <...etc...>
  protocol_commandset: 0x20

text_sensor:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1

    hardware_version:
      name: "Hardware Version"
    serial_number:
      name: "Serial Number"
```

If it worked, you're basically done.  Just fill out your YAML with [the rest of the settings / readouts you want exposed](#Exposing-the-sensors-this-is-the-good-part) and you can skip the rest of this section.  Please contact me with your make/model/hardware version as well as the settings you used so that I can add it to the known supported list.

If it didn't, no worries, just continue reading.

Step 1: Is the BMS speaking paceic?
-
The first step is to make sure it's communicating at all.  If you can't connect the battery manufacturer's BMS software to it and get readings back, don't proceed any further until you can.  There's no point trying to debug a dead port or a broken BMS.  You can try both RS232 and RS485.  One or the other may not be "active".  The RS232 port if available is the most likely to be speaking paceic (different ports may be configured to speak different protocols).  

Once your manufacturer's recommended software is talking to your battery pack successfully, if you're on Windows, you can use [this software](https://www.com-port-monitoring.com/downloads.html) to "snoop" on the COM port and see what the protocol looks like.  Linux or Mac should have equivalents available but I'm not familiar with them.  You should see something like this (make sure you're in "text" mode):

```~25xx46xxxxxxxxxx\r```
or
```~20xx46xxxxxxxxxx\r```

The values may be slightly different.  The x's will be hexidecimal numbers (in fact, all values are ASCII text hexidecimal).  The \r may or may not be visible, it might just show up as a line return in whatever software you're using to snoop on the COM port.  If it looks nothing like that at all, sorry, you're out of luck.  If some of the requests look like that and other's don't, that's fine, continue on as long as at least some of them do.

Step 2: Understanding what we need
-
We need at least one and as many as four configuration values to speak with the BMS successfully:
1) **`protocol_commandset`** - The **actual** protocol version being used, this determines what commands can be sent to the BMS.
2) **`protocol_version`** - The "claimed" version of the protocol - some BMSes lie about what protocol version they are speaking in order to lock you into an ecosystem.  This is the value sent over the wire in the frame header, but which commands can be sent is still determined by `protocol_commandset`
3) **`protocol_variant`** - For protocol commandset 20 only, the "variant" of the protocol this BMS is using.  This determines how some of the BMS responses (to the same command) are interpreted, and can be one of (currently) three supported values:
    * PYLON
    * SEPLOS
    * EG4

    Protocol 25 has no known variants.
4) **`battery_chemistry`** - In almost all cases this will be 0x46, but some manufacturers who intentionally break compatibility will use a different value (or actually legitimately have a different chemistry in some cases).

Step 3: the commandset
-
Now, going back to the requests you snooped over the COM port
```
~25xx46xxxxxxxxxx\r
```
The first number, the 20 or the 25 at the beginning (it may be a different number, more on that in a moment) is the protocol version your BMS is speaking.  The second number (after two other hexidecimal values shown as x's) is your battery chemistry.  Put both of them into your config YAML (you can skip battery_chemistry if it was 46 as expected since that is the default value).  

Note that the "0x" prefix just means "this value is hexidecimal":
 

```yaml
pace_bms:
  protocol_commandset: 0x20
  battery_chemistry: 0x4A # only if not 46
```
or
```yaml
pace_bms:
  protocol_commandset: 0x25
  battery_chemistry: 0x4A # only if not 46
```  

If your commandset value is 0x25 then you're basically done.  Just fill out your YAML with [the rest of the settings / readouts you want exposed](#Exposing-the-sensors-this-is-the-good-part) and you can skip the rest of this section.  Please contact me with your make/model/hardware version as well as the settings you used so that I can add it to the known supported list.

Step 4: If the BMS is lying
-
If the requests you were seeing started with either **20 or 25, skip this step**.

If the requests you were seeing *didn't* start with either 20 or 25, but otherwise "looked right", that means your BMS is using a custom firmware with a non-standard protocol version reported.  That's probably fine, it's probably still speaking version 20 or 25, but is lying about it because some manufacturers intentionally break compatibility, presumably because they want to lock you into their ecosystem.  So you're going to have to try both, and configure pace_bms to lie right back.  Here we'll use 42 as an example of that first number you saw instead of a 20 or 25. 

```yaml
pace_bms:
  protocol_commandset: 0x20
  battery_chemistry: 0x4A # only if not 46
  protocol_version: 0x42 # the BMS is lying, so lie right back
```
or

```yaml
pace_bms:
  protocol_commandset: 0x25
  battery_chemistry: 0x4A # only if not 46
  protocol_version: 0x42 # the BMS is lying, so lie right back
```

If you had to guess which commandset like this, you can figure out if it is "truly" 0x20 or 0x25 simply by seeing if pace_bms starts logging errors or returns good data.  I suggest trying to read these two values first, since there is some overlap between the protocol versions for the analog and status values - so it may not be obvious at first if the data returned is wrong or not.  If the BMS responds to either of these with something intelligible, you have probably picked the correct commandset value.  But keep an eye on the logs for errors and warnings.

```yaml
text_sensor:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1

    hardware_version:
      name: "Hardware Version"
    serial_number:
      name: "Serial Number"
```

Once again, if your "true" commandset value is determined to be 0x25 then you're basically done.  Just fill out your YAML with [the rest of the settings / readouts you want exposed](#Exposing-the-sensors-this-is-the-good-part) and you can skip the rest of this section.  Please contact me with your make/model/hardware version as well as the settings you used so that I can add it to the known supported list.

Step 5: An extra step for commandset 20
-
If you determined the commandset to be 0x20 then you also need to figure out which "variant" of protocol version 20 it is.  Start by putting this into your config:

```yaml
sensor:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1
    
    cell_count:
      name: "Cell Count"

text_sensor:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1

    system_status:
      name: "System Status"
```

This will cause both the "analog information" and "status  information" requests to be sent to the BMS, the results of which contain telltales which pace_bms will sniff out to determine which version 20 protocol variant your BMS is speaking.  

You should see lines like this in the logs, they will be highlighted **green**:  

```[20:08:57][I][pace_bms_protocol:028]: Detected protocol variant: EG4```

Alternatively you may see lines like this instead, highlighted in **yellow**:

```[20:08:57][I][pace_bms_protocol:028]: Protocol variant not configured, unable to auto-detect, defaulting to EG4```

If you got the line highlighted in green, add that to your config and you're done.
```yaml
pace_bms:
  protocol_commandset: 0x20
  protocol_version: 0x42 # the BMS is lying, so lie right back
  battery_chemistry: 0x4A # only if not 46
  protocol_variant: "EG4"
```
If you only got the yellow highlighted line, you're going to have to guess.  Try the following values and see which one gives you the most "correct" data: 
* PYLON
* SEPLOS
* EG4

The problem areas are going to be the last of the analog values such as Cycle Count, State of Charge and State of Health, and all of the status values.  If those don't make sense, or the BMS doesn't respond, it's the wrong protocol variant.  

Once you've figured out the proper protocol variant that returns sensible status values, just fill out your YAML with [the rest of the settings / readouts you want exposed](#Exposing-the-sensors-this-is-the-good-part) and you can skip the rest of this section. Please contact me with your make/model/hardware version as well as the settings you used so that I can add it to the known supported list.

If it didn't work
-
If none of the protocol variants work properly, or you have a different issue following these steps, I'd be interested to hear about it.  You may have a BMS speaking a protocol variant I haven't come across or found documentation for.  Please file an issue and provide me with whatever data you can, including make/model/hardware version (in particular the hardware version reported by pace_bms if you can get it to respond to that request, or from the manufacturer's recommended BMS software if not), and VERY_VERBOSE level logs.  Even better if you can provide me some COM port traces between the manufacturer's software and the BMS or even a protocol spec doc you found by googling your hardware.  I might be able to implement the new variant for you.


# Decoding the Status Values (but you probably don't want to)

No seriously, just use the text values I painstakingly decoded for you :)  This is under a separate heading for a reason.

Really? OK, well here's the thing.  They're completely different for every single protocol version and variant.  Which is why I consolidated them into something you can display and understand.  But you might have a specific use case that necessitates decoding those bit flags yourself, so I did painstakingly expose and document them all.  Lets go over them one by one.

This is going to be tedious, so I'll "cheat" a bit by sharing some raw enums from the code.

sub-sections:
  - [Paceic Version 25 RAW Status Values](#Paceic-Version-25-RAW-Status-Values)
  - [Paceic Version 20 RAW Status Values: PYLON variant](#Paceic-Version-20-RAW-Status-Values-PYLON-variant)
  - [Paceic Version 20 RAW Status Values: SEPLOS variant](#Paceic-Version-20-RAW-Status-Values-SEPLOS-variant)
  - [Paceic Version 20 RAW Status Values: EG4 variant](#Paceic-Version-20-RAW-Status-Values-EG4-variant)


<details>
<summary>

## Paceic Version 25 RAW Status Values

</summary>

First, the full set of YAML config entries:
```yaml
sensor:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1

    # specific raw status values that you probably don't need, but the values / bit flags are documented anyway
    # you can probably just use the 6 text sensor equivalents which encompass all of these values and are suitable for display
    warning_status_value_cell_01:
      name: "Warning Status Value Cell 01"
    warning_status_value_cell_02:
      name: "Warning Status Value Cell 02"
    warning_status_value_cell_03:
      name: "Warning Status Value Cell 03"
    warning_status_value_cell_04:
      name: "Warning Status Value Cell 04"
    warning_status_value_cell_05:
      name: "Warning Status Value Cell 05"
    warning_status_value_cell_06:
      name: "Warning Status Value Cell 06"
    warning_status_value_cell_07:
      name: "Warning Status Value Cell 07"
    warning_status_value_cell_08:
      name: "Warning Status Value Cell 08"
    warning_status_value_cell_09:
      name: "Warning Status Value Cell 09"
    warning_status_value_cell_10:
      name: "Warning Status Value Cell 10"
    warning_status_value_cell_11:
      name: "Warning Status Value Cell 11"
    warning_status_value_cell_12:
      name: "Warning Status Value Cell 12"
    warning_status_value_cell_13:
      name: "Warning Status Value Cell 13"
    warning_status_value_cell_14:
      name: "Warning Status Value Cell 14"
    warning_status_value_cell_15:
      name: "Warning Status Value Cell 15"
    warning_status_value_cell_16:
      name: "Warning Status Value Cell 16"
    
    warning_status_value_temperature_01:
      name: "Warning Status Value Temperature 01"
    warning_status_value_temperature_02:
      name: "Warning Status Value Temperature 02"
    warning_status_value_temperature_03:
      name: "Warning Status Value Temperature 03"
    warning_status_value_temperature_04:
      name: "Warning Status Value Temperature 04"
    warning_status_value_temperature_05:
      name: "Warning Status Value Temperature 05"
    warning_status_value_temperature_06:
      name: "Warning Status Value Temperature 06"
    
    warning_status_value_charge_current:
      name: "Warning Status Value Charge Current"
    warning_status_value_total_voltage:
      name: "Warning Status Value Total Voltage"
    warning_status_value_discharge_current:
      name: "Warning Status Value Discharge Current"
      
    warning_status_value_1:
      name: "Warning Status Value 1"
    warning_status_value_2:
      name: "Warning Status Value 2"
    
    balancing_status_value:
      name: "Balancing Status Value"
    system_status_value:
      name: "System Status Value"
    configuration_status_value:
      name: "Configuration Status Value"
    protection_status_value_1:
      name: "Protection Status Value 1"
    protection_status_value_2:
      name: "Protection Status Value 2"
    fault_status_value:
      name: "Fault Status Value"
```

The entries:
- `warning_status_value_cell_01` through `warning_status_value_cell_16`
- `warning_status_value_temperature_01` through `warning_status_value_temperature_06`
- `warning_status_value_charge_current`
- `warning_status_value_total_voltage`
- `warning_status_value_discharge_current`

All contain a scalar value.  They indicate a warning but not a fault or error (yet) on their respective measurement.  Possible values:

```C++
	enum StatusInformation_WarningValues
	{
		WV_Normal = 0,
		WV_BelowLowerLimitValue = 1,
		WV_AboveUpperLimitValue = 2,
		WV_OtherFaultValue = 0xF0,
	};
```


The entries:
- `warning_status_value_1`
- `warning_status_value_2`

Contain bitflags.  They indicate a warning but not a fault or error (yet).  Possible values:

```C++
	enum StatusInformation_Warning1Flags
	{
		W1F_UndefinedWarning1Bit8 = (1 << 7),
		W1F_UndefinedWarning1Bit7 = (1 << 6),
		W1F_DischargeCurrentBit = (1 << 5),
		W1F_ChargeCurrentBit = (1 << 4),
		W1F_LowTotalVoltageBit = (1 << 3),
		W1F_HighTotalVoltageBit = (1 << 2),
		W1F_LowCellVoltageBit = (1 << 1),
		W1F_HighCellVoltageBit = (1 << 0),
	};
	enum StatusInformation_Warning2Flags
	{
		W2F_LowPower = (1 << 7),
		W2F_HighMosfetTemperature = (1 << 6),
		W2F_LowEnvironmentalTemperature = (1 << 5),
		W2F_HighEnvironmentalTemperature = (1 << 4),
		W2F_LowDischargeTemperature = (1 << 3),
		W2F_LowChargeTemperature = (1 << 2),
		W2F_HighDischargeTemperature = (1 << 1),
		W2F_HighChargeTemperature = (1 << 0),
	};
```

The entry:
- `balancing_status_value`

Contains bitflags.  It is 16 bits wide.  One for each cell.  If the bit is set, it indicates that cell is currently balancing.  Cell 1 is the least significant bit.

The entry:
- `system_status_value`

Contains bitflags.  These flags indicate the current status of the BMS.  Possible values:

```C++
	enum StatusInformation_SystemFlags
	{
		SF_HeartIndicatorBit = (1 << 7),
		SF_UndefinedStatusBit7 = (1 << 6),
		SF_ChargingBit = (1 << 5),
		SF_PositiveNegativeTerminalsReversedBit = (1 << 4),
		SF_DischargingBit = (1 << 3),
		SF_DischargeMosfetOnBit = (1 << 2),
		SF_ChargeMosfetOnBit = (1 << 1),
		SF_ChargeCurrentLimiterTurnedOffBit = (1 << 0), // this is the inverse of CF_ChargeCurrentLimiterEnabledBit
	};
```


The entry:
- `configuration_status_value`

Contains bitflags.  These flags indicate the current configuration of the BMS.  Possible values:

```C++
	enum StatusInformation_ConfigurationFlags
	{
		CF_UndefinedConfigurationStatusBit8 = (1 << 7),
		CF_UndefinedConfigurationStatusBit7 = (1 << 6),
		CF_LedAlarmEnabledBit = (1 << 5),
		CF_ChargeCurrentLimiterEnabledBit = (1 << 4),
		CF_ChargeCurrentLimiterLowGearSetBit = (1 << 3), // if not set, mode is High Gear
		CF_DischargeMosfetTurnedOff = (1 << 2), // it is not documented, but in practice I have seen this flag being set to mean "Discharge MOSFET turned OFF" in addition to the SF_DischargeMosfetOnBit flag being cleared, but it might also be an error
		CF_ChargeMosfetTurnedOff = (1 << 1), // it is not documented, but in practice I have seen this flag being set to mean "Charge MOSFET turned OFF" in addition to the SF_ChargeMosfetOnBit flag being cleared, but it might also be an error
		CF_BuzzerAlarmEnabledBit = (1 << 0),
	};
```


The entry:
- `protection_status_value_1` and `protection_status_value_2`

Contain bitflags.  These flags indicate that action is being taken by the BMS to protect itself.  Possible values:

```C++
	enum StatusInformation_Protection1Flags
	{
		P1F_UndefinedProtect1Bit = (1 << 7),
		P1F_ShortCircuitProtect1Bit = (1 << 6),
		P1F_DischargeCurrentProtect1Bit = (1 << 5),
		P1F_ChargeCurrentProtect1Bit = (1 << 4),
		P1F_LowTotalVoltageProtect1Bit = (1 << 3),
		P1F_HighTotalVoltageProtect1Bit = (1 << 2),
		P1F_LowCellVoltageProtect1Bit = (1 << 1),
		P1F_HighCellVoltageProtect1Bit = (1 << 0),
	};
	enum StatusInformation_Protection2Flags
	{
		P2F_FullyProtect2Bit = (1 << 7),
		P2F_LowEnvironmentalTemperatureProtect2Bit = (1 << 6),
		P2F_HighEnvironmentalTemperatureProtect2Bit = (1 << 5),
		P2F_HighMosfetTemperatureProtect2Bit = (1 << 4),
		P2F_LowDischargeTemperatureProtect2Bit = (1 << 3),
		P2F_LowChargeTemperatureProtect2Bit = (1 << 2),
		P2F_HighDischargeTemperatureProtect2Bit = (1 << 1),
		P2F_HighChargeTemperatureProtect2Bit = (1 << 0),
	};
```


The entry:
- `fault_status_value`

Contains bitflags.  These flags indicate the BMS is faulted, a more serious condition than a protection being enabled.  Generally this means the hardware has failed in some way.  Possible values:

```C++
	enum StatusInformation_FaultFlags
	{
		FF_UndefinedFaultStatusBit8 = (1 << 7),
		FF_UndefinedFaultStatusBit7 = (1 << 6),
		FF_SampleBit = (1 << 5),
		FF_CellBit = (1 << 4),
		FF_UndefinedFaultStatusBit4 = (1 << 3),
		FF_NTCBit = (1 << 2),
		FF_DischargeMosfetBit = (1 << 1),
		FF_ChargeMosfetBit = (1 << 0),
	};
```
</details>
(click header to expand/collapse section)
<details>
<summary>

## Paceic Version 20 RAW Status Values: PYLON variant

</summary>


First, the full set of YAML config entries:
```yaml
sensor:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1

    # specific raw status values that you probably don't need, but the values / bit flags are documented anyway
    # you can probably just use the 6 text sensor equivalents which encompass all of these values and are suitable for display
    warning_status_value_cell_01:
      name: "Warning Status Value Cell 01"
    warning_status_value_cell_02:
      name: "Warning Status Value Cell 02"
    warning_status_value_cell_03:
      name: "Warning Status Value Cell 03"
    warning_status_value_cell_04:
      name: "Warning Status Value Cell 04"
    warning_status_value_cell_05:
      name: "Warning Status Value Cell 05"
    warning_status_value_cell_06:
      name: "Warning Status Value Cell 06"
    warning_status_value_cell_07:
      name: "Warning Status Value Cell 07"
    warning_status_value_cell_08:
      name: "Warning Status Value Cell 08"
    warning_status_value_cell_09:
      name: "Warning Status Value Cell 09"
    warning_status_value_cell_10:
      name: "Warning Status Value Cell 10"
    warning_status_value_cell_11:
      name: "Warning Status Value Cell 11"
    warning_status_value_cell_12:
      name: "Warning Status Value Cell 12"
    warning_status_value_cell_13:
      name: "Warning Status Value Cell 13"
    warning_status_value_cell_14:
      name: "Warning Status Value Cell 14"
    warning_status_value_cell_15:
      name: "Warning Status Value Cell 15"
    warning_status_value_cell_16:
      name: "Warning Status Value Cell 16"
    
    warning_status_value_temperature_01:
      name: "Warning Status Value Temperature 01"
    warning_status_value_temperature_02:
      name: "Warning Status Value Temperature 02"
    warning_status_value_temperature_03:
      name: "Warning Status Value Temperature 03"
    warning_status_value_temperature_04:
      name: "Warning Status Value Temperature 04"
    warning_status_value_temperature_05:
      name: "Warning Status Value Temperature 05"
    warning_status_value_temperature_06:
      name: "Warning Status Value Temperature 06"
    
    warning_status_value_charge_current:
      name: "Warning Status Value Charge Current"
    warning_status_value_total_voltage:
      name: "Warning Status Value Total Voltage"
    warning_status_value_discharge_current:
      name: "Warning Status Value Discharge Current"

    status1_value:
      name: "Status1 Value"
    status2_value:
      name: "Status2 Value"
    status3_value:
      name: "Status3 Value"
    status4_value:
      name: "Status4 Value"
    status5_value:
      name: "Status5 Value"
```

The entries:
- `warning_status_value_cell_01` through `warning_status_value_cell_16`
- `warning_status_value_temperature_01` through `warning_status_value_temperature_06`
- `warning_status_value_charge_current`
- `warning_status_value_total_voltage`
- `warning_status_value_discharge_current`

All contain a scalar value.  They indicate a warning but not a fault or error (yet) on their respective measurement.  Possible values:

```C++
	enum StatusInformation_WarningValues
	{
		WV_Normal = 0,
		WV_BelowLowerLimitValue = 1,
		WV_AboveUpperLimitValue = 2,
		WV_OtherFaultValue = 0xF0,
	};
```

The entry:
- `status1_value`

Contains bitflags.  It's unclear from the documentation whether these are "warning" or "protection" flags.  I chose to implement them as "protection" but if you have a PYLON BMS and can tell me otherwise I'm happy to better classify them.  Possible values:

```C++
		enum StatusInformation_Status1
		{
			S1_PackUnderVoltage = (1 << 7),
			S1_ChargeTemperatureProtection = (1 << 6),
			S1_DischargeTemperatureProtection = (1 << 5),
			S1_DischargeOverCurrent = (1 << 4),
			S1_UndefinedStatus1Bit4 = (1 << 3),
			S1_ChargeOverCurrent = (1 << 2),
			S1_CellUnderVoltage = (1 << 1),
			S1_PackOverVoltage = (1 << 0),
		};
```

The entry:
- `status2_value`

Contains bitflags.  These flags indicate the current configuration of the BMS.  Possible values:

```C++
		enum StatusInformation_Status2
		{
			S2_UndefinedStatus2Bit8 = (1 << 7),
			S2_UndefinedStatus2Bit7 = (1 << 6),
			S2_UndefinedStatus2Bit6 = (1 << 5),
			S2_UndefinedStatus2Bit5 = (1 << 4),
			S2_UsingBatteryPower = (1 << 3),
			S2_DischargeMosfetOn = (1 << 2),
			S2_ChargeMosfetOn = (1 << 1),
			S2_PrechargeMosfetOn = (1 << 0),
		};
```

The entry:
- `status3_value`

Contains bitflags.  These flags indicate the current system state of the BMS.  Possible values:

```C++
		enum StatusInformation_Status3
		{
			S3_Charging = (1 << 7),
			S3_Discharging = (1 << 6),
			S3_HeaterOn = (1 << 5),
			S3_UndefinedStatus3Bit5 = (1 << 4),
			S3_FullyCharged = (1 << 3),
			S3_UndefinedStatus3Bit3 = (1 << 2),
			S3_UndefinedStatus3Bit2 = (1 << 1),
			S3_Buzzer = (1 << 0),
		};
```

The entries:
- `status4_value`
- `status5_value`

Contains bitflags.  These flags indicate a cell fault.  Possible values:

```C++
		enum StatusInformation_Status4
		{
			S4_Cell08Fault = (1 << 7),
			S4_Cell07Fault = (1 << 6),
			S4_Cell06Fault = (1 << 5),
			S4_Cell05Fault = (1 << 4),
			S4_Cell04Fault = (1 << 3),
			S4_Cell03Fault = (1 << 2),
			S4_Cell02Fault = (1 << 1),
			S4_Cell01Fault = (1 << 0),
		};
		enum StatusInformation_Status5
		{
			S5_Cell16Fault = (1 << 7),
			S5_Cell15Fault = (1 << 6),
			S5_Cell14Fault = (1 << 5),
			S5_Cell13Fault = (1 << 4),
			S5_Cell12Fault = (1 << 3),
			S5_Cell11Fault = (1 << 2),
			S5_Cell10Fault = (1 << 1),
			S5_Cell09Fault = (1 << 0),
		};
```
</details>
(click header to expand/collapse section)
<details>
<summary>

## Paceic Version 20 RAW Status Values: SEPLOS variant

</summary>

First, the full set of YAML config entries:
```yaml
sensor:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1

    # specific raw status values that you probably don't need, but the values / bit flags are documented anyway
    # you can probably just use the 6 text sensor equivalents which encompass all of these values and are suitable for display
    warning_status_value_cell_01:
      name: "Warning Status Value Cell 01"
    warning_status_value_cell_02:
      name: "Warning Status Value Cell 02"
    warning_status_value_cell_03:
      name: "Warning Status Value Cell 03"
    warning_status_value_cell_04:
      name: "Warning Status Value Cell 04"
    warning_status_value_cell_05:
      name: "Warning Status Value Cell 05"
    warning_status_value_cell_06:
      name: "Warning Status Value Cell 06"
    warning_status_value_cell_07:
      name: "Warning Status Value Cell 07"
    warning_status_value_cell_08:
      name: "Warning Status Value Cell 08"
    warning_status_value_cell_09:
      name: "Warning Status Value Cell 09"
    warning_status_value_cell_10:
      name: "Warning Status Value Cell 10"
    warning_status_value_cell_11:
      name: "Warning Status Value Cell 11"
    warning_status_value_cell_12:
      name: "Warning Status Value Cell 12"
    warning_status_value_cell_13:
      name: "Warning Status Value Cell 13"
    warning_status_value_cell_14:
      name: "Warning Status Value Cell 14"
    warning_status_value_cell_15:
      name: "Warning Status Value Cell 15"
    warning_status_value_cell_16:
      name: "Warning Status Value Cell 16"
    
    warning_status_value_temperature_01:
      name: "Warning Status Value Temperature 01"
    warning_status_value_temperature_02:
      name: "Warning Status Value Temperature 02"
    warning_status_value_temperature_03:
      name: "Warning Status Value Temperature 03"
    warning_status_value_temperature_04:
      name: "Warning Status Value Temperature 04"
    warning_status_value_temperature_05:
      name: "Warning Status Value Temperature 05"
    warning_status_value_temperature_06:
      name: "Warning Status Value Temperature 06"
    
    warning_status_value_charge_current:
      name: "Warning Status Value Charge Current"
    warning_status_value_total_voltage:
      name: "Warning Status Value Total Voltage"
    warning_status_value_discharge_current:
      name: "Warning Status Value Discharge Current"

    balancing_status_value:
      name: "Balancing Status Value"
    system_status_value:
      name: "System Status Value"
      
    power_status_value:
      name: "Power Status Value"
    disconnection_status_value: 
      name: "Disconnection Status Value"

    warning1_status_value:
      name: "Warning1 Status Value"
    warning2_status_value:
      name: "Warning2 Status Value"
    warning3_status_value:
      name: "Warning3 Status Value"
    warning4_status_value:
      name: "Warning4 Status Value"
    warning5_status_value:
      name: "Warning5 Status Value"
    warning6_status_value:
      name: "Warning6 Status Value"
    warning7_status_value:
      name: "Warning7 Status Value"
    warning8_status_value:
      name: "Warning8 Status Value"
```

The entries:
- `warning_status_value_cell_01` through `warning_status_value_cell_16`
- `warning_status_value_temperature_01` through `warning_status_value_temperature_06`
- `warning_status_value_charge_current` *
- `warning_status_value_discharge_current` *
- `warning_status_value_total_voltage`

All contain a scalar value.  They indicate a warning but not a fault or error (yet) on their respective measurement.  Possible values:

**Important:** * The SEPLOS variant does not differentiate between charge and discharge current warnings, these two fields will have an identical value.

```C++
	enum StatusInformation_WarningValues
	{
		WV_Normal = 0,
		WV_BelowLowerLimitValue = 1,
		WV_AboveUpperLimitValue = 2,
		WV_OtherFaultValue = 0xF0,
	};
```

The entry:
- `balancing_status_value`

Contains bitflags.  It is 16 bits wide.  One for each cell.  If the bit is set, it indicates that cell is currently balancing.  Cell 1 is the least significant bit.

The entry:
- `system_status_value`

Contains bitflags.  These flags indicate the current system status of the BMS.  Possible values:

```C++
		enum StatusInformation_SystemStatus
		{
			SS_ReservedSystemStatusBit8 = (1 << 7),
			SS_ReservedSystemStatusBit7 = (1 << 6),
			SS_PowerOff = (1 << 5),
			SS_Standby = (1 << 4),
			SS_ReservedSystemStatusBit4 = (1 << 3),
			SS_FloatingCharge = (1 << 2),
			SS_Charging = (1 << 1),
			SS_Discharging = (1 << 0),
		};
```

The entry:
- `power_status_value`

Contains bitflags.  These flags indicate the current configuration of the BMS.  Possible values:

```C++
		enum StatusInformation_PowerStatus
		{
			PS_ReservedPowerStatusBit8 = (1 << 7),
			PS_ReservedPowerStatusBit7 = (1 << 6),
			PS_ReservedPowerStatusBit6 = (1 << 5),
			PS_ReservedPowerStatusBit5 = (1 << 4),
			PS_HeatingSwitchStatus = (1 << 3),
			PS_CurrentLimitSwitchStatus = (1 << 2),
			PS_ChargeSwitchStatus = (1 << 1),
			PS_DischargeSwitchStatus = (1 << 0),
		};
```

The entry:
- `disconnection_status_value`

Contains bitflags.  It is 16 bits wide.  One for each cell.  If the bit is set, it indicates that cell is disconnected.  Cell 1 is the least significant bit.

The entry:
- `warning1_status_value`

Contains bitflags.  These flags indicate faults of the BMS.  Possible values:

```C++
		enum StatusInformation_Warning1
		{
			// pretty sure these three mean MOSFET when they say "Switch" in the doc...
			W1_CurrentLimitSwitchFailure = (1 << 7), // fault
			W1_DischaringSwitchFailure = (1 << 6), // fault
			W1_ChargingSwitchFailure = (1 << 5), // fault
			W1_CellVoltageDifferenceSensingFailure = (1 << 4), // fault
			W1_PowerSwitchFailure = (1 << 3), // fault
			W1_CurrentSensingFailure = (1 << 2), // fault
			W1_TemperatureSensingFailure = (1 << 1), // fault
			W1_VoltageSensingFailure = (1 << 0), // fault
		};
```

The entry:
- `warning2_status_value`

Contains bitflags.  These flags indicate mixed status of the BMS.  Possible values:

```C++
		enum StatusInformation_Warning2
		{
			W2_PackLowVoltageProtection = (1 << 7), // protection 
			W2_PackLowVoltageWarning = (1 << 6), // warning 
			W2_PackOverVoltageProtection = (1 << 5), // protection 
			W2_PackOverVoltageWarning = (1 << 4), // warning 
			W2_CellLowVoltageProtection = (1 << 3), // protection 
			W2_CellLowVoltageWarning = (1 << 2), // warning 
			W2_CellOverVoltageProtection = (1 << 1), // protection 
			W2_CellOverVoltageWarning = (1 << 0), // warning 
		};
```

The entry:
- `warning3_status_value`

Contains bitflags.  These flags indicate mixed status of the BMS.  Possible values:

```C++
		enum StatusInformation_Warning3
		{
			W3_DischargingLowTemperatureProtection = (1 << 7), // protection 
			W3_DischargingLowTemperatureWarning = (1 << 6), // warning
			W3_DischargingHighTemperatureProtection = (1 << 5), // protection 
			W3_DischargingHighTemperatureWarning = (1 << 4), // warning
			W3_ChargingLowTemperatureProtection = (1 << 3), // protection 
			W3_ChargingLowTemperatureWarning = (1 << 2), // warning
			W3_ChargingHighTemperatureProtection = (1 << 1), // protection 
			W3_ChargingHighTemperatureWarning = (1 << 0), // warning
		};
```

The entry:
- `warning4_status_value`

Contains bitflags.  These flags indicate mixed status of the BMS.  Possible values:

```C++
		enum StatusInformation_Warning4
		{
			W4_ReservedWarning4Bit8 = (1 << 7), // warning
			W4_Heating = (1 << 6), // system
			W4_ComponentHighTemperatureProtection = (1 << 5), // protection
			W4_ComponentHighTemperatureWarning = (1 << 4), // warning
			W4_AmbientLowTemperatureProtection  = (1 << 3), // protection
			W4_AmbientLowTemperatureWarning = (1 << 2), // warning
			W4_AmbientHighTemperatureProtection = (1 << 1), // protection
			W4_AmbientHighTemperatureWarning = (1 << 0), // warning
		};
```

The entry:
- `warning5_status_value`

Contains bitflags.  These flags indicate mixed status of the BMS.  Possible values:

```C++
		enum StatusInformation_Warning5
		{
			W5_OutputShortCircuitLock = (1 << 7), // fault 
			W5_TransientOverCurrentLock = (1 << 6), // fault
			W5_OutputShortCircuitProtection = (1 << 5), // protection 
			W5_TansientOverCurrentProtection = (1 << 4), // protection 
			W5_DischargeOverCurrentProtection = (1 << 3), // protection 
			W5_DischargeOverCurrentWarning = (1 << 2), // warning
			W5_ChargeOverCurrentProtection = (1 << 1), // protection 
			W5_ChargeOverCurrentWarning = (1 << 0), // warning
		};
```

The entry:
- `warning6_status_value`

Contains bitflags.  These flags indicate mixed status of the BMS.  Possible values:

```C++
		enum StatusInformation_Warning6
		{
			W6_InternalWarning6Bit8 = (1 << 7), // warning
			W6_OutputConnectionFailure = (1 << 6), // fault
			W6_OutputReverseConnectionProtection = (1 << 5), // protection
			W6_CellLowVoltageChargingForbidden = (1 << 4), // fault
			W6_RemaingCapacityProtection = (1 << 3), // protection
			W6_RemaingCapacityWarning = (1 << 2), // warning
			W6_IntermittentPowerSupplementWaiting = (1 << 1), // warning
			W6_ChargingHighVoltageProtection = (1 << 0), // protection
		};
```

The entry:
- `warning7_status_value`

Contains bitflags.  These flags indicate warning status of the BMS.  Possible values:

```C++
		enum StatusInformation_Warning7
		{
			W7_Warning7InternalBit8 = (1 << 7),
			W7_Warning7InternalBit7 = (1 << 6),
			W7_ManualChargingWait = (1 << 5),
			W7_AutoChargingWait = (1 << 4),
			W7_Warning7InternalBit4 = (1 << 3),
			W7_Warning7InternalBit3 = (1 << 2),
			W7_Warning7InternalBit2 = (1 << 1),
			W7_Warning7InternalBit1 = (1 << 0),
		};
```

The entry:
- `warning8_status_value`

Contains bitflags.  These flags indicate fault status of the BMS.  Possible values:

```C++
		enum StatusInformation_Warning8
		{
			W8_Warning8InternalBit8 = (1 << 7),
			W8_Warning8InternalBit7 = (1 << 6),
			W8_Warning8InternalBit6 = (1 << 5),
			W8_NoNullPointCalibration = (1 << 4),
			W8_NoCurrentCalibration = (1 << 3),
			W8_NoVoltageCalibration = (1 << 2),
			W8_RTCFailure = (1 << 1),
			W8_EEPStorageFailure = (1 << 0),
		};
```
</details>
(click header to expand/collapse section)
<details>
<summary>

## Paceic Version 20 RAW Status Values: EG4 variant

</summary>

First, the full set of YAML config entries:
```yaml
sensor:
  - platform: pace_bms
    pace_bms_id: pace_bms_at_address_1

    # specific raw status values that you probably don't need, but the values / bit flags are documented anyway
    # you can probably just use the 6 text sensor equivalents which encompass all of these values and are suitable for display
    warning_status_value_cell_01:
      name: "Warning Status Value Cell 01"
    warning_status_value_cell_02:
      name: "Warning Status Value Cell 02"
    warning_status_value_cell_03:
      name: "Warning Status Value Cell 03"
    warning_status_value_cell_04:
      name: "Warning Status Value Cell 04"
    warning_status_value_cell_05:
      name: "Warning Status Value Cell 05"
    warning_status_value_cell_06:
      name: "Warning Status Value Cell 06"
    warning_status_value_cell_07:
      name: "Warning Status Value Cell 07"
    warning_status_value_cell_08:
      name: "Warning Status Value Cell 08"
    warning_status_value_cell_09:
      name: "Warning Status Value Cell 09"
    warning_status_value_cell_10:
      name: "Warning Status Value Cell 10"
    warning_status_value_cell_11:
      name: "Warning Status Value Cell 11"
    warning_status_value_cell_12:
      name: "Warning Status Value Cell 12"
    warning_status_value_cell_13:
      name: "Warning Status Value Cell 13"
    warning_status_value_cell_14:
      name: "Warning Status Value Cell 14"
    warning_status_value_cell_15:
      name: "Warning Status Value Cell 15"
    warning_status_value_cell_16:
      name: "Warning Status Value Cell 16"
    
    warning_status_value_temperature_01:
      name: "Warning Status Value Temperature 01"
    warning_status_value_temperature_02:
      name: "Warning Status Value Temperature 02"
    warning_status_value_temperature_03:
      name: "Warning Status Value Temperature 03"
    warning_status_value_temperature_04:
      name: "Warning Status Value Temperature 04"
    warning_status_value_temperature_05:
      name: "Warning Status Value Temperature 05"
    warning_status_value_temperature_06:
      name: "Warning Status Value Temperature 06"
    
    warning_status_value_charge_current:
      name: "Warning Status Value Charge Current"
    warning_status_value_discharge_current:
      name: "Warning Status Value Discharge Current"
    warning_status_value_total_voltage:
      name: "Warning Status Value Total Voltage"
    
    balance_event_value:
      name: "Balance Event Value"
    voltage_event_value:
      name: "Voltage Event Value"
    temperature_event_value:
      name: "Temperature Event Value"
    current_event_value:
      name: "Current Event Value"
    remaining_capacity_value:
      name: "Remaining Capacity Value"
    fet_status_value:
      name: "FET Status Value"

    system_status_value:
     name: "System Status Value"

```

The entries:
- `warning_status_value_cell_01` through `warning_status_value_cell_16`
- `warning_status_value_temperature_01` through `warning_status_value_temperature_06`
- `warning_status_value_charge_current` *
- `warning_status_value_discharge_current` *
- `warning_status_value_total_voltage`

All contain a scalar value.  They indicate a warning but not a fault or error (yet) on their respective measurement.  Possible values:

**Important:** * The EG4 variant does not differentiate between charge and discharge current warnings, these two fields will have an identical value.

```C++
	enum StatusInformation_WarningValues
	{
		WV_Normal = 0,
		WV_BelowLowerLimitValue = 1,
		WV_AboveUpperLimitValue = 2,
		WV_OtherFaultValue = 0xF0,
	};
```

The entry:
- `balance_event_value`

Contains bitflags.  These flags contain mixed status information on current status of the BMS.  Possible values:

```C++
		enum StatusInformation_BalanceEvent
		{
			BE_BalanceEventReservedBit8 = (1 << 7), // warning
			BE_DischargeMosFaultAlarm = (1 << 6), // fault
			BE_ChargeMosFaultAlarm = (1 << 5), // fault
			BE_CellVoltageDifferenceAlarm = (1 << 4), // warning
			BE_BalanceEventReservedBit4 = (1 << 3), // warning
			BE_BalanceEventReservedBit3 = (1 << 2), // warning
			BE_BalanceEventReservedBit2 = (1 << 1), // warning
			BE_BalanceEventBalancingActive = (1 << 0), // warning
		};
```

The entry:
- `voltage_event_value`

Contains bitflags.  These flags contain mixed status information on current status of the BMS.  Possible values:

```C++
		enum StatusInformation_VoltageEvent
		{
			VE_PackUnderVoltageProtect = (1 << 7), // protection
			VE_PackUnderVoltageAlarm = (1 << 6), // warning
			VE_PackOverVoltageProtect = (1 << 5), // protection
			VE_PackOverVoltageAlarm = (1 << 4), // warning
			VE_CellUnderVoltageProtect = (1 << 3), // protection
			VE_CellUnderVoltageAlarm = (1 << 2), // warning
			VE_CellOverVoltageProtect = (1 << 1), // protection
			VE_CellOverVoltageAlarm = (1 << 0), // warning
		};
```

The entry:
- `temperature_event_value`

Contains bitflags.  These flags contain mixed status information on current status of the BMS.  Possible values:

```C++
		enum StatusInformation_TemperatureEvent
		{
			TE_TemperatureEventReservedBit16 = (1 << 15), // warning
			TE_TemperatureEventReservedBit15 = (1 << 14), // warning
			TE_FireAlarm = (1 << 13), // fault
			TE_MosfetHighTemperatureProtect = (1 << 12), // protection
			TE_EnvironmentLowTemperatureProtect = (1 << 11), // protection
			TE_EnvironmentLowTemperatureAlarm = (1 << 10), // warning
			TE_EnvironmentHighTemperatureProtect = (1 << 9), // protection
			TE_EnvironmentHighTemperatureAlarm = (1 << 8), // warning
			TE_DischargeLowTemperatureProtect = (1 << 7), // protection
			TE_DischargeLowTemperatureAlarm = (1 << 6), // warning
			TE_DischargeHighTemperatureProtect = (1 << 5), // protection
			TE_DischargeHighTemperatureAlarm = (1 << 4), // warning
			TE_ChargeLowTemperatureProtect = (1 << 3), // protection
			TE_ChargeLowTemperatureAlarm = (1 << 2), // warning
			TE_ChargeHighTemperatureProtect = (1 << 1), // protection
			TE_ChargeHighTemperatureAlarm = (1 << 0), // warning
		};
```

The entry:
- ` current_event_value`

Contains bitflags.  These flags contain mixed status information on current status of the BMS.  Possible values:

```C++
		enum StatusInformation_CurrentEvent
		{
			CE_OutputShortCircuitLockout = (1 << 7), // fault
			CE_DischargeLevel2OverCurrentLockout = (1 << 6), // fault
			CE_OutputShortCircuitProtect = (1 << 5), // protection
			CE_DischargeLevel2OverCurrentProtect = (1 << 4), // protection
			CE_DischargeOverCurrentProtect = (1 << 3), // protection
			CE_DischargeOverCurrentAlarm = (1 << 2), // warning
			CE_ChargeOverCurrentProtect = (1 << 1), // protection
			CE_ChargeOverCurrentAlarm = (1 << 0), // warning
		};
```

The entry:
- `remaining_capacity_value`

Contains bitflags.  These flags contain mixed status information on current status of the BMS.  Possible values:

```C++
		enum StatusInformation_RemainingCapacity
		{
			RC_RemainingCapacityReservedBit8 = (1 << 7), // warning
			RC_RemainingCapacityReservedBit7 = (1 << 6), // warning
			RC_RemainingCapacityReservedBit6 = (1 << 5), // warning
			RC_RemainingCapacityReservedBit5 = (1 << 4), // warning
			RC_RemainingCapacityReservedBit4 = (1 << 3), // warning
			RC_RemainingCapacityReservedBit3 = (1 << 2), // warning
			RC_RemainingCapacityReservedBit2 = (1 << 1), // warning
			RC_StateOfChargeLow = (1 << 0), // warning
		};
```

The entry:
- `fet_status_value`

Contains bitflags.  These flags contain mixed status information on current status of the BMS.  Possible values:

```C++
		enum StatusInformation_FetStatus
		{
			FS_FetStatusReservedBit8 = (1 << 7), // configuration
			FS_FetStatusReservedBit7 = (1 << 6), // configuration
			FS_FetStatusReservedBit6 = (1 << 5), // configuration
			FS_FetStatusReservedBit5 = (1 << 4), // configuration
			FS_HeaterOn = (1 << 3), // configuration
			FS_ChargeCurrentLimiterOn = (1 << 2), // configuration
			FS_ChargeMosfetOn = (1 << 1), // configuration
			FS_DischargeMosfetOn = (1 << 0), // configuration
		};
```

The entry:
- `system_status_value`

Contains bitflags.  These flags contain mixed status information on current status of the BMS.  Possible values:

```C++
		enum StatusInformation_SystemStatus
		{
			SS_SystemStatusReservedBit8 = (1 << 7), // system
			SS_SystemStatusReservedBit7 = (1 << 6), // system
			SS_SystemStatusReservedBit6 = (1 << 5), // system
			SS_SystemStatusReservedBit5 = (1 << 4), // system
			SS_Standby = (1 << 3), // system
			SS_SystemStatusReservedBit3 = (1 << 2), // system
			SS_Charging = (1 << 1), // system
			SS_Discharging = (1 << 0), // system
		};
```
</details>
(click header to expand/collapse section)


# I'm having a problem using this component

Did you read this entire document?  If not, please do that first to make sure you understand how everything works.  You might be able to figure it out on your own!

If you still have an issue, or are seeing some "strange data" or log output, you can create an issue report. 

# Miscellaneous Notes
 
- My personal preference is for the [C# Style Guidelines](https://learn.microsoft.com/en-us/dotnet/standard/design-guidelines/) but the idea is to get this into ESPHome and [their guidelines](https://esphome.io/guides/contributing.html#codebase-standards) are different.  It's currently a bit of a mishmash until I can refactor over to ESPHome's style completely.

- Huge shout-out to https://github.com/syssi/esphome-seplos-bms who implemented an initial basic decode letting me know this was possible, and also compiled some documentation which was immensely useful.  Without which I might never have gotten started on, or been motivated to finish, this more complete implementation of the protocol.

# Helping Out

- I would like to make additions to the [known supported battery packs](#What-Battery-Packs-are-Supported) section.  If you have a pack that works, please share!

- If you can locate any new [documentation](https://github.com/nkinnan/esphome-pace-bms/tree/main/protocol_documentation) on the protocol, particularly for version 20 variants, or if you find a variation on version 25 (I'm not aware of any at this time), please let me know!

- Want to contribute more directly? Found a bug? Submit a PR! Could be helpful to discuss it with me first if it's non-trivial design change, or adding a new variant. 

- And of course, if you appreciate the work that went into this, you can always [buy me a coffee](https://www.buymeacoffee.com/nkinnan) :)
