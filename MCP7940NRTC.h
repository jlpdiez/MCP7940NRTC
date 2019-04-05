/*
 * MCP7940NRTC.h - library for MCP7940N RTC
 * This library is intended to be uses with Arduino Time library functions
 * Based on the library for the DS1307 https://github.com/PaulStoffregen/DS1307RTC
 */

#ifndef MCP7940NRTC_h
#define MCP7940NRTC_h

#include <TimeLib.h>

class MCP7940NRTC {

  public:
    MCP7940NRTC(uint8_t sdaPin, uint8_t sclPin);
    time_t get();
    bool set(time_t t);
    bool read(tmElements_t &tm);
    bool write(tmElements_t &tm);
    bool chipPresent() { return _exists; }
    unsigned char isRunning();
    void setConfig(uint8_t confValue);
    uint8_t getConfig();
    void setCalibration(char calValue);
    char getCalibration();

  private:
    uint8_t _sdaPin, _sclPin;
    bool _exists;
    uint8_t getRegister(const uint8_t regAddr);
    void setRegister(const uint8_t regAddr, const uint8_t regData);
    static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num);
};

#endif
 

