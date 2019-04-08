#include <Wire.h>
#include <TimeLib.h>
#include <MCP7940NRTC.h>

//Change these if needed
const uint8_t SCLPIN = A4;
const uint8_t SDAPIN = A5;

//Declare RTC object
MCP7940NRTC RTC = MCP7940NRTC(SDAPIN, SCLPIN);

void setup() {
  Serial.begin(115200);
  while (!Serial) ; // wait for serial
  delay(200);
  Serial.println("MCP7940N Read Test");
  Serial.println("-------------------");
}

void loop() {
  tmElements_t tm;

  if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The MCP7940N is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("MCP7940N read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
