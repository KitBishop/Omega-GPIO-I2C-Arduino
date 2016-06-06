#include "I2CDevice.h"
#include "I2CAccess.h"

I2CDevice::I2CDevice(unsigned char devAddr) {
    deviceAddr = devAddr;
}

I2C_Result I2CDevice::probe() {
    return I2CAccess::probe(deviceAddr);
}

I2C_Result I2CDevice::write8(unsigned char val) {
    return I2CAccess::write8(deviceAddr, val);
}

I2C_Result I2CDevice::write16(unsigned int val) {
    return I2CAccess::write16(deviceAddr, val);
}

I2C_Result I2CDevice::write32(unsigned long val) {
    return I2CAccess::write32(deviceAddr, val);
}

I2C_Result I2CDevice::writeBuffer(I2C_Data data) {
    return I2CAccess::writeBuffer(deviceAddr, data);
}

I2C_Result I2CDevice::write8(unsigned char regAddr, unsigned char val) {
    return I2CAccess::write8(deviceAddr, regAddr, val);
}

I2C_Result I2CDevice::write16(unsigned char regAddr, unsigned int val) {
    return I2CAccess::write16(deviceAddr, regAddr, val);
}

I2C_Result I2CDevice::write32(unsigned char regAddr, unsigned long val) {
    return I2CAccess::write32(deviceAddr, regAddr, val);
}

I2C_Result I2CDevice::writeBuffer(unsigned char regAddr, I2C_Data data) {
    return I2CAccess::writeBuffer(deviceAddr, regAddr, data);
}

I2C_Result I2CDevice::read8(unsigned char & val) {
    return I2CAccess::read8(deviceAddr, val);
}

I2C_Result I2CDevice::read16(unsigned int & val) {
    return I2CAccess::read16(deviceAddr, val);
}

I2C_Result I2CDevice::read32(unsigned long & val) {
    return I2CAccess::read32(deviceAddr, val);
    
}

I2C_Result I2CDevice::readBuffer(I2C_Data & data, int numBytes) {
    return I2CAccess::readBuffer(deviceAddr, data, numBytes);
}

I2C_Result I2CDevice::read8(unsigned char regAddr, unsigned char & val) {
    return I2CAccess::read8(deviceAddr, regAddr, val);
}

I2C_Result I2CDevice::read16(unsigned char regAddr, unsigned int & val) {
    return I2CAccess::read16(deviceAddr, regAddr, val);
}

I2C_Result I2CDevice::read32(unsigned char regAddr, unsigned long & val) {
    return I2CAccess::read32(deviceAddr, regAddr, val);
}

I2C_Result I2CDevice::readBuffer(unsigned char regAddr, I2C_Data & data, int numBytes) {
    return I2CAccess::readBuffer(deviceAddr, regAddr, data, numBytes);
}

unsigned char I2CDevice::getDevAddr() {
    return deviceAddr;
}
