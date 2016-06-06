#ifndef I2CTypes_h
#define I2CTypes_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_BUFFER_SIZE  32

enum I2C_Result {
    I2C_OK = 0,
    I2C_BAD_ACCESS = 1,
    I2C_BAD_DEV = 2,
    I2C_BAD_WRITE = 3,
    I2C_BAD_READ = 4,
    I2C_BAD_READ_ADDR = 5,
    I2C_BAD_READ_LEN = 6,
    I2C_BAD_PROBE = 7
};

//typedef uint8_t I2C_Buffer[I2C_BUFFER_SIZE], * I2C_Buffer_P;

typedef struct I2C_Data {
    int size;
    unsigned char data[I2C_BUFFER_SIZE];
} I2C_Data;

#ifdef __cplusplus
}
#endif

#endif /* I2CTYPES_H */

