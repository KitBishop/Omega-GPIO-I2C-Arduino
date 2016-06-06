#ifndef ArduinoPort_H
#define ArduinoPort_H

#include "ArduinoAccessTypes.h"
#include "ArduinoAccess.h"
#include "I2CTypes.h"
#include "I2CAccess.h"
#include "I2CDevice.h"
#include "GPIOPin.h"

class ArduinoAccess;
class ArduinoAccessSigHandler;

class ArduinoPort {
friend ArduinoAccess;
friend ArduinoAccessSigHandler;
public:
    
    ArduinoPort(unsigned char portN);
    ArduinoPort(unsigned char devAddr, unsigned char portN);
    
    Arduino_Result sendCmd(unsigned char cmd);
    Arduino_Result send8(unsigned char cmd, unsigned char val);
    Arduino_Result send16(unsigned char cmd, unsigned int val);
    Arduino_Result send32(unsigned char cmd, unsigned long val);
    Arduino_Result sendBuffer(unsigned char cmd, I2C_Data i2cData);

    Arduino_Result getStatus();
    Arduino_Result get8(unsigned char & val);
    Arduino_Result get16(unsigned int & val);
    Arduino_Result get32(unsigned long & val);
    Arduino_Result getBuffer(I2C_Data & i2cData, int numBytes);

    void setResponseDelayMS(unsigned long MS);
    unsigned long getResponseDelayMS();

    Arduino_Result setSignalHandler(int pin, Arduino_Sig_Handler_Func handler);
    Arduino_Result setSignalHandler(int pin, Arduino_Sig_Handler_Object * handlerObj);
    
private:
    unsigned char arduinoDevAddr;
    unsigned char arduinoPortNum;
    unsigned long responseDelayMS;
    
    Arduino_Sig_Handler_Func sigHandler;
    Arduino_Sig_Handler_Object * sigHandlerObj;
    
};

#endif
