#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "ArduinoPort.h"

ArduinoPort::ArduinoPort(unsigned char portN) {
    arduinoDevAddr = DEFAULT_ARDUINO_DEV_ADDR;
    arduinoPortNum = portN;
    responseDelayMS = 1;
    sigHandler = NULL;
    sigHandlerObj = NULL;
    ArduinoAccess::registerPort(this);
}

ArduinoPort::ArduinoPort(unsigned char devAddr, unsigned char portN) {
    arduinoDevAddr = devAddr;
    arduinoPortNum = portN;
    responseDelayMS = 1;
    sigHandler = NULL;
    sigHandlerObj = NULL;
    ArduinoAccess::registerPort(this);
}

Arduino_Result ArduinoPort::sendCmd(unsigned char cmd) {
    return ArduinoAccess::sendCmd(this, cmd);
}

Arduino_Result ArduinoPort::send8(unsigned char cmd, unsigned char val) {
    return ArduinoAccess::send8(this, cmd, val);
}

Arduino_Result ArduinoPort::send16(unsigned char cmd, unsigned int val) {
    return ArduinoAccess::send16(this, cmd, val);
}

Arduino_Result ArduinoPort::send32(unsigned char cmd, unsigned long val) {
    return ArduinoAccess::send32(this, cmd, val);
}

Arduino_Result ArduinoPort::sendBuffer(unsigned char cmd, I2C_Data i2cData) {
    return ArduinoAccess::sendBuffer(this, cmd, i2cData);
}

Arduino_Result ArduinoPort::get8(unsigned char & val) {
    return ArduinoAccess::get8(this, val);
}

Arduino_Result ArduinoPort::get16(unsigned int & val) {
    return ArduinoAccess::get16(this, val);
}

Arduino_Result ArduinoPort::get32(unsigned long & val) {
    return ArduinoAccess::get32(this, val);
}

Arduino_Result ArduinoPort::getBuffer(I2C_Data & i2cData, int numBytes) {
    return ArduinoAccess::getBuffer(this, i2cData, numBytes);
}

Arduino_Result ArduinoPort::getStatus() {
    return ArduinoAccess::getStatus(this);
}

void ArduinoPort::setResponseDelayMS(unsigned long MS) {
    responseDelayMS = MS;
}

unsigned long ArduinoPort::getResponseDelayMS() {
    return responseDelayMS;
}

Arduino_Result ArduinoPort::setSignalHandler(int pin, Arduino_Sig_Handler_Func handler) {
    sigHandler = handler;
    sigHandlerObj = NULL;
    
    Arduino_Result res = ArduinoAccess::setSignalHandler(this, pin);
    if (res != ARDUINO_OK) {
        sigHandler = NULL;
        sigHandlerObj = NULL;
    }
    
    return res;
}

Arduino_Result ArduinoPort::setSignalHandler(int pin, Arduino_Sig_Handler_Object * handlerObj) {
    sigHandler = NULL;
    sigHandlerObj = handlerObj;
    
    Arduino_Result res = ArduinoAccess::setSignalHandler(this, pin);
    if (res != ARDUINO_OK) {
        sigHandler = NULL;
        sigHandlerObj = NULL;
    }
    
    return res;
}
