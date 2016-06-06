#ifndef I2CAccess_H
#define I2CAccess_H

#include "I2CTypes.h"

#define DEFAULT_I2C_CHANNEL     0
#define i2cLibraryVersion  "1.0.0"
    
class I2CAccess {
public:
    static I2C_Result probe(unsigned char devAddr);

    static I2C_Result write8(unsigned char devAddr, unsigned char val);
    static I2C_Result write16(unsigned char devAddr, unsigned int val);
    static I2C_Result write32(unsigned char devAddr, unsigned long val);
    static I2C_Result writeBuffer(unsigned char devAddr, I2C_Data data);

    static I2C_Result write8(unsigned char devAddr, unsigned char regAddr, unsigned char val);
    static I2C_Result write16(unsigned char devAddr, unsigned char regAddr, unsigned int val);
    static I2C_Result write32(unsigned char devAddr, unsigned char regAddr, unsigned long val);
    static I2C_Result writeBuffer(unsigned char devAddr, unsigned char regAddr, I2C_Data data);
    
    static I2C_Result read8(unsigned char devAddr, unsigned char & val);
    static I2C_Result read16(unsigned char devAddr, unsigned int & val);
    static I2C_Result read32(unsigned char devAddr, unsigned long & val);
    static I2C_Result readBuffer(unsigned char devAddr, I2C_Data & data, int numBytes);

    static I2C_Result read8(unsigned char devAddr, unsigned char regAddr, unsigned char & val);
    static I2C_Result read16(unsigned char devAddr, unsigned char regAddr, unsigned int & val);
    static I2C_Result read32(unsigned char devAddr, unsigned char regAddr, unsigned long & val);
    static I2C_Result readBuffer(unsigned char devAddr, unsigned char regAddr, I2C_Data & data, int numBytes);

    static bool isAccessOk();

    static char * getLibVersion();
    
private:
    static int i2cChannelFD;
    static int channelNumber;

    static I2C_Result checkAndSetupChannel();
    static I2C_Result checkAndSetupDev(int devAddr);
    
    static I2C_Result writeNBytes(unsigned char devAddr, unsigned long val, int numBytes);
    static I2C_Result writeNBytes(unsigned char devAddr, unsigned char regAddr, unsigned long val, int numBytes);
    static I2C_Result readNBytes(unsigned char devAddr, unsigned long & val, int numBytes);
    static I2C_Result readNBytes(unsigned char devAddr, unsigned char regAddr, unsigned long & val, int numBytes);
};

#endif
