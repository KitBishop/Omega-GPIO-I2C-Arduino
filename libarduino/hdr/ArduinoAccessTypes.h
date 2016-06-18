#ifndef ArduinoAccessTypes_H
#define ArduinoAccessTypes_H

#include <unistd.h>

#include "I2CTypes.h"

#define DEFAULT_ARDUINO_DEV_ADDR    0x08
#define DEFAULT_ARDUINO_SYSPORT     0

enum Arduino_Result {
    ARDUINO_OK = 0,
    ARDUINO_UNKNOWN_COMMAND = 1,
    ARDUINO_DATA_ERR = 2,
    ARDUINO_I2C_ERR = 3,
    ARDUINO_BAD_PIN = 4,
    ARDUINO_BAD_PORT = 5,
    ARDUINO_SIG_PENDING = 6,
    ARDUINO_BAD_SIG = 7,
    ARDUINO_BAD_READ_LEN = 8,
    ARDUINO_NO_LINK = 9,
    ARDUINO_TIMEOUT = 10
};

typedef void (*Arduino_Sig_Handler_Func) (unsigned char devAddr, unsigned char portN, I2C_Data &linkData);

class Arduino_Sig_Handler_Object {
public:
    virtual void handleSignalData(unsigned char devAddr, unsigned char portN, I2C_Data &linkData) = 0;
};

#endif
