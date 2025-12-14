// Mock Arduino.h for native testing
// This file provides stubs for Arduino functions when testing on native platform

#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#include <cstdint>
#include <cstdio>

// Mock Serial class
class MockSerial {
public:
    void begin(long baud) { printf("Serial.begin(%ld)\n", baud); }
    void println(const char* str) { printf("%s\n", str); }
    void print(const char* str) { printf("%s", str); }
    operator bool() { return true; }
};

extern MockSerial Serial;

// Arduino timing functions (declared here, implemented in test file)
extern unsigned long millis();
extern void delay(unsigned long ms);

#endif // MOCK_ARDUINO_H
