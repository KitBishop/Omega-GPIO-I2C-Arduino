#include <unistd.h>
#include <stdio.h>

#include "GPIOPwmPin.h"
#include "GPIOAccess.h"

GPIOPwmPin::GPIOPwmPin(int pinNum)
{
    pinNumber = pinNum;
    pwmRunning = false;
    pwmFreq = PWM_DEFAULT_FREQ;
    pwmDuty = PWM_DEFAULT_DUTY;
}

GPIOPwmPin::~GPIOPwmPin(void)
{
    stopPWM();
}

GPIO_Result GPIOPwmPin::setPWM(long int freq, int duty, bool isTone, int durationMs) {
    if (freq <= 0) {
        return stopPWM();
    }
    
    GPIO_Direction dir;
    GPIO_Result res = GPIOAccess::getDirection(pinNumber, dir);
    if (res != GPIO_OK) {
        return res;
    }
    if ((dir != GPIO_OUTPUT) || ((!isTone) && ((duty < 0) || (duty > 100)))) {
        return GPIO_INVALID_OP;
    }
    pwmFreq = freq;
    if (!isTone) {
        pwmDuty = duty;
    } else {
        pwmDuty = 50;
    }
    pwmDuration = durationMs;
    pwmIsTone = isTone;

    // find the period in nano seconds
    long long periodNS = 1000000000LL / (((long long)pwmFreq));

    // find the low and high periods based on the duty-cycle
    periodHighNS = (periodNS * (long long)pwmDuty) / 100LL;
    periodLowNS = periodNS - periodHighNS; //can also be: period * ((100 - dutyCycle) /100);

    if (pwmRunning) {
        stopPWM();
    }

    pthread_create(&pwmThread, NULL, &GPIOPwmPin::pwmThreadRunner, this);

    return GPIO_OK;
}

GPIO_Result GPIOPwmPin::startPWM(bool isTone, int durationMs) {
    return setPWM(pwmFreq, pwmDuty, isTone, durationMs);
}

void * GPIOPwmPin::runPWM() {
    pwmRunning = true;
    
    bool durationExpired = false;
    long long durationLeftNS = pwmDuration * 1000000LL;

    while (pwmRunning  && !durationExpired) {
        // HIGH part of cycle
        GPIOAccess::rawSet(pinNumber, 1);
        sleepNano(periodHighNS);

        // LOW part of cycle
        GPIOAccess::rawSet(pinNumber, 0);
        sleepNano(periodLowNS);

        if (pwmDuration > 0) {
            durationLeftNS = durationLeftNS - periodHighNS - periodLowNS;
            durationExpired = (durationLeftNS <= 0LL);
        }
    }
    
    if (durationExpired) {
        stopPWM();
    }

    return NULL;
}

GPIO_Result GPIOPwmPin::stopPWM() {
    GPIO_Direction dir;
    GPIO_Result res = GPIOAccess::getDirection(pinNumber, dir);
    if (res != GPIO_OK) {
        return res;
    }
    
    if (dir != GPIO_OUTPUT) {
        return GPIO_INVALID_OP;
    }

    if (pwmRunning) {
        pwmRunning = false;
        pthread_join(pwmThread, NULL);
    }

    return GPIO_OK;
}

void * GPIOPwmPin::pwmThreadRunner(void * pvPin) {
    GPIOPwmPin * pPin = (GPIOPwmPin *)pvPin;
    return pPin->runPWM();
}

long int GPIOPwmPin::getPWMFreq() {
    return pwmFreq;
}

int GPIOPwmPin::getPWMDuty() {
    return pwmDuty;
}

bool GPIOPwmPin::isPWMRunning() {
    return pwmRunning;
}

int GPIOPwmPin::getPinNumber() {
    return pinNumber;
}

int GPIOPwmPin::getPWMDuration() {
    return pwmDuration;
}

bool GPIOPwmPin::isTone() {
    return pwmIsTone;
}
