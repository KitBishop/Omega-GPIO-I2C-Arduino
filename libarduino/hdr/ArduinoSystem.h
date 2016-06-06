#ifndef ArduinoSystem_H
#define ArduinoSystem_H

#include "ArduinoAccessTypes.h"
#include "ArduinoPort.h"
#include "GPIOPin.h"

#define MAX_PIN 19

// Digital pins
#define D0      0
#define D1      1
#define D2      2
#define D3      3
#define D4      4
#define D5      5
#define D6      6
#define D7      7
#define D8      8
#define D9      9
#define D10     10
#define D11     11
#define D12     12
#define D13     13

// Analog pins
#define A0      14
#define A1      15
#define A2      16
#define A3      17
#define A4      18
#define A5      19

// SPI pins
#define SS      D10
#define MOSI    D11
#define MISO    D12
#define SCK     D13

// I2C pins
#define SDA     A4
#define SCL     A5

enum ArduinoPinMode {
    INPUT = 0x0,
    OUTPUT = 0x1,
    INPUT_PULLUP = 0x2            
};

enum ArduinoPinVal {
    LOW = 0x0,
    HIGH = 0x1
};

enum ArduinoARefMode {
    DEFAULT = 0,
    EXTERNAL = 1,
    INTERNAL = 2
};

enum ArduinoBitOrder {
    LSBFIRST = 0,
    MSBFIRST = 1
};

class ArduinoSystem {
public:
    ArduinoSystem();
    ArduinoSystem(unsigned char portN);
    ArduinoSystem(unsigned char devAddr, unsigned char portN);

    void setResponseDelayMS(unsigned long MS);
    unsigned long getResponseDelayMS();

    Arduino_Result setSignalHandler(int pin, Arduino_Sig_Handler_Func handler);
    Arduino_Result setSignalHandler(int pin, Arduino_Sig_Handler_Object * handlerObj);
    
//Digital I/O

    Arduino_Result pinMode(unsigned char pin, ArduinoPinMode mode);
    Arduino_Result digitalWrite(unsigned char pin, ArduinoPinVal val);
    Arduino_Result digitalRead(unsigned char pin, ArduinoPinVal & val);

//Analog I/O

    Arduino_Result analogReference(ArduinoARefMode mode);
    Arduino_Result analogRead(unsigned char pin, unsigned int & val);
    Arduino_Result analogWrite(unsigned char pin, unsigned char val);

//Advanced I/O

    Arduino_Result tone(unsigned char pin, unsigned int freq, unsigned long durationMS = 0);
    Arduino_Result noTone(unsigned char pin);
    Arduino_Result shiftOut(unsigned char dataPin, unsigned char clockPin, ArduinoBitOrder order, unsigned char value);
    Arduino_Result shiftIn(unsigned char dataPin, unsigned char clockPin, ArduinoBitOrder order, unsigned char & val);
    Arduino_Result pulseIn(unsigned char pin, ArduinoPinVal pulseType, unsigned long & val);
    Arduino_Result pulseIn(unsigned char pin, ArduinoPinVal pulseType, unsigned long timeOutUS, unsigned long & val);

private:
    void setup();
    bool pinOk(unsigned char pin);
    
    ArduinoPort * arduinoPort;
};

#endif
