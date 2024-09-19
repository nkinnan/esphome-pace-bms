
esphome-pace-bms
-
This is an **ESPHome** component that supports "**paceic**" protocol **version 20 and 25** which is used by seemingly the majority of low-cost rack-mount Lithium Iron (LiFePO4) battery packs (but occasionally a different chemistry as well) manufactured in SE Asia.  The BMS can be communicated with over **RS485** or **RS232** and is manufactured by PACE (or is a clone).  It's used by many, many different manufacturers under different labels and branding.

If you are a developer, the protocol implementation is fully portable with a clean interface in C++ with no dependencies on ESPHome or any other libraries (it does require C++17 support due to use of `std::optional`, though that could be removed easily).  Feel free to use it for whatever you wish, but a heads-up would be appreciated just so I know what's happening with it :)

Example PACE BMS board:
![Example PACE BMS board (front)](images/example-board-front.png)
![Example PACE BMS board (back)](images/example-board-back.png)

The protocol is characterized by both requests and responses beginning with a '**~**' (tilde) character followed by two ASCII numbers (usually) either "**20**" or "**25**" and ending with a '**\r**' (carriage return) character.

I strongly encourage you to read through this entire document, but here are some Quick Links:
- [What Battery Packs are supported?](fixme)
- [I just want an ESPHome config](fixme)
- [How do I wire my ESP to the RS485/RS232 port?](fixme)
- [I'm having a problem using this component](fixme)
- [I want to talk to a battery that isn't listed](fixme)

Paceic Protocol Version 20
-
  This protocol version is spoken by older battery packs and has several variants, with firmware customized by individual manufacturers.  Three protocol variants are currently known/supported:
 - **EG4**
 - **PYLON**
 - **SEPLOS**

Different manufacturers will have different BMS management software (it will not be PbmsTools) but typically it speaks a variant of paceic version 20.  These older BMSes will usually have two RS485 ports (looks like an ethernet socket) and may have an RS232 port (looks like a telephone socket).  They usually won't have a CAN bus port.

There is a high likelihood that one of these protocol variants will work for battery packs speaking protocol version 20 which are branded by a different manufacturer than the three listed, but if you can find a spec doc for a new variant that behaves differently, I can probably add support.  See [here](https://github.com/nkinnan/esphome-pace-bms/tree/main/protocol_documentation/paceic/0x20) for documentation on currently known version 20 protocol variants. 

Example protocol version 20 BMS front-panel:
![EG4 Protocol 20 Front Panel](images/EG4-0x20.webp)

Paceic Protocol Version 25
-
This version seems more standardized, with an official protocol specification from PACE itself.  As far as I know, all newer battery packs speaking this protocol version should be supported.  See [here](https://github.com/nkinnan/esphome-pace-bms/tree/main/protocol_documentation/paceic/0x25) for documentation on protocol version 25.

These BMSes speaking paceic version 25 will invariably use PbmsTools for their BMS management software (or a rebadged version of it) which looks like this:

![PbmsTools Screenshot](images/PbmsTools.jpg)

The exact look isn't important, just that the tabs and general layout looks like this.  This is PbmsTools regardless of any specific brand badging and indicates that your BMS supports protocol version 25.

These BMSes will typically have two RS485 ports (looks like an ethernet socket) an RS232 port (looks like a telephone socket) and possibly a CAN bus port and an LCD display as well, especially if newer.

Example protocol version 25 BMS front-panel:
![Jakiper Protocol 25 Front Panel](images/Jakiper-0x25.png)

Pace MODBUS Protocol
-
Some BMS firmwares also support reading data via MODBUS protocol over the RS485 port.  I haven't looked into this yet.  It seems like it may co-exist with Paceic version 25.  Documentation can be found [here](https://github.com/nkinnan/esphome-pace-bms/tree/main/protocol_documentation/modbus).  I may add support for this later, but since documentation is available, ESPHome supports MODBUS already, and syssi has already created an [ESPHome configuration for it](https://github.com/syssi/esphome-pace-bms), it's low priority.

Supported BMS Values/Status (read only)
-
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

Supported BMS Configuration (read / write)
-
- **System Date and Time** - Allows access to the BMS internal real-time clock 
- **Shutdown** - A button which sends the shutdown command to the BMS

Supported BMS Configuration (read / write) - **Version 25 ONLY**
-
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

What Battery Packs are Supported?
- 
As far as I know, many/most.  Any not listed should simply require a small tweak to the configuration.  

However, I'd like to keep a full list here if only for search engine discoverability, so if you find that it does work with your battery pack, please contact me with the configuration tweaks required, the make/model of battery pack (and a link to the exact model on the manufacturer's website if possible), and what it reports for the hardware version.

For help figuring out how to do those configuration tweaks to get your battery pack working, see [here](FIXME)

**Known working protocol version 20 battery packs:**
- **EG4 LIFEPOWER4**
  - hardware versions: 
	  - **QTHN 0d[3][6]**
	    - ![EG4 LIFEPOWER4](images/EG4-0x20-320.png)
  - required config: 
	  - `protocol_commandset: 0x20`
	  - `protocol_variant: "EG4"`
	  - `battery_chemistry: 0x4A`
  - notes:
	  - The BMS is a bit slow, so don't reduce the timeouts too much. I have found the following settings prevent lockup from querying it too quickly:
		  -   `request_throttle: 200ms`
		  -   `response_timeout: 2000ms`


**Known working protocol version 25 battery packs:**
- **Jakiper JK48V100**
  - hardware versions: 
	  - **FIXME**
	    - ![EG4 LIFEPOWER4](images/Jakiper-0x25-320.png)
  - required config: 
	  - `protocol_commandset: 0x25`

What ESPs are Supported?
- 
Both ESP8266 and ESP32 are supported, though an ESP32 class device is recommended.  

Any board which gives you access to a hardware UART (both RX and TX) is fine.  Software UART on GPIO pins is not recommended.  

You cannot connect the UART RX/TX pins directly to either the RS232 or RS485 port, a converter chip for RS485 or RS232 signal levels is required.  Some boards may have such a converter chip built-in, or you can use a breakout board.  

RS485 will require at least one additional GPIO pin for flow control in addition to the UART RX and TX pins.  RS232 will require only the UART RX and TX.

If using an 8266, you will need to redirect serial logs to the second UART (which is TX only, but that's fine for logging).  An example config for that is included below in the YAML section.

How do I wire my ESP to the RS485 port?
- 
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

How do I wire my ESP to the RS232 port?
- 
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

Example ESPHome configuration YAML
-
A full ESPHome configuration will consist of thee parts:

1. The basic stuff like board type, wifi credentials, api or mqtt configuration, web_server if used, and so on. 
2. Configuration of the UART and the pace_bms component to speak with your battery pack.
3. Exposing the values / status / configuration that you want accessible via the web_server dashboard, homeassistant, or mqtt.

I won't go over 1 since that will be specific to your setup, except to say that if you want to use `web_server` then you should probably add `version: 3` and click the dark mode icon whenever you open it up because it is a *significant* improvement over version 2, but not yet set as the default.

If using an 8266 you will need to add this to your esphome config section.  It **massively** speeds up how quickly the 8266 can speak with the web_server dashboard by correcting a bug in the web server code.  Once [this PR](https://github.com/esphome/ESPAsyncWebServer/pull/41) goes through these lines can be removed.

```
esphome:
  libraries:
    # huge improvement to event throughput to the on-device web_server dashboard
    - ESPAsyncWebServer-esphome=https://github.com/nkinnan/ESPAsyncWebServer#async_event_source_yield```
```

Before anything else, you will need to tell ESPHome where to find this component.  Add the following lines to your YAML:

```
external_components:
  - source:
      type: git
      url: https://github.com/nkinnan/esphome-pace-bms
      ref: "main"
    components: [ pace_bms ]
    refresh: 1s
    
  - source:
      type: git
      url: https://github.com/nkinnan/esphome
      ref: "make_time_dependency_optional"
    components: [ datetime ]
    refresh: 1s
```

The second source section is needed to work around a design bug in the ESPHome DateTime component, it will be removed / become unnecessary once [the PR](https://github.com/esphome/esphome/pull/7425) to fix that goes through.

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

  protocol_commandset: 0x20
  protocol_variant: "EG4"
  protocol_version: 0x20 
  battery_chemistry: 0x4A 
```
* **address:** This is the address of your BMS, set with the DIP switches on the front next to the RS232 and RS485 ports.  **Important:** If you change the value of the DIP switches, you'll need to reset the BMS for the new address to take effect.  Either by flipping the breaker, or using a push-pin to depress the recessed reset button.  The most common address values are 0 and 1, unless your battery packs are daisy chained, which is not currently supported by this component.
* **uart_id:** The ID of the UART you configured.  This component currently requires one UART per BMS, though I'm considering a design change that would allow it to read "daisy chained" BMSes in the future.
* **flow_control_pin:** If using RS232 this setting should be omitted.  If using RS485, this is required to be set, as it controls the direction of communication on the RS485 bus.  It should be connected to *both* the **DE** (Driver Output Enable) and **R̅E̅** (Receiver Output Enable, active low) pins on the RS485 adapter / breakout board.
* **update_interval:** How often to query the BMS and publish whatever updated values are read back.  What queries are sent to the BMS is determined by what values you have requested to be published in the rest of your configuration.
* **request_throttle:** Minimum interval between sending requests to the BMS.  Increasing this may help if your BMS "locks up" after a while, it's probably getting overwhelmed.
* **response_timeout:** Maximum time to wait for a response before "giving up" and sending the next.  Increasing this may help if your BMS "locks up" after a while, it's probably getting overwhelmed.
* **protocol_commandset, protocol_variant, protocol_version,** and **battery_chemistry:** 
   - Consider these as a set.  Use values from the known supported list, or determine them manually by following the steps in [I want to talk to a battery that isn't listed](fixme)



Next, lets go over making things available to the web_server dashboard, homeassistant, or mqtt.  This is going to differ slightly depending on what data you want to read back from the BMS, I will provide a complete example which you can pare down to only what you want to see.

Example 1: All read-only values, available on all protocol versions and variants
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

Example 2: Read-write values available on both protocol version 20 and 25
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
Example 3: Read-write values only available with protocol version 25
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
      name: "Sleep Delay"
 
    full_charge_voltage:
      name: "Full Charge Voltage"
    full_charge_amps:
      name: "Full Charge Amps"
    low_charge_alarm_percent:
      name: "Low Charge Alarm"
 
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


I want to talk to a battery that isn't listed
- 
Before proceeding through this section, please read the entire rest of this document first!  It assumes some familiarity and does not repeat steps like configuring the UART, but simply provides a guide on how to determine the specific protocol your BMS is speaking.

If your battery pack has a front panel that "looks like" a Pace BMS but is not in the "known supported" list, it probably is, and is probably supported.  Unless there are more version 20 variants out there than I've guessed, but even then you should be able to get some useful data back.  So you just need to figure out what settings will enable this component to speak with it.

The first step is to make sure it's communicating at all.  If you can't connect the battery manufacturer's BMS management software to it and get readings back, don't proceed any further until you can.  There's no point trying to debug a dead port or a broken BMS.  You can try both RS232 and RS485.  One or the other may not be "active".  The RS232 port if available is the most likely to be speaking paceic (they may be programmed to speak different protocols).  

Once your manufacturer's recommended software is talking to your battery pack successfully, if you're on Windows, you can use [this](https://www.com-port-monitoring.com/downloads.html) software to "snoop" on the COM port and see what the protocol looks like.  Linux or Mac should have equivalents available but I'm not familiar with them.  You should see something like this:

```~25xx46xxxxxxxx\r```
or
```~20xx46xxxxxxxx\r```

The x's will be hexidecimal values.  The \r may or may not be visible, it might just show up as a line return in whatever software you're using to snoop on the COM port.  If it looks nothing like that at all, sorry, you're out of luck.  If some of the requests look like that and other's don't, that's fine, continue on as long as at least some of them do.

We need at least one and as many as four configuration values to speak with the BMS successfully:
1) **`protocol_commandset`** - The **actual** protocol version being used, this determines what commands can be sent to the BMS.
2) **`protocol_version`** - The "claimed version" of the protocol - some BMSes lie about what protocol version they are speaking.  This is the value sent over the wire in the frame header, but which commands can be sent is still determined by `protocol_commandset`
3) **`protocol_variant`** - For protocol commandset 20 only, the "variant" of the protocol this BMS is using.  This determines how some of the BMS responses (to the same command) are interpreted, and can be one of (currently) three supported values:
    * PYLON
    * SEPLOS
    * EG4
4) **`battery_chemistry`** - In almost all cases this will be 0x46, but some manufacturers who again hate compatibility will use a different value (or actually legitimately have a different chemistry in some cases).

Now, going back to the requests you snooped over the COM port
```
~25xx46xxxxxxxx\r
```
The first number, the 20 or the 25 at the beginning (it may be a different number, more on that in a moment) is the protocol version your BMS is speaking.  The second number (after two x's) is your battery chemistry.  Put both of them into your config YAML (you can skip battery_chemistry if it was 46 as expected since that is the default value):
 

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

If your commandset value is 0x25 then you're basically done.  Just fill out your YAML with the rest of the settings / readouts you want exposed and you can skip the rest of this section.  Please contact me with your make/model/hardware version as well as the settings you used so that I can add it to the known supported list.

If the requests you were seeing didn't start with either 20 or 25, but otherwise "looked right", that means your BMS is using a custom firmware with a non-standard protocol version reported.  That's probably fine, it's probably still speaking version 20 or 25, but is lying about it because manufacturers dislike compatibility for some reason.  So you're going to have to try both, and configure pace_bms to lie right back.  Here we'll use 42 as an example of that first number you saw instead of a 20 or 25.

```yaml
pace_bms:
  protocol_commandset: 0x20
  protocol_version: 0x42 # the BMS is lying, so lie right back
  battery_chemistry: 0x4A # only if not 46
```
or
```yaml
pace_bms:
  protocol_commandset: 0x25
  protocol_version: 0x42 # the BMS is lying, so lie right back
  battery_chemistry: 0x4A # only if not 46
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

Once again, if your "true" commandset value is determined to be 0x25 then you're basically done.  Just fill out your YAML with the rest of the settings / readouts you want exposed and you can skip the rest of this section.  Please contact me with your make/model/hardware version as well as the settings you used so that I can add it to the known supported list.

However, if it is 0x20 then you also need to figure out which "variant" of protocol version 20 it is.  Start by putting this into your config:

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

If none of them work properly, I'd be interested to hear about it.  You have a BMS speaking a protocol variant I haven't come across or found documentation for.  Please file an issue and provide me with whatever data you can including make/model/hardware version (in particular the hardware version reported by pace_bms if you can get it to respond to that request, or the manufacturer's recommended BMS software if not).  Even better if you can provide me some COM port traces between the manufacturer's software and the BMS or even a protocol spec doc you found by googling your hardware.  I might be able to implement the new variant for you.

I'm having a problem using this component
- 
Did you read this entire document?  If not, please do that first to make sure you understand how everything works.  You might be able to figure it out on your own!

If you still have an issue, or are seeing some "strange data" or log output, you can create an issue report. 

Each Configuration Entry in Excruciating Detail
- 


Decoding the Status Values (but you probably don't want to)
- 

Miscellaneous Notes
- 
- My personal preference is for the [C# Style Guidelines](https://learn.microsoft.com/en-us/dotnet/standard/design-guidelines/) but the idea is to get this into ESPHome and [their guidelines](https://esphome.io/guides/contributing.html#codebase-standards) are different.  It's currently a bit of a mishmash until I can refactor over to ESPHome's style completely.

- Huge shout-out to https://github.com/syssi/esphome-seplos-bms who implemented an initial basic decode letting me know this was possible, and also compiled some documentation which was immensely useful.  Without which I might never have gotten started on, or been motivated to finish, this more complete implementation of the protocol.

Helping Out
- 
- I would like to make additions to the [known supported battery packs](fixme) section.  If you have a pack that works, please share!

- If you can locate any new [documentation](https://github.com/nkinnan/esphome-pace-bms/tree/main/protocol_documentation) on the protocol, particularly for version 20 variants, or if you find a variation on version 25 (I'm not aware of any at this time), please let me know!

- Want to contribute more directly? Found a bug? Submit a PR! Could be helpful to discuss it with me first if it's non-trivial design change, or adding a new variant. 

- And of course, if you appreciate the work that went into this, you can always [buy me a coffee](https://www.buymeacoffee.com/nkinnan) :)



