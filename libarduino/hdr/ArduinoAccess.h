#ifndef ArduinoAccess_H
#define ArduinoAccess_H

#include "ArduinoAccessTypes.h"
#include "ArduinoAccessInfo.h"
#include "I2CAccess.h"
#include "I2CDevice.h"
#include "GPIOPin.h"

#define arduinoLibraryVersion  "1.0.0"
#define MAX_PORT                    15

class ArduinoAccessSigHandler;
class ArduinoPort;

typedef struct SignalData {
    unsigned char port;
    I2C_Data linkData;
} SignalData;

class ArduinoAccess {
friend ArduinoPort;
friend ArduinoAccessSigHandler;
public:
    static Arduino_Result registerPort(ArduinoPort * arduinoPort);
    
    static Arduino_Result rebootArduino(ArduinoPort * arduinoPort);
    
    static Arduino_Result sendCmd(ArduinoPort * arduinoPort, unsigned char cmd);
    static Arduino_Result send8(ArduinoPort * arduinoPort, unsigned char cmd, unsigned char val);
    static Arduino_Result send16(ArduinoPort * arduinoPort, unsigned char cmd, unsigned int val);
    static Arduino_Result send32(ArduinoPort * arduinoPort, unsigned char cmd, unsigned long val);
    static Arduino_Result sendBuffer(ArduinoPort * arduinoPort, unsigned char cmd, I2C_Data i2cData);

    static Arduino_Result getStatus(ArduinoPort * arduinoPort);
    static Arduino_Result get8(ArduinoPort * arduinoPort, unsigned char & val);
    static Arduino_Result get16(ArduinoPort * arduinoPort, unsigned int & val);
    static Arduino_Result get32(ArduinoPort * arduinoPort, unsigned long & val);
    static Arduino_Result getBuffer(ArduinoPort * arduinoPort, I2C_Data & i2cData, int numBytes);
    
    static char * getLibVersion();
private:
    static map<int, AccessInfo *> portInfo;
    
    static Arduino_Result sendCmdInternal(unsigned char devAddr, unsigned char portN, unsigned char cmd);
    static Arduino_Result send8Internal(unsigned char devAddr, unsigned char portN, unsigned char cmd, unsigned char val);
    static Arduino_Result getStatusInternal(unsigned char devAddr, unsigned char portN, long int delayMS);
    static Arduino_Result get8Internal(unsigned char devAddr, unsigned char portN, unsigned char & val, long int delayMS);
    static Arduino_Result get16Internal(unsigned char devAddr, unsigned char portN, unsigned int & val, long int delayMS);
    static Arduino_Result getBufferInternal(unsigned char devAddr, unsigned char portN, I2C_Data & i2cData, int numBytes, unsigned int delayMS);
    static Arduino_Result getSignalDataInternal(unsigned char devAddr, unsigned char portN, SignalData &sigData);
    static Arduino_Result getSigPorts(unsigned char devAddr, unsigned int &sigPorts);
    static void clearSignal(unsigned char devAddr);
    
    static Arduino_Result setSignalHandlerInternal(ArduinoPort * arduinoPort, int pin);
    
    static Arduino_Result sendNBytesInternal(ArduinoPort * arduinoPort, unsigned char cmd, unsigned long val, int numBytes);
    static Arduino_Result getNBytesInternal(unsigned char devAddr, unsigned char portN, unsigned long & val, int numBytes, long int delayMS);
};

#endif
