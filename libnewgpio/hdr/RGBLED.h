#ifndef RGBLED_h
#define RGBLED_h

#include "GPIOPin.h"

class RGBLED {
public:
    RGBLED();
    RGBLED(int redPinN, int greenPinN, int bluePinN);
    ~RGBLED(void);

    void setColor(int rval, int gval, int bval);
    void setRed(int rval);
    void setGreen(int gval);
    void setBlue(int bval);
    int getRed();
    int getGreen();
    int getBlue();
    void on();
    void off();
    
    GPIOPin * getRedPin();
    GPIOPin * getGreenPin();
    GPIOPin * getBluePin();
    
    void setActiveLow(bool actLow);
    bool isActiveLow();

private:
    void buildRGBLED(int redPinN, int greenPinN, int bluePinN);
    void setPinVal(GPIOPin * pin, int value);
    
    int redVal;
    int greenVal;
    int blueVal;
    
    GPIOPin * redPin;
    GPIOPin * greenPin;
    GPIOPin * bluePin;
    
    bool activeLow;
};

#endif
