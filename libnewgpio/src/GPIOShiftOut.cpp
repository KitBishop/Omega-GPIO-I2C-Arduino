#include "GPIOShiftOut.h"
#include "GPIOAccess.h"

GPIOShiftOut::GPIOShiftOut(int dataPinN, int clockPinN) {
    dataPin = new GPIOPin(dataPinN);
    clockPin = new GPIOPin(clockPinN);
    
    dataPin->setDirection(GPIO_OUTPUT);
    dataPin->set(0);
    clockPin->setDirection(GPIO_OUTPUT);
    clockPin->set(0);

    clockPeriodNS = 2000;
    bitOrder = GPIO_MSB_FIRST;
}

GPIOShiftOut::~GPIOShiftOut(void) {
    delete dataPin;
    delete clockPin;
}

void GPIOShiftOut::setClockPeriodNS(long int clockPerNS) {
    clockPeriodNS = clockPerNS;
}

long int GPIOShiftOut::getClockPeriodNS() {
    return clockPeriodNS;
}

void GPIOShiftOut::setBitOrder(GPIO_Bit_Order bitOrd) {
    bitOrder = bitOrd;
}

GPIO_Bit_Order GPIOShiftOut::getBitOrder() {
    return bitOrder;
}

GPIO_Result GPIOShiftOut::write(int val) {
    if (dataPin->getPinNumber() == clockPin->getPinNumber()) {
        return GPIO_INVALID_OP;
    }
    return GPIOAccess::shiftOut(dataPin->getPinNumber(), clockPin->getPinNumber(), val, clockPeriodNS, bitOrder);
}

GPIOPin * GPIOShiftOut::getDataPin() {
    return dataPin;
}

GPIOPin * GPIOShiftOut::getClockPin() {
    return clockPin;
}
