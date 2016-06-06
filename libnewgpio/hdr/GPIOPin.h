#ifndef GPIOPin_h
#define GPIOPin_h

#include <pthread.h>
#include "GPIOTypes.h"

class GPIOPin {
public:
    GPIOPin(int pinNum);
    ~GPIOPin(void);

    GPIO_Result setDirection(GPIO_Direction dir);
    GPIO_Result getDirection(GPIO_Direction &dir);

    GPIO_Result set(int value);
    GPIO_Result get(int &value);

    GPIO_Result setIrq(GPIO_Irq_Type type, GPIO_Irq_Handler_Func handler, long int debounceMs =0);
    GPIO_Result setIrq(GPIO_Irq_Type type, GPIO_Irq_Handler_Object * handlerObj, long int debounceMs = 0);
    GPIO_Result resetIrq();
    GPIO_Result enableIrq();
    GPIO_Result disableIrq();
    GPIO_Result enableIrq(bool enable);
    GPIO_Result irqEnabled(bool &enabled);
    GPIO_Irq_Type getIrqType();
    GPIO_Irq_Handler_Func getIrqHandler();
    GPIO_Irq_Handler_Object * getIrqHandlerObj();
    
    GPIO_Result setPWM(long int freq, int duty, int durationMs = 0);
    GPIO_Result startPWM(int durationMs = 0);
    GPIO_Result stopPWM();
    
    GPIO_Result getPWMFreq(long int &freq);
    GPIO_Result getPWMDuty(int &duty);
    GPIO_Result getPWMDuration(int &duration);
    GPIO_Result isPWMRunning(bool &running);

    GPIO_Result setTone(long int freq, int durationMs = 0);
    GPIO_Result startTone(int durationMs = 0);
    GPIO_Result stopTone();
    GPIO_Result getToneFreq(long int &freq);
    GPIO_Result getToneDuration(int &duty);
    GPIO_Result isToneRunning(bool &running);

    GPIO_Result pulseOut(long int pulseLenUS, int pulseLevel = 1);

    GPIO_Result pulseIn(int pulseLevel, long int timeoutUS, long int &value);
    GPIO_Result pulseIn(GPIO_PulseIn_Handler_Func handler, int pulseLevel = 1, long int timeoutUS = 0);
    GPIO_Result pulseIn(GPIO_PulseIn_Handler_Object * handlerObj, int pulseLevel = 1, long int timeoutUS = 0);
    GPIO_Result stopPulseIn();
    GPIO_Result isPulseInRunning(bool &running);
    
    GPIO_Result getFrequency(long int sampleTimeMS, long int &freq);
    
    int getPinNumber();
private:
    int pinNumber;
};

#endif
