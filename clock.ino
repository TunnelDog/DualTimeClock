// Luke Gamertsfelder
// CST and GMT Digital Clock For Dad, Chirtmas 2023
// 24 Hour Time for CST and regular for GMT

#include "TinyGPS++.h"
#include "SoftwareSerial.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

SoftwareSerial serial_connection(3, 4); // TX=pin 10, RX=pin 11
TinyGPSPlus gps; // This is the GPS object that will pretty much do all the grunt work with the NMEA data

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

bool isDST(int year, int month, int day, int hour) {
  // DST starts on the second Sunday of March and ends on the first Sunday of November
  if (month > 3 && month < 11) {
    return true;  // DST is in effect
  } else if (month == 3 && (day - ((hour + 24 - 2) / 24 * 7 - 13)) >= 0 && (day - ((hour + 24 - 2) / 24 * 7 - 13)) <= 6) {
    return true;  // DST starts
  } else if (month == 11 && (day - ((hour + 24 - 1) / 24 * 7 - 6)) >= 0 && (day - ((hour + 24 - 1) / 24 * 7 - 6)) <= 6) {
    return true;  // DST ends
  }
  return false;
}

void printTwoDigits(int digits) {
  if (digits < 10) {
    display.print('0');
  }
  display.print(digits);
}

void setup() {
  // gps
  Serial.begin(9600); // This opens up communications to the Serial monitor in the Arduino IDE
  serial_connection.begin(9600); // This opens up communications to the GPS
  // oled
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

void loop() {
  while (serial_connection.available()) // While there are characters to come from the GPS
  {
    gps.encode(serial_connection.read()); // This feeds the serial NMEA data into the library one char at a time
  }

  if (gps.time.isUpdated()) {
    display.clearDisplay();

    display.setTextSize(2);
    display.setTextColor(WHITE);

    // Displaying the UTC time
    display.setCursor(17, 17);
    printTwoDigits(gps.time.hour());
    display.print(':');
    printTwoDigits(gps.time.minute());
    display.print(':');
    printTwoDigits(gps.time.second());

    // Adjusting for CST timezone
    int cstHour = gps.time.hour() - 6; // UTC - 6 hours for CST

    // Check for daylight saving time
    if (isDST(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour())) {
      cstHour++;  // Add 1 hour during DST
    }

    if (cstHour < 0) {
      cstHour += 24; // Handle crossing midnight
    }

    display.setCursor(17, 0);
    printTwoDigits(cstHour);
    display.print(':');
    printTwoDigits(gps.time.minute());
    display.print(':');
    printTwoDigits(gps.time.second());

    display.display();
    delay(2000); // This delay gives us the exact time every 5 seconds
  }
}