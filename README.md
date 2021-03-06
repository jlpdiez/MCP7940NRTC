# MCP7940N RTC Library for ESP8266
[![Build Status](https://travis-ci.com/Melkoroth/MCP7940NRTC.svg?branch=master)](https://travis-ci.com/Melkoroth/MCP7940NRTC)
[![C++ Arduino](https://img.shields.io/badge/c%2B%2B-Arduino%20-red.svg)](https://github.com/adafruit/Adafruit_CircuitPlayground)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)

This library is intended to make use of the [MCP7940N RTC](https://www.microchip.com/wwwproducts/en/MCP7940N) under the Arduino environment for the ESP8266, ESP01, ESP12E & all other derivative hardware.

It's been based on the [DS1307RTC library](https://github.com/PaulStoffregen/DS1307RTC) and its main purpose is to be used in conjuction with the [Time library](https://github.com/PaulStoffregen/Time).

## Usage:
First of all object should be created by passing the SDA & SCL pin numbers to its contructor:
```cpp
const uint8_t SCLPIN = 12;
const uint8_t SDAPIN = 14;

MCP7940NRTC RTC = MCP7940NRTC(SDAPIN, SCLPIN);
```

Once the object has been created you can now invoke its methods normally. They are as follows:

```cpp
//Acquire data from hardware and return time_t
//Returns 0 if there is a comms issue or oscillator is halted
time_t get();
//Sets the HW clock to the time given in time_t format
//Returns false if there was a communication error
bool set(time_t t);
//Returns the HW time in tmElements_t format
//False if theres a comms issue or oscillator is halted
bool read(tmElements_t &tm);
//Writes the given data to the HW registers
//Returns false if there was a comms error
bool write(tmElements_t &tm);
//This is set to true when no communication errors exist
bool chipPresent();
//Checks the oscillator running bit
bool isRunning();
//Checks the external battery status
bool getBatteryStatus() const;
//Sets the external battery
void enableBattery();
void disableBattery();
```

Please see attached examples folder to get an idea of it's basic usage.

## Not implemented:
* Calibration procedure
* Retrieving of power down/up status bit and timestamps
* Alarm functionality
* Internal SRAM
