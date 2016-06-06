#include <stdlib.h>
#include <unistd.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>

#include "I2CAccess.h"

#define I2C_DEV_PATH "/dev/i2c/%d"
#define I2C_DEV_PATH_ALT  "/dev/i2c-%d"

int I2CAccess::i2cChannelFD = -1;
int I2CAccess::channelNumber = DEFAULT_I2C_CHANNEL;

bool I2CAccess::isAccessOk() {
    return checkAndSetupChannel() == I2C_OK;
}

I2C_Result I2CAccess::checkAndSetupChannel()
{
    if (i2cChannelFD > 0) {
        return I2C_OK;
    }
    
    char pathname[255];

    snprintf(pathname, sizeof (pathname), I2C_DEV_PATH, channelNumber);
    i2cChannelFD = open(pathname, O_RDWR);

    if (i2cChannelFD <= 0) {
        snprintf(pathname, sizeof (pathname), I2C_DEV_PATH_ALT, channelNumber);
        i2cChannelFD = open(pathname, O_RDWR);

        if (i2cChannelFD <= 0) {
            return I2C_BAD_ACCESS;
        }
    }

    return I2C_OK;
}

I2C_Result I2CAccess::checkAndSetupDev(int devAddr) {
    I2C_Result res = checkAndSetupChannel();
    if (res != I2C_OK) {
        return res;
    }

    // set to 7-bit addr
    if (ioctl(i2cChannelFD, I2C_TENBIT, 0) < 0) {
        return I2C_BAD_DEV;
    }

    // set the address
    if (ioctl(i2cChannelFD, I2C_SLAVE, devAddr) < 0) {
        return I2C_BAD_DEV;
    }

    return I2C_OK;
}

I2C_Result I2CAccess::probe(unsigned char devAddr) {
    I2C_Result res = checkAndSetupDev(devAddr);
    
    if (res == I2C_OK) {
	struct i2c_smbus_ioctl_data args;

	args.read_write = I2C_SMBUS_WRITE;
	args.command = 0;
	args.size = I2C_SMBUS_QUICK;
	args.data = NULL;

	if (ioctl(i2cChannelFD, I2C_SMBUS, &args) < 0) {
            res = I2C_BAD_PROBE;
        }
    }
    
    return res;
}

I2C_Result I2CAccess::writeNBytes(unsigned char devAddr, unsigned long val, int numBytes) {
    I2C_Data data;
    int index;
    for (index = 0; index < numBytes; index++) {
        data.data[index] = (uint8_t) ((val >> (8 * index)) & 0xff);
    }
    data.size = numBytes;

    return writeBuffer(devAddr, data);
}

I2C_Result I2CAccess::write8(unsigned char devAddr, unsigned char val) {
    return writeNBytes(devAddr, val, 1);
}

I2C_Result I2CAccess::write16(unsigned char devAddr, unsigned int val) {
    return writeNBytes(devAddr, val, 2);
}

I2C_Result I2CAccess::write32(unsigned char devAddr, unsigned long val) {
    return writeNBytes(devAddr, val, 4);
}

I2C_Result I2CAccess::writeBuffer(unsigned char devAddr, I2C_Data data) {
    I2C_Result res = checkAndSetupDev(devAddr);
    
    if (res == I2C_OK) {
        
        if (write(i2cChannelFD, data.data, data.size) != data.size) {
            res = I2C_BAD_WRITE;
        }
    }
    
    return res;
}

I2C_Result I2CAccess::writeNBytes(unsigned char devAddr, unsigned char regAddr, unsigned long val, int numBytes) {
    I2C_Data data;
    int index;
    for (index = 0; index < numBytes; index++) {
        data.data[index] = (uint8_t) ((val >> (8 * index)) & 0xff);
    }
    data.size = numBytes;

    return writeBuffer(devAddr, regAddr, data);
}

I2C_Result I2CAccess::write8(unsigned char devAddr, unsigned char regAddr, unsigned char val) {
    return writeNBytes(devAddr, regAddr, val, 1);
}

I2C_Result I2CAccess::write16(unsigned char devAddr, unsigned char regAddr, unsigned int val) {
    return writeNBytes(devAddr, regAddr, val, 2);
}

I2C_Result I2CAccess::write32(unsigned char devAddr, unsigned char regAddr, unsigned long val) {
    return writeNBytes(devAddr, regAddr, val, 4);
}

I2C_Result I2CAccess::writeBuffer(unsigned char devAddr, unsigned char regAddr, I2C_Data data) {
    I2C_Data dataNew;

    dataNew.data[0] = regAddr & 0xff;
    memcpy(&dataNew.data[1], &data.data[0], data.size);
    dataNew.size = data.size + 1;

    I2C_Result res = writeBuffer(devAddr, dataNew);
    
    return res;
}

I2C_Result I2CAccess::readNBytes(unsigned char devAddr, unsigned long & val, int numBytes) {
    val = 0;

    I2C_Data data;
    I2C_Result res = readBuffer(devAddr, data, numBytes);
    if (res == I2C_OK) {
        if (data.size != numBytes) {
            res = I2C_BAD_READ_LEN;
        } else {
            int ind;
            for (ind = 0; ind < numBytes; ind++) {
                val = val + (data.data[ind] << (ind * 8));
            }
        }
    }
    return res;
}

I2C_Result I2CAccess::read8(unsigned char devAddr, unsigned char & val) {
    unsigned long v;
    I2C_Result res = readNBytes(devAddr, v, 1);
    if (res == I2C_OK) {
        val = v & 0xff;
    }
    
    return res;
}

I2C_Result I2CAccess::read16(unsigned char devAddr, unsigned int & val) {
    unsigned long v;
    I2C_Result res = readNBytes(devAddr, v, 2);
    if (res == I2C_OK) {
        val = v & 0xffff;
    }
    
    return res;
}

I2C_Result I2CAccess::read32(unsigned char devAddr, unsigned long & val) {
    unsigned long v;
    I2C_Result res = readNBytes(devAddr, v, 4);
    if (res == I2C_OK) {
        val = v;
    }
    
    return res;
}

I2C_Result I2CAccess::readBuffer(unsigned char devAddr, I2C_Data & data, int numBytes) {
    I2C_Result res = checkAndSetupDev(devAddr);
    
    if (res == I2C_OK) {
        // Read the data 
        if (numBytes > 0) {
            int numRead;
            numRead = read(i2cChannelFD, &data.data[0], numBytes);
            if (numRead < 0) {
                res = I2C_BAD_READ_LEN;
            }
            data.size = numRead;
        }
    }
    
    return res;
}

I2C_Result I2CAccess::readNBytes(unsigned char devAddr, unsigned char regAddr, unsigned long & val, int numBytes) {
    I2C_Data data;
    I2C_Result res = readBuffer(devAddr, regAddr, data, numBytes);
    if (res == I2C_OK){
        if (data.size != numBytes) {
            res = I2C_BAD_READ_LEN;
        } else {
            int ind;
            for (ind = 0; ind < numBytes; ind++) {
                val = val + (data.data[ind] << (ind * 8));
            }
        }
    }
    return res;
}

I2C_Result I2CAccess::read8(unsigned char devAddr, unsigned char regAddr, unsigned char & val) {
    unsigned long v;
    I2C_Result res = readNBytes(devAddr, regAddr, v, 1);
    if (res == I2C_OK) {
        val = v & 0xff;
    }
    
    return res;
}

I2C_Result I2CAccess::read16(unsigned char devAddr, unsigned char regAddr, unsigned int & val) {
    unsigned long v;
    I2C_Result res = readNBytes(devAddr, regAddr, v, 2);
    if (res == I2C_OK) {
        val = v & 0xffff;
    }
    
    return res;
}

I2C_Result I2CAccess::read32(unsigned char devAddr, unsigned char regAddr, unsigned long & val) {
    unsigned long v;
    I2C_Result res = readNBytes(devAddr, regAddr, v, 4);
    if (res == I2C_OK) {
        val = v;
    }
    
    return res;
}

I2C_Result I2CAccess::readBuffer(unsigned char devAddr, unsigned char regAddr, I2C_Data & data, int numBytes) {
    I2C_Result res = write8(devAddr, regAddr);

    if (res == I2C_OK) {
        res = readBuffer(devAddr, data, numBytes);
    } else if (res == I2C_BAD_WRITE) {
        res = I2C_BAD_READ_ADDR;
    }
    
    return res;
}

char * I2CAccess::getLibVersion() {
    return (char *)i2cLibraryVersion;
}
