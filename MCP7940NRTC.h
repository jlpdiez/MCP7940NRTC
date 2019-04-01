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
    static time_t get();
    static bool set(time_t t);
    static bool read(tmElements_t &tm);
    static bool write(tmElements_t &tm);
    static bool chipPresent() { return _exists; }
    static unsigned char isRunning();
    static void setCalibration(char calValue);
    static char getCalibration();

  private:
    uint8_t _sdaPin, _sclPin;
    static bool _exists;
    static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num);
};

#endif
 

