#include "RGBLED.h"

#define defRedPin 17
#define defGreenPin 16
#define defBluePin 15

RGBLED::RGBLED()
{
    buildRGBLED(defRedPin, defGreenPin, defBluePin);
}

RGBLED::RGBLED(int redPinN, int greenPinN, int bluePinN) {
    buildRGBLED(redPinN, greenPinN, bluePinN);
}

void RGBLED::buildRGBLED(int redPinN, int greenPinN, int bluePinN) {
    redPin = new GPIOPin(redPinN);
    greenPin = new GPIOPin(greenPinN);
    bluePin = new GPIOPin(bluePinN);

    redPin->setDirection(GPIO_OUTPUT);
    greenPin->setDirection(GPIO_OUTPUT);
    bluePin->setDirection(GPIO_OUTPUT);

    activeLow = true;

    redVal = 0;
    greenVal = 0;
    blueVal = 0;
    setColor(0, 0, 0);
}
RGBLED::~RGBLED(void) {
    delete redPin;
    delete greenPin;
    delete bluePin;
}

void RGBLED::setColor(int rval, int gval, int bval) {
    setRed(rval);
    setGreen(gval);
    setBlue(bval);
}

void RGBLED::setPinVal(GPIOPin * pin, int value) {
    int v = value;
    if (activeLow) {
	v = 100 - v;
    }
    if (v == 0) {
        pin->stopPWM();
        pin->set(0);
    } else if (v == 100) {
        pin->stopPWM();
        pin->set(1);
    } else {
        pin->setPWM(200, v);
    }
}

void RGBLED::setRed(int rval) {
    redVal = rval;
    setPinVal(redPin, rval);
}

void RGBLED::setGreen(int gval) {
    greenVal = gval;
    setPinVal(greenPin, gval);
}

void RGBLED::setBlue(int bval) {
    blueVal = bval;
    setPinVal(bluePin, bval);
}

int RGBLED::getRed() {
    return redVal;
}

int RGBLED::getGreen() {
    return greenVal;
}

int RGBLED::getBlue() {
    return blueVal;
}

GPIOPin * RGBLED::getRedPin() {
    return redPin;
}

GPIOPin * RGBLED::getGreenPin() {
    return greenPin;
}

GPIOPin * RGBLED::getBluePin() {
    return bluePin;
}

void RGBLED::setActiveLow(bool actLow) {
    bool changed = activeLow != actLow;
    activeLow = actLow;
    if (changed) {
        setColor(redVal, greenVal, blueVal);
    }
}

bool RGBLED::isActiveLow() {
    return activeLow;
}

void RGBLED::on() {
    setColor(redVal, greenVal, blueVal);
}

void RGBLED::off() {
    setPinVal(redPin, 0);
    setPinVal(greenPin, 0);
    setPinVal(bluePin, 0);
}
