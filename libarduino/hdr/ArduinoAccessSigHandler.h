#ifndef ArduinoAccessSigHandler_H
#define ArduinoAccessSigHandler_H

#include "GPIOTypes.h"
#include "ArduinoAccess.h"

class ArduinoAccessSigHandler : public GPIO_Irq_Handler_Object {
public:
    ArduinoAccessSigHandler(unsigned char devAddr);
    virtual void handleIrq(int pinNum, GPIO_Irq_Type type);
private:
    unsigned char arduinoDevAddr;
    AccessInfo * accessInfo;
};

#endif
