#ifndef I2CDevice_h
#define I2CDevice_h

#include "I2CTypes.h"
#include "I2CAccess.h"

class I2CDevice {
public:
    I2CDevice(unsigned char devAddr);

    I2C_Result probe();
    
    I2C_Result write8(unsigned char val);
    I2C_Result write16(unsigned int val);
    I2C_Result write32(unsigned long val);
    I2C_Result writeBuffer(I2C_Data data);

    I2C_Result write8(unsigned char regAddr, unsigned char val);
    I2C_Result write16(unsigned char regAddr, unsigned int val);
    I2C_Result write32(unsigned char regAddr, unsigned long val);
    I2C_Result writeBuffer(unsigned char regAddr, I2C_Data data);
    
    I2C_Result read8(unsigned char & val);
    I2C_Result read16(unsigned int & val);
    I2C_Result read32(unsigned long & val);
    I2C_Result readBuffer(I2C_Data & data, int numBytes);

    I2C_Result read8(unsigned char regAddr, unsigned char & val);
    I2C_Result read16(unsigned char regAddr, unsigned int & val);
    I2C_Result read32(unsigned char regAddr, unsigned long & val);
    I2C_Result readBuffer(unsigned char regAddr, I2C_Data & data, int numBytes);

    void setRetryDelay(unsigned int delayMS);
    void setRetryCount(int count);
    unsigned int getRetryDelay();
    int getRetryCount();
    
    unsigned char getDevAddr();
    
private:
    unsigned char deviceAddr;
};

#endif
