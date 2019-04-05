# MCP7940N RTC Library for Arduino & ESP8266

This library is intended to make use of the MCP7940N RTC in the Arduino environment. 
It's main purpose is to be used in conjuction with the [Time library](https://github.com/PaulStoffregen/Time). See the TimeRTC example sketches provided with the Time library download for usage

## Usage:
First of all object should be created by passing the SDA & SCL pin numbers to its contructor:
```cpp
const uint8_t SCLPIN = 12;
const uint8_t SDAPIN = 14;

MCP7940NRTC RTC = MCP7940NRTC(SDAPIN, SCLPIN);
```

