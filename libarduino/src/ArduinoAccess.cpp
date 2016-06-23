#include <string.h>

#include "ArduinoAccess.h"
#include "ArduinoPort.h"
#include "GPIOAccess.h"
#include "ArduinoAccessSigHandler.h"

#define ARDUINO_CMD_GETSIGNALPORTS      250
#define ARDUINO_CMD_SETREADPORT         251
#define ARDUINO_CMD_GETSIGSIZE          252
#define ARDUINO_CMD_GETSIGDATA          253
#define ARDUINO_CMD_CLEARSIGNALS        254
#define ARDUINO_CMD_REBOOT              255

#define COMMAND_PORT                    255

map<int, AccessInfo *> ArduinoAccess::portInfo = map<int, AccessInfo *>();

Arduino_Result ArduinoAccess::registerPort(ArduinoPort * arduinoPort) {
    int pN = arduinoPort->arduinoPortNum;
    int devAddr = arduinoPort->arduinoDevAddr;
    
    if ((pN < 0) || (pN > MAX_PORT) || (devAddr <= 3) || (devAddr > 0x7f)) {
        return ARDUINO_BAD_PORT;
    }

    AccessInfo * aInf;
    auto infItem = portInfo.find(devAddr);
    if (infItem == portInfo.end()) {
        aInf = new AccessInfo();
        aInf->signalPin = NULL;
        aInf->ports = map<int, ArduinoPort *>();
        
        portInfo.insert(std::make_pair(devAddr, aInf));
    } else {
        aInf = infItem->second;
    }
    
    auto portItem = aInf->ports.find(pN);
    if (portItem == aInf->ports.end()) {
        aInf->ports.insert(std::make_pair(pN, arduinoPort));
    }
    
    return ARDUINO_OK;
}

Arduino_Result ArduinoAccess::sendNBytesInternal(ArduinoPort * arduinoPort, unsigned char cmd, unsigned long val, int numBytes) {
    I2C_Data i2cData;
    int index;
    for (index = 0; index < numBytes; index++) {
        i2cData.data[index] = (uint8_t) ((val >> (8 * index)) & 0xff);
    }
    i2cData.size = numBytes;

    return sendBuffer(arduinoPort, cmd, i2cData);
}

Arduino_Result ArduinoAccess::getNBytesInternal(unsigned char devAddr, unsigned char portN, unsigned long & val, int numBytes, long int delayMS) {
    val = 0;

    I2C_Data i2cData;

    Arduino_Result res = getBufferInternal(devAddr, portN, i2cData, numBytes, delayMS);
    if (res == ARDUINO_OK) {
        if (i2cData.size != numBytes) {
            res = ARDUINO_BAD_READ_LEN;
        } else {
            int i;
            for (i = 0; i < numBytes; i++) {
                val = val + (i2cData.data[i] << (i * 8));
            }
        }
    }
    
    return res;
}

Arduino_Result ArduinoAccess::sendCmdInternal(unsigned char devAddr, unsigned char portN, unsigned char cmd) {
    I2C_Result res = I2CAccess::write8(devAddr, portN, cmd);
    
    if (res == I2C_OK) {
        return ARDUINO_OK;
    } else if (res == I2C_TIME_OUT) {
        return ARDUINO_TIMEOUT;
    } else {
        return ARDUINO_I2C_ERR;
    }
}

Arduino_Result ArduinoAccess::send8Internal(unsigned char devAddr, unsigned char portN, unsigned char cmd, unsigned char val) {
    I2C_Data i2cData;
    i2cData.data[0] = cmd;
    i2cData.data[1] = val;
    i2cData.size = 2;
    
    I2C_Result res = I2CAccess::writeBuffer(devAddr, portN, i2cData);
    
    if (res == I2C_OK) {
        return ARDUINO_OK;
    } else if (res == I2C_TIME_OUT) {
        return ARDUINO_TIMEOUT;
    } else {
        return ARDUINO_I2C_ERR;
    }
}

Arduino_Result ArduinoAccess::sendCmd(ArduinoPort * arduinoPort, unsigned char cmd) {
    return sendCmdInternal(arduinoPort->arduinoDevAddr, arduinoPort->arduinoPortNum, cmd);
}

Arduino_Result ArduinoAccess::send8(ArduinoPort * arduinoPort, unsigned char cmd, unsigned char val) {
    return sendNBytesInternal(arduinoPort, cmd, val, 1);
}

Arduino_Result ArduinoAccess::send16(ArduinoPort * arduinoPort, unsigned char cmd, unsigned int val) {
    return sendNBytesInternal(arduinoPort, cmd, val, 2);
}
Arduino_Result ArduinoAccess::send32(ArduinoPort * arduinoPort, unsigned char cmd, unsigned long val) {
    return sendNBytesInternal(arduinoPort, cmd, val, 4);
}
Arduino_Result ArduinoAccess::sendBuffer(ArduinoPort * arduinoPort, unsigned char cmd, I2C_Data i2cData) {
    I2C_Data i2cDataNew;

    i2cDataNew.data[0] = cmd & 0xff;
    memcpy(&i2cDataNew.data[1], &i2cData.data[0], i2cData.size);
    i2cDataNew.size = i2cData.size + 1;

    I2C_Result res = I2CAccess::writeBuffer(arduinoPort->arduinoDevAddr, arduinoPort->arduinoPortNum, i2cDataNew);
    if (res == I2C_OK) {
        return ARDUINO_OK;
    } else if (res == I2C_TIME_OUT) {
        return ARDUINO_TIMEOUT;
    } else {
        return ARDUINO_I2C_ERR;
    }
}

Arduino_Result ArduinoAccess::getStatus(ArduinoPort * arduinoPort) {
    I2C_Data i2cData;
    return getBuffer(arduinoPort, i2cData, 0);
}

Arduino_Result ArduinoAccess::getStatusInternal(unsigned char devAddr, unsigned char portN, long int delayMS) {
    I2C_Data i2cData;
    return getBufferInternal(devAddr, portN, i2cData, 0, delayMS);
}

Arduino_Result ArduinoAccess::get8Internal(unsigned char devAddr, unsigned char portN, unsigned char & val, long int delayMS) {
    unsigned long v;

    Arduino_Result res = getNBytesInternal(devAddr, portN, v, 1, delayMS);
    if (res == ARDUINO_OK) {
        val = v & 0xff;
    }
    return res;
}

Arduino_Result ArduinoAccess::get16Internal(unsigned char devAddr, unsigned char portN, unsigned int & val, long int delayMS) {
    unsigned long v;

    Arduino_Result res = getNBytesInternal(devAddr, portN, v, 2, delayMS);
    if (res == ARDUINO_OK) {
        val = v & 0xffff;
    }
    return res;
}

Arduino_Result ArduinoAccess::get8(ArduinoPort * arduinoPort, unsigned char & val) {
    return get8Internal(arduinoPort->arduinoDevAddr, arduinoPort->arduinoPortNum, val, arduinoPort->responseDelayMS);
}

Arduino_Result ArduinoAccess::get16(ArduinoPort * arduinoPort, unsigned int & val) {
    unsigned long v;

    Arduino_Result res = getNBytesInternal(arduinoPort->arduinoDevAddr, arduinoPort->arduinoPortNum, v, 2, arduinoPort->responseDelayMS);
    if (res == ARDUINO_OK) {
        val = v & 0xffff;
    }
    return res;
}

Arduino_Result ArduinoAccess::get32(ArduinoPort * arduinoPort, unsigned long & val) {
    unsigned long v;

    Arduino_Result res = getNBytesInternal(arduinoPort->arduinoDevAddr, arduinoPort->arduinoPortNum, v, 4, arduinoPort->responseDelayMS);
    if (res == ARDUINO_OK) {
        val = v;
    }
    return res;
}

Arduino_Result ArduinoAccess::getBufferInternal(unsigned char devAddr, unsigned char portN, I2C_Data & i2cData, int numBytes, unsigned int delayMS) {
    I2C_Data i2cReadData;
    usleep(delayMS * 1000L);

    Arduino_Result aRes = send8Internal(devAddr, COMMAND_PORT, ARDUINO_CMD_SETREADPORT, portN);

    if (aRes != ARDUINO_OK) {
        return aRes;
    }

    I2C_Result i2cRes = I2CAccess::readBuffer(devAddr, i2cReadData, numBytes + 1);
    if (i2cRes == I2C_OK) {
        if (i2cReadData.size <= 0) {
            return ARDUINO_BAD_READ_LEN;
        }
        
        if (i2cReadData.data[0] == ARDUINO_OK) {
            int i;
            for (i = 0; i < (i2cReadData.size - 1); i++) {
                i2cData.data[i] = i2cReadData.data[i + 1];
            }
            i2cData.size = i2cReadData.size - 1;
            return ARDUINO_OK;
        } else {
            return (Arduino_Result)i2cReadData.data[0];
        }
    } else if (i2cRes == I2C_TIME_OUT) {
        return ARDUINO_TIMEOUT;
    } else {
        return ARDUINO_I2C_ERR;
    }
}

Arduino_Result ArduinoAccess::getBuffer(ArduinoPort * arduinoPort, I2C_Data & i2cData, int numBytes) {
    return getBufferInternal(arduinoPort->arduinoDevAddr, arduinoPort->arduinoPortNum, i2cData, numBytes, arduinoPort->responseDelayMS);
}

Arduino_Result ArduinoAccess::setSignalHandlerInternal(ArduinoPort * arduinoPort, int pin) {
    GPIO_Result gres = GPIO_OK;
    if (pin >= 0) {
        if (!GPIOAccess::isPinUsable(pin)) {
            return ARDUINO_BAD_PIN;
        }
    }

    AccessInfo * accessInfo;
    auto infItem = ArduinoAccess::portInfo.find(arduinoPort->arduinoDevAddr);
    if (infItem != ArduinoAccess::portInfo.end()) {
        accessInfo = infItem->second;
    } else {
        return ARDUINO_BAD_SIG;
    }
    
    if (pin >= 0) {
        if ((accessInfo->signalPin != NULL) && (accessInfo->signalPin->getPinNumber() != pin)) {
            delete accessInfo->signalPin;
            accessInfo->signalPin = NULL;
        }
    }
    
    if (accessInfo->signalPin == NULL) {
        accessInfo->signalPin = new GPIOPin(pin);
        gres = accessInfo->signalPin->setDirection(GPIO_Direction::GPIO_INPUT);
    }
    
    if (gres == GPIO_OK) {
        bool enabled;
        gres = accessInfo->signalPin->irqEnabled(enabled);
        if ((gres ==GPIO_OK) && !enabled) {
            gres = accessInfo->signalPin->setIrq(GPIO_Irq_Type::GPIO_IRQ_RISING, new ArduinoAccessSigHandler(arduinoPort->arduinoDevAddr), 0);
        }
    }
    
    if (gres != GPIO_OK) {
        return ARDUINO_BAD_SIG;
    }
    
    return ARDUINO_OK;
}

Arduino_Result ArduinoAccess::getSignalDataInternal(unsigned char devAddr, unsigned char portN, SignalData &sigData) {
    Arduino_Result res;
    unsigned char sigSz;
    
    sigData.port = portN;
    
    res = send8Internal(devAddr, COMMAND_PORT, ARDUINO_CMD_GETSIGSIZE, portN);
    if (res == ARDUINO_OK) {
        res = get8Internal(devAddr, COMMAND_PORT, sigSz, 1);
        
        if (res == ARDUINO_OK) {
            res = send8Internal(devAddr, COMMAND_PORT, ARDUINO_CMD_GETSIGDATA, portN);
            if (res == ARDUINO_OK) {
                I2C_Data i2cData;

                res = getBufferInternal(devAddr, COMMAND_PORT, i2cData, sigSz, 1);
                if (res == ARDUINO_OK) {
                    int i;
                    for (i = 0; i < i2cData.size; i++) {
                        sigData.linkData.data[i] = i2cData.data[i];
                    }
                    sigData.linkData.size = i2cData.size;
                }
            }
        }
    }
    
    return res;
}

Arduino_Result ArduinoAccess::rebootArduino(ArduinoPort * arduinoPort) {
    
    Arduino_Result aRes = sendCmdInternal(arduinoPort->arduinoDevAddr, COMMAND_PORT, ARDUINO_CMD_REBOOT);    
    
    if (aRes == ARDUINO_OK) {
        // Wait for reboot to take effect
        usleep(1000L);
        aRes = getStatusInternal(arduinoPort->arduinoDevAddr, COMMAND_PORT, arduinoPort->responseDelayMS);
    }

    return aRes;
}

char * ArduinoAccess::getLibVersion() {
    return (char *)arduinoLibraryVersion;
}

Arduino_Result ArduinoAccess::getSigPorts(unsigned char devAddr, unsigned int &sigPorts) {
    Arduino_Result aRes = sendCmdInternal(devAddr, COMMAND_PORT, ARDUINO_CMD_GETSIGNALPORTS);    

    if (aRes == ARDUINO_OK) {
        aRes = get16Internal(devAddr, COMMAND_PORT, sigPorts, 1);
    }
    
    return aRes;
}

void ArduinoAccess::clearSignal(unsigned char devAddr) {
    Arduino_Result aRes = sendCmdInternal(devAddr, COMMAND_PORT, ARDUINO_CMD_CLEARSIGNALS);    

    if (aRes == ARDUINO_OK) {
        getStatusInternal(devAddr, COMMAND_PORT, 1);
    }
}
