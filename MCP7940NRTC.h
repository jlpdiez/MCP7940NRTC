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
    time_t get();
    bool set(time_t t);
    bool read(tmElements_t &tm);
    bool write(tmElements_t &tm);
    bool chipPresent() { return _exists; }
    bool isRunning();
    void setConfig(uint8_t confValue);
    uint8_t getConfig() const;
    //void setCalibration(char calValue);
    //char getCalibration();

  private:
    uint8_t _sdaPin, _sclPin;
    bool _exists;
    uint8_t getRegister(const uint8_t regAddr) const;
    void setRegister(const uint8_t regAddr, const uint8_t regData);
    static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num);
};

#endif
 

