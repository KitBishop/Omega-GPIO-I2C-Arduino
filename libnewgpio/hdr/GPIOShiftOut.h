#ifndef GPIOShiftOut_H
#define GPIOShiftOut_H


#include "GPIOPin.h"

class GPIOShiftOut {
public:
    GPIOShiftOut(int dataPinN, int clockPinN);
    ~GPIOShiftOut(void);

    GPIO_Result write(int val);

    void setClockPeriodNS(long int clockPerNS);
    long int getClockPeriodNS();
    void setBitOrder(GPIO_Bit_Order bitOrd);
    GPIO_Bit_Order getBitOrder();
    
    GPIOPin * getDataPin();
    GPIOPin * getClockPin();

private:
    GPIOPin * dataPin;
    GPIOPin * clockPin;
    
    long int clockPeriodNS;
    GPIO_Bit_Order bitOrder;
};


#endif
