#include "ArduinoSystem.h"
#include "ArduinoAccessSigHandler.h"
#include "GPIOAccess.h"

#define ARDUINO_SYSCMD_MODE         1
#define ARDUINO_SYSCMD_WRITE        2
#define ARDUINO_SYSCMD_READ         3
#define ARDUINO_SYSCMD_AREF         4
#define ARDUINO_SYSCMD_AREAD        5
#define ARDUINO_SYSCMD_AWRITE       6
#define ARDUINO_SYSCMD_TONE         7
#define ARDUINO_SYSCMD_NOTONE       8
#define ARDUINO_SYSCMD_SHFTOUT      9
#define ARDUINO_SYSCMD_SHFTIN       10
#define ARDUINO_SYSCMD_PULSEIN      11

ArduinoSystem::ArduinoSystem() {
    arduinoPort = new ArduinoPort(DEFAULT_ARDUINO_DEV_ADDR, DEFAULT_ARDUINO_SYSPORT);
}

ArduinoSystem::ArduinoSystem(unsigned char portN) {
    arduinoPort = new ArduinoPort(DEFAULT_ARDUINO_DEV_ADDR, portN);
}

ArduinoSystem::ArduinoSystem(unsigned char devAddr, unsigned char portN) {
    arduinoPort = new ArduinoPort(devAddr, portN);
}

void ArduinoSystem::setResponseDelayMS(unsigned long MS) {
    arduinoPort->setResponseDelayMS(MS);
}

unsigned long ArduinoSystem::getResponseDelayMS() {
    return arduinoPort->getResponseDelayMS();
}

Arduino_Result ArduinoSystem::setSignalHandler(int pin, Arduino_Sig_Handler_Func handler) {
    return arduinoPort->setSignalHandler(pin, handler);
}

Arduino_Result ArduinoSystem::setSignalHandler(int pin, Arduino_Sig_Handler_Object * handlerObj) {
    return arduinoPort->setSignalHandler(pin, handlerObj);
}

Arduino_Result ArduinoSystem::pinMode(unsigned char pin, ArduinoPinMode mode) {
    if (!pinOk(pin)) {
        return ARDUINO_BAD_PIN;
    }
    I2C_Data i2cData;
    i2cData.data[0] = pin;
    i2cData.data[1] = mode;
    i2cData.size = 2;
    Arduino_Result res = arduinoPort->sendBuffer(ARDUINO_SYSCMD_MODE, i2cData);
    if (res == ARDUINO_OK) {
        res = arduinoPort->getStatus();
    }
    return res;
}

Arduino_Result ArduinoSystem::digitalWrite(unsigned char pin, ArduinoPinVal val) {
    if (!pinOk(pin)) {
        return ARDUINO_BAD_PIN;
    }
    I2C_Data i2cData;
    i2cData.data[0] = pin;
    i2cData.data[1] = val;
    i2cData.size = 2;
    Arduino_Result res = arduinoPort->sendBuffer(ARDUINO_SYSCMD_WRITE, i2cData);
    if (res == ARDUINO_OK) {
        res = arduinoPort->getStatus();
    }
    return res;
}

Arduino_Result ArduinoSystem::digitalRead(unsigned char pin, ArduinoPinVal & val) {
    if (!pinOk(pin)) {
        return ARDUINO_BAD_PIN;
    }
    Arduino_Result res = arduinoPort->send8(ARDUINO_SYSCMD_READ, pin);
    if (res == ARDUINO_OK) {
        unsigned char v;
        res = arduinoPort->get8(v);
        if (res == ARDUINO_OK) {
            val = (ArduinoPinVal)v;
        }
    }
    return res;
}

Arduino_Result ArduinoSystem::analogReference(ArduinoARefMode mode) {
    Arduino_Result res = arduinoPort->send8(ARDUINO_SYSCMD_AREF, mode);
    if (res == ARDUINO_OK) {
        res = arduinoPort->getStatus();
    }
    return res;
}

Arduino_Result ArduinoSystem::analogRead(unsigned char pin, unsigned int & val) {
    if (!pinOk(pin)) {
        return ARDUINO_BAD_PIN;
    }
    Arduino_Result res = arduinoPort->send8(ARDUINO_SYSCMD_AREAD, pin);
    if (res == ARDUINO_OK) {
        res = arduinoPort->get16(val);
    }
    return res;
}

Arduino_Result ArduinoSystem::analogWrite(unsigned char pin, unsigned char val) {
    if (!pinOk(pin)) {
        return ARDUINO_BAD_PIN;
    }
    I2C_Data i2cData;
    i2cData.data[0] = pin;
    i2cData.data[1] = val;
    i2cData.size = 2;
    Arduino_Result res = arduinoPort->sendBuffer(ARDUINO_SYSCMD_AWRITE, i2cData);
    if (res == ARDUINO_OK) {
        res = arduinoPort->getStatus();
    }
    return res;
}

Arduino_Result ArduinoSystem::tone(unsigned char pin, unsigned int freq, unsigned long durationMS) {
    if (!pinOk(pin)) {
        return ARDUINO_BAD_PIN;
    }
    I2C_Data i2cData;
    i2cData.data[0] = pin;
    i2cData.data[1] = freq & 0xff;
    i2cData.data[2] = (freq >> 8) & 0xff;
    i2cData.data[3] = durationMS & 0xff;
    i2cData.data[4] = (durationMS >> 8) & 0xff;
    i2cData.data[5] = (durationMS >> 16) & 0xff;
    i2cData.data[6] = (durationMS >> 24) & 0xff;
    i2cData.size = 7;
    Arduino_Result res = arduinoPort->sendBuffer(ARDUINO_SYSCMD_TONE, i2cData);
    if (res == ARDUINO_OK) {
        res = arduinoPort->getStatus();
    }
    return res;
}

Arduino_Result ArduinoSystem::noTone(unsigned char pin) {
    if (!pinOk(pin)) {
        return ARDUINO_BAD_PIN;
    }
    I2C_Data i2cData;
    i2cData.data[0] = pin;
    i2cData.size = 1;
    Arduino_Result res = arduinoPort->sendBuffer(ARDUINO_SYSCMD_NOTONE, i2cData);
    if (res == ARDUINO_OK) {
        res = arduinoPort->getStatus();
    }
    return res;
}

Arduino_Result ArduinoSystem::shiftOut(unsigned char dataPin, unsigned char clockPin, ArduinoBitOrder order, unsigned char value) {
    if ((!pinOk(dataPin)) || (!pinOk(clockPin))) {
        return ARDUINO_BAD_PIN;
    }
    I2C_Data i2cData;
    i2cData.data[0] = dataPin;
    i2cData.data[1] = clockPin;
    i2cData.data[2] = order;
    i2cData.data[3] = value;
    i2cData.size = 4;
    Arduino_Result res = arduinoPort->sendBuffer(ARDUINO_SYSCMD_SHFTOUT, i2cData);
    if (res == ARDUINO_OK) {
        res = arduinoPort->getStatus();
    }
    return res;
}

Arduino_Result ArduinoSystem::shiftIn(unsigned char dataPin, unsigned char clockPin, ArduinoBitOrder order, unsigned char & val) {
    if ((!pinOk(dataPin)) || (!pinOk(clockPin))) {
        return ARDUINO_BAD_PIN;
    }
    I2C_Data i2cData;
    i2cData.data[0] = dataPin;
    i2cData.data[1] = clockPin;
    i2cData.data[2] = order;
    i2cData.size = 3;
    Arduino_Result res = arduinoPort->sendBuffer(ARDUINO_SYSCMD_SHFTIN, i2cData);
    if (res == ARDUINO_OK) {
        unsigned char v;
        res = arduinoPort->get8(v);
        if (res == ARDUINO_OK) {
            val = v;
        }
    }
    return res;
}

Arduino_Result ArduinoSystem::pulseIn(unsigned char pin, ArduinoPinVal pulseType, unsigned long & val) {
    return pulseIn(pin, pulseType, 0, val);
}

Arduino_Result ArduinoSystem::pulseIn(unsigned char pin, ArduinoPinVal pulseType, unsigned long timeOutUS, unsigned long & val) {
    if (!pinOk(pin)) {
        return ARDUINO_BAD_PIN;
    }
    I2C_Data i2cData;
    i2cData.data[0] = pin;
    i2cData.data[1] = pulseType;
    i2cData.data[2] = timeOutUS & 0xff;
    i2cData.data[3] = (timeOutUS >> 8) & 0xff;
    i2cData.data[4] = (timeOutUS >> 16) & 0xff;
    i2cData.data[5] = (timeOutUS >> 24) & 0xff;
    i2cData.size = 6;
    Arduino_Result res = arduinoPort->sendBuffer(ARDUINO_SYSCMD_PULSEIN, i2cData);
    if (res == ARDUINO_OK) {
        unsigned long v;
        res = arduinoPort->get32(v);
        if (res == ARDUINO_OK) {
            val = v;
        }
    }
    return res;
}

bool ArduinoSystem::pinOk(unsigned char pin) {
    return (pin >= 0) && (pin <= MAX_PIN);
}