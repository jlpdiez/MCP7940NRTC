# MCP7940N RTC Library for ESP8266
[![Build Status](https://travis-ci.com/Melkoroth/MCP7940NRTC.svg?branch=master)](https://travis-ci.com/Melkoroth/MCP7940NRTC)
[![C++ Arduino](https://img.shields.io/badge/c%2B%2B-Arduino%20-red.svg)](https://github.com/adafruit/Adafruit_CircuitPlayground)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)

This library is intended to make use of the MCP7940N RTC under the Arduino environment for the ESP8266, ESP01, ESP12E & all other derivatives.

It's main purpose is to be used in conjuction with the [Time library](https://github.com/PaulStoffregen/Time).

## Usage:
First of all object should be created by passing the SDA & SCL pin numbers to its contructor:
```cpp
const uint8_t SCLPIN = 12;
const uint8_t SDAPIN = 14;

MCP7940NRTC RTC = MCP7940NRTC(SDAPIN, SCLPIN);
```

Once the object has been created you can now invoke it's methods normally.

Please see attached examples folder to get an idea of it's basic usage.