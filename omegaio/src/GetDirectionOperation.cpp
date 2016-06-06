#include "GetDirectionOperation.h"
#include "Utilities.h"
#include "GPIOAccess.h"
#include "GPIOPin.h"

using namespace std;

GetDirectionOperation::GetDirectionOperation()
: PinOperation() {
    opType = opGetDirection;
    pinAllAllowed = false;
}

bool GetDirectionOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int pinNumber;
    if (!pinExpr->getPinNumber(pinNumber)) {
        return false;
    }

    GPIOPin * pin = new GPIOPin(pinNumber);
    appInfo->prtReport("Getting pin direction:" + to_string(pinNumber));
    GPIO_Direction dir;
    GPIO_Result r = pin->getDirection(dir);
    bool isok = r == GPIO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to get pin direction (" + gpioResultStr(r) + ")");
    } else {
        appInfo->prtOutput(dir);
//        appInfo->prtOutput(to_string(dir));
        appInfo->result = dir;
    }
    delete pin;

    return isok;
}

string GetDirectionOperation::help() {
    stringstream hStr;
    hStr << "get-direction <pin-expr>";
    hStr << "\n\tReads, displays and returns the current direction";
    hStr << "\n\tof given pin";
    hStr << "\n\t<pin-expr> is an expression that must evaluate to one of:";
    hStr << "\n\t\t0,1,6,7,8,12,13,14,15,16,17,18,19,23,26";
    
    return hStr.str();
}
