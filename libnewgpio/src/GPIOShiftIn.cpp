#include "GPIOShiftIn.h"
#include "GPIOAccess.h"

GPIOShiftIn::GPIOShiftIn(int dataPinN, int clockPinN) {
    dataPin = new GPIOPin(dataPinN);
    clockPin = new GPIOPin(clockPinN);
    
    dataPin->setDirection(GPIO_INPUT);
    clockPin->setDirection(GPIO_OUTPUT);
    clockPin->set(0);

    clockPeriodNS = 2000;
    bitOrder = GPIO_MSB_FIRST;
}

GPIOShiftIn::~GPIOShiftIn(void) {
    delete dataPin;
    delete clockPin;
}

void GPIOShiftIn::setClockPeriodNS(long int clockPerNS) {
    clockPeriodNS = clockPerNS;
}

long int GPIOShiftIn::getClockPeriodNS() {
    return clockPeriodNS;
}

void GPIOShiftIn::setBitOrder(GPIO_Bit_Order bitOrd) {
    bitOrder = bitOrd;
}

GPIO_Bit_Order GPIOShiftIn::getBitOrder() {
    return bitOrder;
}

GPIO_Result GPIOShiftIn::read(int &val) {
    return GPIOAccess::shiftIn(dataPin->getPinNumber(), clockPin->getPinNumber(), clockPeriodNS, bitOrder, val);
}

GPIOPin * GPIOShiftIn::getDataPin() {
    return dataPin;
}

GPIOPin * GPIOShiftIn::getClockPin() {
    return clockPin;
}
