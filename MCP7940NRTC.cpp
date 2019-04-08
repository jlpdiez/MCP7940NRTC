#include <Wire.h>
#include "MCP7940NRTC.h"

#define MCP7940N_CTRL_ID 0x6F 
#define MCP7940N_CONTROL_REG 0x07
#define MCP7940N_WKDAY_REG 0x03

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
    if (!endTransmission()) return false;

    //Request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
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

    //Technically we should read OSCRUN bit and not ST bit
    //But this is fastest as we avoid another I2C communication cycle
    //Check if bit 7 of RTCSEC register is zero
    if (!((sec >> 7) & 1)) return false;
    return true;
}

//Writes the given data to the HW registers
//Returns false if there was a comms error
bool MCP7940NRTC::write(tmElements_t &tm) {
    /* Per datasheet:
    To avoid rollover issues when loading new time and date values, 
    the oscillator/clock input should be disabled by clearing
    the ST bit for External Crystal mode and the 
    EXTOSC bit for External Clock Input mode. 
    After waiting for the OSCRUN bit to clear, the new values 
    can be loaded and the ST or EXTOSC bit can then be re-enabled.
    */
    //Stop internal oscillator and set time minus seconds
    Wire.beginTransmission(MCP7940N_CTRL_ID);
    Wire.write((uint8_t)0x00);
    Wire.write((uint8_t)0x00);
    Wire.write(dec2bcd(tm.Minute));
    Wire.write(dec2bcd(tm.Hour));      // sets 24 hour format
    Wire.write(dec2bcd(tm.Wday));   
    Wire.write(dec2bcd(tm.Day));
    Wire.write(dec2bcd(tm.Month));
    Wire.write(dec2bcd(tmYearToY2k(tm.Year))); 
    if (!endTransmission()) return false;
    // Now go back and set the seconds, starting the oscillator as a side effect
    Wire.beginTransmission(MCP7940N_CTRL_ID);
    Wire.write((uint8_t)0x00);
    //Write the seconds and start oscillator
    Wire.write(dec2bcd(tm.Second) | 0x80);
    return endTransmission();
}

//Makes use of OSCRUN bit which is bit 5 of 0x03 register.
//OSCRUN 1 = Oscillator is enabled and running
//OSCRUN 0 = Oscillator has stopped or has been disabled
bool MCP7940NRTC::isRunning() {
    return getRegisterBit(MCP7940N_WKDAY_REG, 5);
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
    return getRegisterBit(MCP7940N_WKDAY_REG, 3);
}

void MCP7940NRTC::enableBattery() {
    setRegisterBit(MCP7940N_WKDAY_REG, 3, 1);
}

void MCP7940NRTC::disableBattery() {
    setRegisterBit(MCP7940N_WKDAY_REG, 3, 0);
}

// *********************************************
// PRIVATE METHODS
// *********************************************
//Returns true if communication was successful
//Sets _exists flag
bool MCP7940NRTC::endTransmission() {
    if (Wire.endTransmission() != 0) {
        _exists = false;
        return false;
    }
    _exists = true;
    return true;
}

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
    endTransmission();
}

//Check that bit bitNum of registerAddress is 1
bool MCP7940NRTC::getRegisterBit(const uint8_t regAddr, const uint8_t bitNum) const {
    return (getRegister(regAddr) >> bitNum) & 1;
}

void MCP7940NRTC::setRegisterBit(const uint8_t regAddr, const uint8_t bitNum, const bool bitValue) {
    uint8_t regValue = getRegister(regAddr);
    //Set to one
    if (bitValue == 1) {
        regValue |= 1 << bitNum;
    //Set to zero
    } else {
        regValue &= ~(1 << bitNum);
    }
    setRegister(regAddr, regValue);
}

//Makes use of EXTOSC bit which is bit 3 of 0x07 register.
//EXTOSC 1 = Enable X1 pin to be driven by external 32.768 kHz source
//EXTOSC 0 = Disable external 32.768 kHz input.
bool MCP7940NRTC::getExtOscStatus() const {
    return getRegisterBit(MCP7940N_CONTROL_REG, 3);
}

void MCP7940NRTC::enableExtOsc() {
    setRegisterBit(MCP7940N_CONTROL_REG, 3, 1);
}

void MCP7940NRTC::disableExtOsc() {
    setRegisterBit(MCP7940N_CONTROL_REG, 3, 0);
}

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t MCP7940NRTC::dec2bcd(uint8_t num) {
    return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t MCP7940NRTC::bcd2dec(uint8_t num) {
    return ((num/16 * 10) + (num % 16));
}

