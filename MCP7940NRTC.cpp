#include <Wire.h>
#include "MCP7940NRTC.h"
//TODO: REMOVE
#include <Arduino.h>

#define MCP7940N_CTRL_ID 0x6F 
#define MCP7940N_CONTROL_REG 0x07

// *********************************************
// PUBLIC METHODS
// *********************************************
MCP7940NRTC::MCP7940NRTC(uint8_t sdaPin, uint8_t sclPin)
: _sdaPin(sdaPin), _sclPin(sclPin), _exists(false) {
    Wire.begin(_sdaPin, _sclPin);
}
  
//Acquire data from hardware and return time_t
//Returns 0 if theres a comms issue or oscillator is halted
time_t MCP7940NRTC::get() {
    tmElements_t tm;
    if (!read(tm)) return 0;
    return(makeTime(tm));
}

//Sets the HW clock to the time given in time_t format
//Returns false if there was a communication error
bool MCP7940NRTC::set(time_t t) {
    tmElements_t tm;
    breakTime(t, tm);
    return write(tm); 
}

//Returns the HW time in tmElements_t format
//False if theres a comms issue or oscillator is halted
bool MCP7940NRTC::read(tmElements_t &tm) {
    uint8_t sec;
    Wire.beginTransmission(MCP7940N_CTRL_ID);
    Wire.write((uint8_t)0x00); 
    if (Wire.endTransmission() != 0) {
      _exists = false;
      return false;
    }
    _exists = true;

    // request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
    Wire.requestFrom(MCP7940N_CTRL_ID, tmNbrFields);
    if (Wire.available() < tmNbrFields) return false;

    sec = Wire.read();
    tm.Second = bcd2dec(sec & 0x7f);    
    tm.Minute = bcd2dec(Wire.read() );
    tm.Hour =   bcd2dec(Wire.read() & 0x3f);  // mask assumes 24hr clock
    tm.Wday = bcd2dec(Wire.read() );
    tm.Day = bcd2dec(Wire.read() );
    tm.Month = bcd2dec(Wire.read() );
    tm.Year = y2kYearToTm((bcd2dec(Wire.read())));

    if (sec & 0x00) return false; // clock is halted
    return true;
}

//Writes the given data to the HW registers
//Returns false if there was a comms error
bool MCP7940NRTC::write(tmElements_t &tm) {
    // To eliminate any potential race conditions,
    // stop the clock before writing the values,
    // then restart it after.
    Wire.beginTransmission(MCP7940N_CTRL_ID);
    
    Wire.write((uint8_t)0x00); // reset register pointer  
    Wire.write((uint8_t)0x00); // Stop the clock. The seconds will be written last
    Wire.write(dec2bcd(tm.Minute));
    Wire.write(dec2bcd(tm.Hour));      // sets 24 hour format
    Wire.write(dec2bcd(tm.Wday));   
    Wire.write(dec2bcd(tm.Day));
    Wire.write(dec2bcd(tm.Month));
    Wire.write(dec2bcd(tmYearToY2k(tm.Year))); 

    if (Wire.endTransmission() != 0) {
        _exists = false;
        return false;
    }
    _exists = true;

    // Now go back and set the seconds, starting the clock back up as a side effect
    Wire.beginTransmission(MCP7940N_CTRL_ID);
    Wire.write((uint8_t)0x00); // reset register pointer  
    Wire.write(dec2bcd(tm.Second) | 0x80); // write the seconds and start oscillator

    if (Wire.endTransmission() != 0) {
        _exists = false;
        return false;
    }
    _exists = true;
    return true;
}

//Makes use of OSCRUN bit which is bit 5 of 0x03 register.
//OSCRUN 1 = Oscillator is enabled and running
//OSCRUN 0 = Oscillator has stopped or has been disabled
bool MCP7940NRTC::isRunning() {
    return getRegisterBit(0x03, 5);
}

void MCP7940NRTC::setConfig(const uint8_t confValue) {
    setRegister(MCP7940N_CONTROL_REG, confValue);
}

uint8_t MCP7940NRTC::getConfig() const {
    return getRegister(MCP7940N_CONTROL_REG);
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

//Makes use of VBATEN bit which is bit 3 of 0x03 register.
//VBATEN 1 = Vbat input is enabled
//VBATEN 0 = Vbat input is disabled
bool MCP7940NRTC::getBatteryStatus() const {
    return getRegisterBit(0x03, 3);
}

void MCP7940NRTC::enableBattery() {
    setRegisterBit(0x03, 3, 1);
}

void MCP7940NRTC::disableBattery() {
    setRegisterBit(0x03, 3, 0);
}


// *********************************************
// PRIVATE METHODS
// *********************************************
uint8_t MCP7940NRTC::getRegister(const uint8_t regAddr) const {
    Wire.beginTransmission(MCP7940N_CTRL_ID);
    Wire.write((uint8_t)regAddr); 
    Wire.endTransmission();
    Wire.requestFrom(MCP7940N_CTRL_ID, 1);
    return Wire.read();
}

void MCP7940NRTC::setRegister(const uint8_t regAddr, const uint8_t regData) {
    //Select device
    Wire.beginTransmission(MCP7940N_CTRL_ID);
    //Select register
    Wire.write((uint8_t)regAddr);
    //Give data to write
    Wire.write(regData);
    Wire.endTransmission();  
}

//Check that bit bitNum of registerAddress is 1
bool MCP7940NRTC::getRegisterBit(const uint8_t regAddr, const uint8_t bitNum) const {
    return (getRegister(regAddr) >> bitNum) & 1;
}

void MCP7940NRTC::setRegisterBit(const uint8_t regAddr, const uint8_t bitNum, const bool bitValue) {
    uint8_t regValue = getRegister(regAddr);
    Serial.print("get: ");
    Serial.println(regValue, BIN);
    //Set to one
    if (bitValue == 1) {
        regValue |= 1 << bitNum;
    //Set to zero
    } else {
        regValue &= ~(1 << bitNum);
    }
    Serial.print("set: ");
    Serial.println(regValue, BIN);
    setRegister(regAddr, regValue);
}


// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t MCP7940NRTC::dec2bcd(uint8_t num) {
    return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t MCP7940NRTC::bcd2dec(uint8_t num) {
    return ((num/16 * 10) + (num % 16));
}

