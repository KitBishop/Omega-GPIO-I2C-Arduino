#ifndef ArduinoAccessInfo_H
#define ArduinoAccessInfo_H

#include <map>
using namespace std;

//#include "ArduinoPort.h"
#include "GPIOPin.h"

class ArduinoPort;

typedef struct AccessInfo {
    GPIOPin * signalPin;
    map<int, ArduinoPort *> ports;
} AccessInfo;

#endif
