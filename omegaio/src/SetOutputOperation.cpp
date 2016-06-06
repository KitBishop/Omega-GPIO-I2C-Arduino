#include "SetOutputOperation.h"
#include "Utilities.h"
#include "GPIOAccess.h"
#include "GPIOPin.h"

using namespace std;

SetOutputOperation::SetOutputOperation()
    : PinOperation() {
    opType = opSetOutput;
    pinAllAllowed = true;
}

bool SetOutputOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    PinOperation::execute(appInfo);
            
    long int pinNumber;
    if (!pinExpr->getPinNumber(pinNumber)) {
        return false;
    }
    int firstPin = pinNumber;
    int lastPin = pinNumber;
    bool isok = true;
    if (pinNumber == -1) {
        firstPin = 0;
        lastPin = 30;
    } 
    for (int pinN = firstPin; (pinN <= lastPin) && isok; pinN++) {
        if (GPIOAccess::isPinUsable(pinN)) {
            GPIOPin * pin = new GPIOPin(pinN);
            appInfo->prtReport("Setting pin:" + to_string(pinN) + " to output");
            GPIO_Result r = pin->setDirection(GPIO_OUTPUT);
            bool isok = r == GPIO_OK;
            if (!isok) {
                appInfo->prtError(opType, "Failed to set pin to output (" + gpioResultStr(r) + ")");
            }
            delete pin;
        }
    }
    return isok;
}

string SetOutputOperation::help() {
    stringstream hStr;
    hStr << "set-output <pin-expr>";
    hStr << "\n\tSets given or all pins to be output pins";
    hStr << "\n\t<pin-expr> is either the value: all";
    hStr << "\n\tor an expression that evaluates to one of:";
    hStr << "\n\t\t0,1,6,7,8,12,13,14,15,16,17,18,19,23,26";
    
    return hStr.str();
}
