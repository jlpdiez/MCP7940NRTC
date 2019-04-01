/*
 * MCP7940NRTC.h - library for MCP7940N RTC
 * This library is intended to be uses with Arduino Time library functions
 * Based on the library for the DS1307 https://github.com/PaulStoffregen/DS1307RTC
 */

#ifndef MCP7940NRTC_h
#define MCP7940NRTC_h

#include <TimeLib.h>

// library interface description
class MCP7940NRTC
{
  // user-accessible "public" interface
  public:
    MCP7940NRTC();
    static time_t get();
    static bool set(time_t t);
    static bool read(tmElements_t &tm);
    static bool write(tmElements_t &tm);
    static bool chipPresent() { return exists; }
    static unsigned char isRunning();
    static void setCalibration(char calValue);
    static char getCalibration();

  private:
    static bool exists;
    static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num);
};

#ifdef RTC
#undef RTC // workaround for Arduino Due, which defines "RTC"...
#endif

extern MCP7940NRTC RTC;

#endif
 

