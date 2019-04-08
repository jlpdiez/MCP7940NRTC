# MCP7940N RTC Library for Arduino & ESP8266
[![Build Status](https://travis-ci.com/Melkoroth/MCP7940NRTC.svg?branch=master)](https://travis-ci.com/Melkoroth/MCP7940NRTC)
[![C++ Arduino](https://img.shields.io/badge/c%2B%2B-Arduino%20-red.svg)](https://github.com/adafruit/Adafruit_CircuitPlayground)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)

This library is intended to make use of the MCP7940N RTC in the Arduino environment. 
It's main purpose is to be used in conjuction with the [Time library](https://github.com/PaulStoffregen/Time). See the TimeRTC example sketches provided with the Time library download for usage

## Usage:
First of all object should be created by passing the SDA & SCL pin numbers to its contructor:
```cpp
const uint8_t SCLPIN = A4;
const uint8_t SDAPIN = A5;

MCP7940NRTC RTC = MCP7940NRTC(SDAPIN, SCLPIN);
```

As you can see [here](https://www.arduino.cc/en/Reference/Wire) the most common I2C pins are the following:

|  BoardI2C     | TWI pins                       |
|---------------|--------------------------------|
| Uno, Ethernet | A4 (SDA), A5 (SCL)             |
| Mega2560      | 20 (SDA), 21 (SCL)             |
| Leonardo      | 2 (SDA), 3 (SCL)               |
| Due           | 20 (SDA), 21 (SCL), SDA1, SCL1 |