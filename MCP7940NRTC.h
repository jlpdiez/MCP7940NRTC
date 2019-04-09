// #############################################################################
// #
// # Name       : MCP7940N RTC Library
// # Author     : Juan L. Perez Diez <ender.vs.melkor at gmail>
//
// # Based on the library for the DS1307 https://github.com/PaulStoffregen/DS1307RTC
//
// #  This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef MCP7940NRTC_h
#define MCP7940NRTC_h

#include <TimeLib.h>

class MCP7940NRTC {

  public:
    MCP7940NRTC(uint8_t sdaPin, uint8_t sclPin);
    //Acquire data from hardware and return time_t
    //Returns 0 if there is a comms issue or oscillator is halted
    static time_t get();
    //Sets the HW clock to the time given in time_t format
    //Returns false if there was a communication error
    bool set(time_t t);
    //Returns the HW time in tmElements_t format
    //False if theres a comms issue or oscillator is halted
    static bool read(tmElements_t &tm);
    //Writes the given data to the HW registers
    //Returns false if there was a comms error
    bool write(tmElements_t &tm);
    //This is set to true when no communication errors exist
    bool chipPresent() { return _exists; }
    //Checks the oscillator running bit
    bool isRunning();
    //Checks the external battery status
    bool getBatteryStatus() const;
    //Sets the external battery
    void enableBattery();
    void disableBattery();
    //void setCalibration(char calValue);
   //char getCalibration();

  private:
    uint8_t _sdaPin, _sclPin;
    //Goes to true when comm was succesful
    static bool _exists;
    //Calls Wire.endTransmission()
    //Returns true if communication was successful
    //Sets _exists flag
    static bool endTransmission();
    //Returns full register
    uint8_t getRegister(const uint8_t regAddr) const;
    //Sets a register to given data
    void setRegister(const uint8_t regAddr, const uint8_t regData);
    //Returns a bit from the given register
    bool getRegisterBit(const uint8_t regAddr, const uint8_t bitNum) const;
    //Sets bit bitNum from regAddr to bitValue
    void setRegisterBit(const uint8_t regAddr, const uint8_t bitNum, const bool bitValue);
    //Helpers to convert to/from BCD
    static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num);
};

#endif