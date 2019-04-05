#if defined (__AVR_ATtiny84__) || defined(__AVR_ATtiny85__) || (__AVR_ATtiny2313__)
    #include <TinyWireM.h>
    #define Wire TinyWireM
#else
    #include <Wire.h>
#endif

#include "MCP7940NRTC.h"
//TODO: REMOVE
//#include <Arduino.h>

#define MCP7940N_CTRL_ID 0x6F 
#define MCP7940N_CONF_REG 0x07

MCP7940NRTC::MCP7940NRTC(uint8_t sdaPin, uint8_t sclPin)
: _sdaPin(sdaPin), _sclPin(sclPin), _exists(false) {
    Wire.begin(_sdaPin, _sclPin);
}
  
// PUBLIC FUNCTIONS
// Aquire data from buffer and convert to time_t
time_t MCP7940NRTC::get() {
    tmElements_t tm;
    if (!read(tm)) return 0;
    return(makeTime(tm));
}

bool MCP7940NRTC::set(time_t t) {
    tmElements_t tm;
    breakTime(t, tm);
    return write(tm); 
}

// Aquire data from the RTC chip in BCD format
bool MCP7940NRTC::read(tmElements_t &tm) {
    uint8_t sec;
    Wire.beginTransmission(MCP7940N_CTRL_ID);
    #if ARDUINO >= 100  
        Wire.write((uint8_t)0x00); 
    #else
        Wire.send(0x00);
    #endif  
      if (Wire.endTransmission() != 0) {
          _exists = false;
          return false;
      }
      _exists = true;

    // request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
    Wire.requestFrom(MCP7940N_CTRL_ID, tmNbrFields);
    if (Wire.available() < tmNbrFields) return false;
    #if ARDUINO >= 100
        sec = Wire.read();
        tm.Second = bcd2dec(sec & 0x7f);    
        tm.Minute = bcd2dec(Wire.read() );
        tm.Hour =   bcd2dec(Wire.read() & 0x3f);  // mask assumes 24hr clock
        tm.Wday = bcd2dec(Wire.read() );
        tm.Day = bcd2dec(Wire.read() );
        tm.Month = bcd2dec(Wire.read() );
        tm.Year = y2kYearToTm((bcd2dec(Wire.read())));
    #else
        sec = Wire.receive();
        tm.Second = bcd2dec(sec & 0x7f);   
        tm.Minute = bcd2dec(Wire.receive() );
        tm.Hour =   bcd2dec(Wire.receive() & 0x3f);  // mask assumes 24hr clock
        tm.Wday = bcd2dec(Wire.receive() );
        tm.Day = bcd2dec(Wire.receive() );
        tm.Month = bcd2dec(Wire.receive() );
        tm.Year = y2kYearToTm((bcd2dec(Wire.receive())));
    #endif
    if (sec & 0x00) return false; // clock is halted
    return true;
}

bool MCP7940NRTC::write(tmElements_t &tm) {
    // To eliminate any potential race conditions,
    // stop the clock before writing the values,
    // then restart it after.
    Wire.beginTransmission(MCP7940N_CTRL_ID);
    #if ARDUINO >= 100  
        Wire.write((uint8_t)0x00); // reset register pointer  
        Wire.write((uint8_t)0x00); // Stop the clock. The seconds will be written last
        Wire.write(dec2bcd(tm.Minute));
        Wire.write(dec2bcd(tm.Hour));      // sets 24 hour format
        Wire.write(dec2bcd(tm.Wday));   
        Wire.write(dec2bcd(tm.Day));
        Wire.write(dec2bcd(tm.Month));
        Wire.write(dec2bcd(tmYearToY2k(tm.Year))); 
    #else  
        Wire.send(0x00); // reset register pointer  
        Wire.send(0x00); // Stop the clock. The seconds will be written last
        Wire.send(dec2bcd(tm.Minute));
        Wire.send(dec2bcd(tm.Hour));      // sets 24 hour format
        Wire.send(dec2bcd(tm.Wday));   
        Wire.send(dec2bcd(tm.Day));
        Wire.send(dec2bcd(tm.Month));
        Wire.send(dec2bcd(tmYearToY2k(tm.Year)));   
    #endif
    if (Wire.endTransmission() != 0) {
        _exists = false;
        return false;
    }
    _exists = true;

    // Now go back and set the seconds, starting the clock back up as a side effect
    Wire.beginTransmission(MCP7940N_CTRL_ID);
    #if ARDUINO >= 100  
        Wire.write((uint8_t)0x00); // reset register pointer  
        Wire.write(dec2bcd(tm.Second) | 0x80); // write the seconds and start oscillator
    #else  
        Wire.send(0x00); // reset register pointer  
        Wire.send(dec2bcd(tm.Second) | 0x80); // write the seconds and start oscillator
    #endif
    if (Wire.endTransmission() != 0) {
        _exists = false;
        return false;
    }
    _exists = true;
    return true;
}

//Makes use of OSCRUN bit which is bit 6 of 0x03 register.
//OSCRUN 1 = Oscillator is enabled and running
//OSCRUN 0 = Oscillator has stopped or has been disabled
bool MCP7940NRTC::isRunning() {
    // const uint8_t cuarenta = 0x40;
    uint8_t dataR = getRegister(0x03);
    // Serial.print("recv: ");
    // Serial.println(dataR,BIN);
    // // if ((dataR & cuarenta) == cuarenta)
    // //     Serial.println("TRUE");
    // Serial.println(uint8_t(dataR) & (1<<6));
    // Serial.println(dataR & (1<<6) == (1<<6));
    // // return (dataR & cuarenta);
    // if ((dataR >> 6) & 1)
    //     Serial.print("tru");
    // else
    //     Serial.print("fals");
    //return ((dataR & (1<<6)) == (1<<6));
    return (dataR >> 6) & 1;
}
    /*Wire.beginTransmission(MCP7940N_CTRL_ID);
    #if ARDUINO >= 100  
      Wire.write((uint8_t)0x03); 
    #else
      Wire.send(0x03);
    #endif  
    Wire.endTransmission();

    // Just fetch the seconds register and check the top bit
    Wire.requestFrom(MCP7940N_CTRL_ID, 1);
    // uint16_t readB = Wire.read();
    // Serial.print("isRunning: ");
    // Serial.println(readB, HEX);
    // Serial.print("& 0x80: ");
    // Serial.println(readB & 0x80);
    // Serial.print("& 0x80 == 0x80: ");
    // Serial.println((readB & 0x80) == 0x80);
    #if ARDUINO >= 100
      return (Wire.read() & 0x80);
    #else
      return (Wire.receive() & 0x80);
    #endif  */

void MCP7940NRTC::setConfig(const uint8_t confValue) {
    setRegister(MCP7940N_CONF_REG, confValue);
}

uint8_t MCP7940NRTC::getConfig() const {
    return getRegister(MCP7940N_CONF_REG);
}

// void MCP7940NRTC::setCalibration(char calValue) {
//   unsigned char calReg = abs(calValue) & 0x1f;
//   if (calValue >= 0) calReg |= 0x20; // S bit is positive to speed up the clock
//   Wire.beginTransmission(MCP7940N_CTRL_ID);
// #if ARDUINO >= 100  
//   Wire.write((uint8_t)0x07); // Point to calibration register
//   Wire.write(calReg);
// #else  
//   Wire.send(0x07); // Point to calibration register
//   Wire.send(calReg);
// #endif
//   Wire.endTransmission();  
// }

// char MCP7940NRTC::getCalibration() {
//   Wire.beginTransmission(MCP7940N_CTRL_ID);
// #if ARDUINO >= 100  
//   Wire.write((uint8_t)0x07); 
// #else
//   Wire.send(0x07);
// #endif  
//   Wire.endTransmission();

//   Wire.requestFrom(MCP7940N_CTRL_ID, 1);
// #if ARDUINO >= 100
//   unsigned char calReg = Wire.read();
// #else
//   unsigned char calReg = Wire.receive();
// #endif
//   char out = calReg & 0x1f;
//   if (!(calReg & 0x20)) out = -out; // S bit clear means a negative value
//   return out;
// }

// PRIVATE FUNCTIONS

uint8_t MCP7940NRTC::getRegister(const uint8_t regAddr) const {
    Wire.beginTransmission(MCP7940N_CTRL_ID);
    #if ARDUINO >= 100  
        Wire.write((uint8_t)regAddr); 
    #else
        Wire.send(regAddr);
    #endif  
    Wire.endTransmission();

    Wire.requestFrom(MCP7940N_CTRL_ID, 1);
    #if ARDUINO >= 100
        return Wire.read();
    #else
        return Wire.receive();
    #endif
}

void MCP7940NRTC::setRegister(const uint8_t regAddr, const uint8_t regData) {
    Wire.beginTransmission(MCP7940N_CTRL_ID);
    #if ARDUINO >= 100  
        Wire.write((uint8_t)regAddr); // Point to calibration register
        Wire.write(regData);
    #else  
        Wire.send(regAddr); // Point to calibration register
        Wire.send(regData);
    #endif
    Wire.endTransmission();  
}

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t MCP7940NRTC::dec2bcd(uint8_t num) {
    return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t MCP7940NRTC::bcd2dec(uint8_t num) {
    return ((num/16 * 10) + (num % 16));
}

