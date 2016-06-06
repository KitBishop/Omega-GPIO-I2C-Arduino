#include <unistd.h>
#include <stdio.h>

#include "GPIOPulseInPin.h"
#include "GPIOAccess.h"

GPIOPulseInPin::GPIOPulseInPin(int pinNum)
{
    pinNumber = pinNum;
    pulseInRunning = false;

    piPulseLevel = 1;
    piTimeoutUS = 0;

    handlerFunc = NULL;
    handlerObj = NULL;
}

GPIOPulseInPin::~GPIOPulseInPin(void)
{
    stopPulseIn();
}

GPIO_Result GPIOPulseInPin::pulseIn(int pulseLevel, long int timeoutUS, long int &val) {
    val = -1;
    
    GPIO_Direction dir;
    GPIO_Result res = GPIOAccess::getDirection(pinNumber, dir);
    if (res != GPIO_OK) {
        return res;
    }
    if (dir != GPIO_INPUT) {
        return GPIO_INVALID_OP;
    }
    int offV;
    int onV;
    long int elapsedUS = 0;
    long int startUS = 0;
    bool timedOut = false;
    if (pulseLevel == 0) {
        offV = 1;
        onV = 0;
    } else {
        offV = 0;
        onV = 1;
    }

    pulseInRunning = true;
    
    // wait until offV
    startUS = timeNowUS();
    while ((GPIOAccess::rawGet(pinNumber) != offV) && pulseInRunning && !timedOut) {
        sleepMicro(1LL);
        if (timeoutUS != 0) {
            elapsedUS = timeNowUS() - startUS;
            if (elapsedUS > timeoutUS) {
                timedOut = true;
            }
        }
    }

    // wait until onV
    startUS = timeNowUS();
    while ((GPIOAccess::rawGet(pinNumber) != onV) && pulseInRunning && !timedOut) {
        sleepMicro(1LL);
        if (timeoutUS != 0) {
            elapsedUS = timeNowUS() - startUS;
            if (elapsedUS > timeoutUS) {
                timedOut = true;
            }
        }
    }

    // wait until offV counting micro secs
    val = 0;
    startUS = timeNowUS();
    while ((GPIOAccess::rawGet(pinNumber) != offV) && pulseInRunning && !timedOut) {
        sleepMicro(1LL);
        val++;
        if (timeoutUS != 0) {
            elapsedUS = timeNowUS() - startUS;
            if (elapsedUS > timeoutUS) {
                timedOut = true;
            }
        }
    }
    if (pulseInRunning && !timedOut) {
        val = timeNowUS() - startUS;
    }

    if (timedOut) {
        res = GPIO_TIME_OUT;
    }
    if (res != GPIO_OK) {
        val = -1L;
    }
    
    if (!pulseInRunning) {
        val = -1;
    }
    
    if (pulseInRunning && (res == GPIO_OK)) {
        if (handlerFunc != NULL) {
            handlerFunc(pinNumber, val);
        } else if (handlerObj != NULL) {
            handlerObj->handlePulseIn(pinNumber, val);
        }
    }
    
    pulseInRunning = false;
    
    return res;
}

GPIO_Result GPIOPulseInPin::pulseIn(GPIO_PulseIn_Handler_Func handler, int pulseLevel, long int timeoutUS) {
    GPIO_Direction dir;
    GPIO_Result res = GPIOAccess::getDirection(pinNumber, dir);
    if (res != GPIO_OK) {
        return res;
    }
    if (dir != GPIO_INPUT) {
        return GPIO_INVALID_OP;
    }
    
    handlerFunc = handler;
    handlerObj = NULL;
    piPulseLevel = pulseLevel;
    piTimeoutUS = timeoutUS;

    if (!pulseInRunning) {
        int pres = pthread_create(&pulseInThread, NULL, &GPIOPulseInPin::pulseInPinThreadRunner, this);
    }
    
    return GPIO_OK;
}

GPIO_Result GPIOPulseInPin::pulseIn(GPIO_PulseIn_Handler_Object * handlerObj, int pulseLevel, long int timeoutUS) {
    GPIO_Direction dir;
    GPIO_Result res = GPIOAccess::getDirection(pinNumber, dir);
    if (res != GPIO_OK) {
        return res;
    }
    if (dir != GPIO_INPUT) {
        return GPIO_INVALID_OP;
    }
    
    handlerFunc = NULL;
    handlerObj = handlerObj;
    piPulseLevel = pulseLevel;
    piTimeoutUS = timeoutUS;

    if (!pulseInRunning) {
        int pres = pthread_create(&pulseInThread, NULL, &GPIOPulseInPin::pulseInPinThreadRunner, this);
    }
    
    return GPIO_OK;
}

void * GPIOPulseInPin::runPulseIn() {
    long int v;
    pulseIn(piPulseLevel, piTimeoutUS, v);

    return NULL;
}

GPIO_Result GPIOPulseInPin::stopPulseIn() {
    GPIO_Direction dir;
    GPIO_Result res = GPIOAccess::getDirection(pinNumber, dir);
    if (res != GPIO_OK) {
        return res;
    }
    
    if ((dir != GPIO_INPUT) || !pulseInRunning) {
        return GPIO_INVALID_OP;
    }
    
    if (pulseInRunning) {
        pulseInRunning = false;
        pthread_join(pulseInThread, NULL);
    }

    return GPIO_OK;
}

void * GPIOPulseInPin::pulseInPinThreadRunner(void * pvPin) {
    GPIOPulseInPin * pPin = (GPIOPulseInPin *)pvPin;
    return pPin->runPulseIn();
}

bool GPIOPulseInPin::isPulseInRunning() {
    return pulseInRunning;
}

int GPIOPulseInPin::getPinNumber() {
    return pinNumber;
}
