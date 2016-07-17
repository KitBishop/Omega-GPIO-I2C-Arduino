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
    I2C_BAD_PROBE = 7,
    I2C_TIME_OUT = 8
};

typedef struct I2C_Data {
    int size;
    unsigned char data[I2C_BUFFER_SIZE];
} I2C_Data;

#define swap16(v)       ((((uint16_t)(v) & 0xff) << 8) | (((uint16_t)(v) & 0xff00) >> 8))
#define swap32(v)       ((((uint32_t)(v) & 0xff) << 24) | (((uint32_t)(v) & 0xff00) << 8) | (((uint32_t)(v) & 0xff0000) >> 8) | (((uint32_t)(v) & 0xff000000) >> 24))

#ifdef __cplusplus
}
#endif

#endif /* I2CTYPES_H */

