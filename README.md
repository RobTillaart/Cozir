
[![Arduino CI](https://github.com/RobTillaart/Cozir/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/Cozir/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/Cozir/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/Cozir/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/Cozir/actions/workflows/jsoncheck.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/Cozir/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/Cozir.svg?maxAge=3600)](https://github.com/RobTillaart/Cozir/releases)


# Cozir

Arduino library for COZIR range CO2 sensors.


## Description

The Cozir library is **experimental** as not all functionality is tested.
The polling mode as used in the examples is tested in the past by DirtGambit.
**CO2meter.com** sponsored a **COZIR GC0034** to start testing (2022-02).
This sensor does not support all commands, but as the pattern of the commands 
is similar The non tested are expected to work as well.

This version of the library supports only the **Serial** interface. 
Preferred is a hardware Serial port to connect the sensor but software Serial 
does work too (on UNO, but not on MEGA.


#### Notes

- the hardware serial based examples like **CozirDemoHWSerial.ino** 
needs to be run on a MEGA or a Teensy, at least a board with more than one Serial port. 
- Read the datasheet before using this library. 
It helps to understand the working of the COZIR sensor.
- If the device is in **CZR_COMMAND** mode it does not respond too polling calls.
It needs to be set in **CZR_POLLING**
- Not all COZIR devices support all calls of this library.


## Interface

Read the datasheet (again).


### Constructor and initialisation

- **COZIR(Stream \* str)** constructor, gets a serial stream as reference.
- **void init()** sets operatingMode to **CZR_POLLING**
- **bool isInitialized()** returns true if enough time has passed after the call to **init()** for the sensor.
The sensor needs a few seconds to get correct values.


### Operating mode

- **void setOperatingMode(uint8_t mode)** set the operating mode either to **CZR_COMMAND**, **CZR_POLLING** or **CZR_STREAMING**
- **uint8_t getOperatingMode()** returns the mode set, **CZR_STREAMING** is the factory default. 
Please note that **init()** sets the operating mode to **CZR_POLLING**.


### Core functions

The COZIR CO2 sensors all support:
- **uint32_t CO2()** returns the CO2 concentration in PPM (!! might need PPMfactor).
- **uint16_t getPPMFactor()** returns 1, 10, 100. 
Normally the value returned is 1 but one should check at the first read and when there is a big jump in values returned.
Also when time interval between reads is large it might be useful to check the PPM factor.

Some COZIR sensors also support:

- **float celsius()** returns temperature of the sensor.
- **float fahrenheit()** idem, 'wrapper' around **celsius()**
- **float humidity()** idem, 'wrapper' around **celsius()**
- **float light()** idem.


### Calibration

Read datasheet before using these functions:

- **uint16_t fineTuneZeroPoint(uint16_t v1, uint16_t v2)**
- **uint16_t calibrateFreshAir()** typically 400 PPM.
- **uint16_t calibrateNitrogen()**
- **uint16_t calibrateKnownGas(uint16_t value)** 


#### Calibration NOT Recommended 

Following 3 functions are **NOT RECOMMENDED** by the datasheet.
Feel free to uncomment in the code and use at your own risk.
Read datasheet before using these functions:

- **uint16_t calibrateManual(uint16_t value)**
- **uint16_t setSpanCalibrate(uint16_t value)**
- **uint16_t getSpanCalibrate()**


### DigiFilter

use with care, read datasheet before use.

| value | meaning                                            |
|:-----:|:---------------------------------------------------|
|   0   | Special, see datasheet                             |
|   1   | fast, shows every single sample, raw, can be noisy |
|  32   | default, good average                              |
| 255   | very slow, max smoothed                            |

- **void setDigiFilter(uint8_t value)** The larger the value the more smoothed the signal is.
Larger values also means that the output does not follow fast changes.
So depending on your needs you need to find an optimal value for the project.
It might even so that you alternate between smooth and fast or adapt depending on
the actual CO2 value.
- **uint8_t getDigiFilter()** returns set value. 


### Streaming MODE

Warning: hardware serial is needed to improve the capture of all output correctly. 

- **void setOutputFields(uint16_t fields)** Sets the fields in the output stream as a 16 bit mask. See table below.
- **void clearOutputFields()** clears all the fields.
- **uint16_t getOutputFields()** returns the 16 bit mask of set output fields.
- **bool inOutputFields(uint16_t field)** returns true if the field is set.
- **void getRecentFields()** After a call to getRecentFields() you must read and parse the serial stream yourself.
The internal buffer of this Class cannot handle the possible large output. Lines can be over 100 bytes long!

The fields must be set as a bit mask, the order of the fields in the output is undetermined. 
So one need to parse the output of the sensor carefully.

Note: NOT all sensors support all fields, check the datasheet of the sensor used.

| Field             | Value HEX | Value DEC | Notes           |
|:------------------|:---------:|:---------:|:----------------|
| CZR_LIGHT         |  0X2000   |    8192   |                 |
| CZR_HUMIDITY      |  0X1000   |    4096   |                 |
| CZR_FILTLED       |  0X0800   |    2048   |                 |
| CZR_RAWLED        |  0X0400   |    1024   |                 |
| CZR_MAXLED        |  0X0200   |     512   |                 |
| CZR_ZEROPOINT     |  0X0100   |     256   |                 |
| CZR_RAWTEMP       |  0X0080   |     128   |                 |
| CZR_FILTTEMP      |  0X0040   |      64   |                 |
| CZR_FILTLEDSIGNAL |  0X0020   |      32   |                 |
| CZR_RAWLEDSIGNAL  |  0X0010   |      16   |                 |
| CZR_SENSTEMP      |  0X0008   |       8   |                 |
| CZR_FILTCO2       |  0X0004   |       4   |                 |
| CZR_RAWCO2        |  0X0002   |       2   |                 |
| CZR_NONE          |  0X0001   |       1   | reset           |
|                   |           |           |                 |
| CZR_DEFAULT       |  0X0006   |       6   | factory default |
| CZR_HTC           |  0X1082   |    4226   | shortcut        |
| CZR_ALL           |  0X3FFE   |   16383   | debug           |

Default value is 6  = CZR_FILTCO2 + CZR_RAWCO2


### EEPROM

Note: not all COZIR devices support EEPROM.
Check datasheet for the capabilities of your device.

In 0.3.0 the EEPROM function have been replaced by specific accessor functions. 
Read datasheet about the addresses and their meaning. Use with care.

- **void setAutoCalibrationPreload(uint16_t value)**
- **uint16_t getAutoCalibrationPreload()**
- **void setAutoCalibrationInterval(uint16_t value)**
- **uint16_t getAutoCalibrationInterval()**
- **void setAutoCalibrationOn()**
- **void setAutoCalibrationOff()**
- **bool getAutoCalibration()**
- **void setAutoCalibrationBackgroundConcentration(uint16_t value)**
- **uint16_t getAutoCalibrationBackgroundConcentration()**
- **void setAmbientConcentration(uint16_t value)**
- **uint16_t getAmbientConcentration()**
- **void setBufferClearTime(uint16_t value)**
- **uint16_t getBufferClearTime()**


#### EEPROM addresses used by above functions.

Read datasheet for the details and defaults of sensor at hand.

| Name     | Address | Default | Notes    |
|:---------|:-------:|:-------:|:---------|
| AHHI     | 0x00    | ?       | reserved |
| ANLO     | 0x01    | ?       | reserved |
| ANSOURCE | 0x02    | ?       | reserved |
| ACINITHI | 0x03    | 87      |          |
| ACINITLO | 0x04    | 192     |          |
| ACHI     | 0x05    | 94      |          |
| ACLO     | 0x06    | 128     |          |
| ACONOFF  | 0x07    | 0       |          |
| ACPPMHI  | 0x08    | 1       |          |
| ACPPMLO  | 0x09    | 194     |          |
| AMBHI    | 0x0A    | 1       |          |
| AMBLO    | 0x0B    | 194     |          |
| BCHI     | 0x0C    | 0       |          |
| BCLO     | 0x0D    | 8       |          |


### Miscellaneous

- **void getVersionSerial()** requests version over serial. 
The user should read (and parse) the serial output as it can become large. 
Also the user must reset the operating mode either to **CZR_POLLING** or **CZR_STREAMING**
- **void getConfiguration()** requests configuration over serial. 
The user should read (and parse) the serial output as it can become large. 
Also the user must reset the operating mode either to **CZR_POLLING** or **CZR_STREAMING**


## Operation

See examples.


## Future

- improve documentation
  - separate releaseNotes.md
  - matrix functions vs sensor ?
- test
  - test streaming mode
  - test table / matrix ?
- add examples
  - example two / array of COZIR sensors. MEGA with 4.
  - example COZIR with I2C display?
  - example streaming mode parsing.
- COZIR I2C class for newer generation 
  ~ same functional interface
  - multiWire / pin a la PCF8574 lib


#### won't for now

- add a **setEEPROMFactoryDefault()**?
  - unknown if all sensors have same values
- build a Arduino COZIR simulator for testing.
  - add other sensors underneath?
  
  
  