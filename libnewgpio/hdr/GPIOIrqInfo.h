#ifndef GPIOIrqInf_h
#define GPIOIrqInf_h

#include "GPIOTypes.h"

struct GPIOIrqInf {
    GPIO_Irq_Type type;
    GPIO_Irq_Handler_Func handlerFunc;
    GPIO_Irq_Handler_Object * handlerObj;
    bool enabled;
    long int debounceMs;
    long int lastTimeMs;
    int lastVal;
};

#endif
