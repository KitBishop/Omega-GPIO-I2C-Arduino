#ifndef GPIOShiftIn_H
#define GPIOShiftIn_H


#include "GPIOPin.h"

class GPIOShiftIn {
public:
    GPIOShiftIn(int dataPinN, int clockPinN);
    ~GPIOShiftIn(void);

    GPIO_Result read(int &val);

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
