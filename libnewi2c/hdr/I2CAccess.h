#ifndef I2CAccess_H
#define I2CAccess_H

#include <map>
using namespace std;

#include "I2CTypes.h"

#define DEFAULT_I2C_CHANNEL     0
#define i2cLibraryVersion  "1.0.0"

typedef struct retryInfo {
    unsigned int delayMS;
    int count;
} RetryInfo;

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

    static void setRetryDelay(unsigned char devAddr, unsigned int delayMS);
    static void setRetryCount(unsigned char devAddr, int count);
    static unsigned int getRetryDelay(unsigned char devAddr);
    static int getRetryCount(unsigned char devAddr);
    
    static bool isAccessOk();

    static char * getLibVersion();
    
private:
    static int i2cChannelFD;
    static int channelNumber;
    static map<unsigned char, RetryInfo> retryInfo;

    static I2C_Result checkAndSetupChannel();
    static I2C_Result checkAndSetupDev(int devAddr);
    
    static I2C_Result writeNBytes(unsigned char devAddr, unsigned long val, int numBytes);
    static I2C_Result writeNBytes(unsigned char devAddr, unsigned char regAddr, unsigned long val, int numBytes);
    static I2C_Result readNBytes(unsigned char devAddr, unsigned long & val, int numBytes);
    static I2C_Result readNBytes(unsigned char devAddr, unsigned char regAddr, unsigned long & val, int numBytes);
};

#endif
