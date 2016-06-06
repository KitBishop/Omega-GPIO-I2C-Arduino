#ifndef ArduionAccessTypes_H
#define ArduionAccessTypes_H

#include <unistd.h>

#include "I2CTypes.h"

#define DEFAULT_ARDUINO_DEV_ADDR    0x08
#define DEFAULT_ARDUINO_SYSPORT     0

#define SIGNAL_PORT                 255

#define ARDUINO_SIG_GETSIGPORT      253
#define ARDUINO_SIG_GETSIGSZ        254
#define ARDUINO_SIG_GETSIGDAT       255

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
    ARDUINO_NO_LINK = 9
};

typedef struct SignalData {
    unsigned char port;
    I2C_Data linkData;
} SignalData;

typedef void (*Arduino_Sig_Handler_Func) (I2C_Data &linkData);

class Arduino_Sig_Handler_Object {
public:
    virtual void handleSignalData(I2C_Data &linkData) = 0;
};

#endif
