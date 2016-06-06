#include <Arduino.h>
#include <avr/wdt.h>

#include "OmegaArduinoSystemPort.h"

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

OmegaArduinoSystemPort::OmegaArduinoSystemPort() 
    : OmegaPort(DEFAULT_ARDUINO_SYSPORT) {
}

OmegaArduinoSystemPort::OmegaArduinoSystemPort(unsigned char portN) 
    : OmegaPort(portN) {
}

ResponseData OmegaArduinoSystemPort::processCommand(unsigned char cmd, LinkData linkData) {
    ResponseData respData;
    respData.status = ARDUINO_OK;
    respData.size = 0;
    
    switch (cmd) {
    case ARDUINO_SYSCMD_MODE:
        if (linkData.size != 2) {
            respData.status = ARDUINO_DATA_ERR;
        } else {
            int pin = linkData.data[0];
            int val = linkData.data[1];
            pinMode(pin, val);
        }
        break;

    case ARDUINO_SYSCMD_WRITE:
        if (linkData.size != 2) {
            respData.status = ARDUINO_DATA_ERR;
        } else {
            int pin = linkData.data[0];
            int val = linkData.data[1];
            digitalWrite(pin, val);
        }
        break;

    case ARDUINO_SYSCMD_READ:
        if (linkData.size != 1) {
            respData.status = ARDUINO_DATA_ERR;
        } else {
            int pin = linkData.data[0];
            int res = digitalRead(pin);
            respData.data[0] = res;
            respData.size = 1;
        }
        break;

    case ARDUINO_SYSCMD_AREF:
        if (linkData.size != 1) {
            respData.status = ARDUINO_DATA_ERR;
        } else {
            int val = linkData.data[0];
            analogReference(val);
        }
        break;

    case ARDUINO_SYSCMD_AREAD:
        if (linkData.size != 1) {
            respData.status = ARDUINO_DATA_ERR;
        } else {
            int pin = linkData.data[0];
            int aval = analogRead(pin);
            respData.data[0] = aval & 0xff;
            respData.data[1] = (aval >> 8) & 0xff;
            respData.size = 2;
        }
        break;

    case ARDUINO_SYSCMD_AWRITE:
        if (linkData.size != 2) {
            respData.status = ARDUINO_DATA_ERR;
        } else {
            int pin = linkData.data[0];
            int val = linkData.data[1];
            analogWrite(pin, val);
        }
        break;

    case ARDUINO_SYSCMD_TONE:
        if (linkData.size != 7) {
            respData.status = ARDUINO_DATA_ERR;
        } else {
            int pin = linkData.data[0];
            unsigned int freq = linkData.data[1] +
                (((unsigned int)linkData.data[2]) << 8);
            unsigned long durationMS = linkData.data[3] +
                (((unsigned long)linkData.data[4]) << 8) +
                (((unsigned long)linkData.data[5]) << 16) +
                (((unsigned long)linkData.data[6]) << 24);
            tone(pin, freq, durationMS);
        }
        break;

    case ARDUINO_SYSCMD_NOTONE:
        if (linkData.size != 1) {
            respData.status = ARDUINO_DATA_ERR;
        } else {
            int pin = linkData.data[0];
            noTone(pin);
        }
        break;

    case ARDUINO_SYSCMD_SHFTOUT:
        if (linkData.size != 4) {
            respData.status = ARDUINO_DATA_ERR;
        } else {
            unsigned char dataPin = linkData.data[0];
            unsigned char clockPin = linkData.data[1];
            unsigned char order = linkData.data[2];
            unsigned char val = linkData.data[3];
            shiftOut(dataPin, clockPin, order, val);
        }
        break;

    case ARDUINO_SYSCMD_SHFTIN:
        if (linkData.size != 3) {
            respData.status = ARDUINO_DATA_ERR;
        } else {
            unsigned char dataPin = linkData.data[0];
            unsigned char clockPin = linkData.data[1];
            unsigned char order = linkData.data[2];
            int res = shiftIn(dataPin, clockPin, order);
            respData.data[0] = res;
            respData.size = 1;
        }
        break;

    case ARDUINO_SYSCMD_PULSEIN:
        if (linkData.size != 6) {
            respData.status = ARDUINO_DATA_ERR;
        } else {
            int pin = linkData.data[0];
            unsigned int pulseType = linkData.data[1];
            unsigned long timeOutUS = linkData.data[2] +
                (((unsigned long)linkData.data[3]) << 8) +
                (((unsigned long)linkData.data[4]) << 16) +
                (((unsigned long)linkData.data[5]) << 24);
            unsigned long res;
            interrupts();
            if (timeOutUS == 0) {
                res = pulseIn(pin, pulseType);
            } else {
                res = pulseIn(pin, pulseType, timeOutUS);
            }
            respData.data[0] = res & 0xff;
            respData.data[1] = (res >> 8) & 0xff;
            respData.data[2] = (res >> 16) & 0xff;
            respData.data[3] = (res >> 24) & 0xff;
            respData.size = 4;
        }
        break;

    default:
        respData.status = ARDUINO_UNKNOWN_COMMAND;
        break;
    }
    
    return respData;
}
