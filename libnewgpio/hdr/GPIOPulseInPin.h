#ifndef GPIOPulseInPin_H
#define GPIOPulseInPin_H

#include <pthread.h>
#include "GPIOTypes.h"

class GPIOPulseInPin {
public:
    GPIOPulseInPin(int pinNum);
    ~GPIOPulseInPin(void);

    GPIO_Result pulseIn(int pulseLevel, long int timeoutUS, long int &val);
    GPIO_Result pulseIn(GPIO_PulseIn_Handler_Func handler, int pulseLevel, long int timeoutUS);
    GPIO_Result pulseIn(GPIO_PulseIn_Handler_Object * handlerObj, int pulseLevel, long int timeoutUS);
    GPIO_Result stopPulseIn();
    bool isPulseInRunning();

    int getPinNumber();
private:
    static void * pulseInPinThreadRunner(void * pvPin);

    void * runPulseIn();

    int pinNumber;
    pthread_t pulseInThread;
    int piPulseLevel;
    long int piTimeoutUS;
    bool pulseInRunning;
    GPIO_PulseIn_Handler_Func handlerFunc;
    GPIO_PulseIn_Handler_Object * handlerObj;
};



#endif
