#include "ReadOperation.h"
#include "Utilities.h"
#include "GPIOPin.h"

using namespace std;

ReadOperation::ReadOperation()
    : PinOperation() {
    opType = opRead;
    pinAllAllowed = false;
}


bool ReadOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    PinOperation::execute(appInfo);

    long int pinNumber;
    if (!pinExpr->getPinNumber(pinNumber)) {
        return false;
    }

    GPIOPin * pin = new GPIOPin(pinNumber);
    if (appInfo->setdir) {
        appInfo->prtReport("Setting pin:" + to_string(pinNumber) + " to input");
        pin->setDirection(GPIO_INPUT);
    }
    appInfo->prtReport("Reading from pin:" + to_string(pinNumber));
    int v;
    GPIO_Result r = pin->get(v);
    bool res = r == GPIO_OK;
    if (!res) {
        appInfo->prtError(opType, "Failed to read from the pin (" + gpioResultStr(r) + ")");
    } else {
        appInfo->prtOutput(v);
        appInfo->result = v;
    }
    delete pin;
    return res;
}

string ReadOperation::help() {
    stringstream hStr;
    hStr << "read <pin-expr>";
    hStr << "\n\tReads, displays and returns value of given pin";
    hStr << "\n\t<pin-expr> is an expression that must evaluate to one of:";
    hStr << "\n\t\t0,1,6,7,8,12,13,14,15,16,17,18,19,23,26";
    
    return hStr.str();
}
