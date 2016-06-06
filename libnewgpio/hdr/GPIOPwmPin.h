#ifndef GPIOPwmPin_h
#define GPIOPwmPin_h

#include <pthread.h>
#include "GPIOTypes.h"

#define PWM_DEFAULT_FREQ 200
#define PWM_DEFAULT_DUTY 50

class GPIOPwmPin {
public:
    GPIOPwmPin(int pinNum);
    ~GPIOPwmPin(void);

    GPIO_Result setPWM(long int freq, int duty, bool isTone = false, int durationMs = 0);
    GPIO_Result startPWM(bool isTone = false, int durationMs = 0);
    GPIO_Result stopPWM();
    
    long int getPWMFreq();
    int getPWMDuty();
    int getPWMDuration();
    bool isTone();
    bool isPWMRunning();
    int getPinNumber();
    
private:
    static void * pwmThreadRunner(void * pvPin);

    void * runPWM();

    int pinNumber;
    pthread_t pwmThread;
    long int pwmFreq;
    int pwmDuty;
    long long periodHighNS;
    long long periodLowNS;
    bool pwmRunning;
    int pwmDuration;
    bool pwmIsTone;
};

#endif
