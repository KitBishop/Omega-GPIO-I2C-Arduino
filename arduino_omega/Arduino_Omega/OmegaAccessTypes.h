#ifndef OmegaAccessTypes_H
#define	OmegaAccessTypes_H

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
    ARDUINO_NO_LINK = 9
};

typedef struct LinkData {
    unsigned char data[32];
    int size;
} LinkData;

typedef struct ResponseData {
    Arduino_Result status;
    unsigned char data[32];
    int size;
} ResponseData;

#endif

