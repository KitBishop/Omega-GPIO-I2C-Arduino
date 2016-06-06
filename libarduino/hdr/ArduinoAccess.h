#ifndef ArduinoAccess_H
#define ArduinoAccess_H

#include "ArduinoAccessTypes.h"
#include "ArduinoAccessInfo.h"
#include "I2CAccess.h"
#include "I2CDevice.h"
#include "GPIOPin.h"

#define arduinoLibraryVersion  "1.0.0"

class ArduinoAccessSigHandler;
class ArduinoPort;

class ArduinoAccess {
friend ArduinoPort;
friend ArduinoAccessSigHandler;
public:
    static Arduino_Result registerPort(ArduinoPort * arduinoPort);
    
    static Arduino_Result resetArduino(ArduinoPort * arduinoPort);
    static Arduino_Result restartArduino(ArduinoPort * arduinoPort);
    
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
    
    static Arduino_Result sendCmd(unsigned char devAddr, unsigned char portN, unsigned char cmd);
    static Arduino_Result get8(unsigned char devAddr, unsigned char portN, unsigned char & val, long int delayMS);
    static Arduino_Result getBuffer(unsigned char devAddr, unsigned char portN, I2C_Data & i2cData, int numBytes, unsigned int delayMS);
    static Arduino_Result getSignalData(unsigned char devAddr, SignalData &sigData);
    
    static Arduino_Result setSignalHandler(ArduinoPort * arduinoPort, int pin);
    
    static Arduino_Result sendNBytes(ArduinoPort * arduinoPort, unsigned char cmd, unsigned long val, int numBytes);
    static Arduino_Result getNBytes(unsigned char devAddr, unsigned char portN, unsigned long & val, int numBytes,long int delayMS);
};

#endif
