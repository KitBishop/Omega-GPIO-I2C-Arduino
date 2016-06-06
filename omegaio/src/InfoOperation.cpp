#include "InfoOperation.h"
#include "GPIOAccess.h"
#include "GPIOPin.h"
#include "ForkAccess.h"
#include "Utilities.h"

using namespace std;

InfoOperation::InfoOperation()
    : PinOperation() {
    opType = opInfo;
    pinAllAllowed = true;
}

bool InfoOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int pinNumber;
    if (!pinExpr->getPinNumber(pinNumber)) {
        return false;
    }
    int firstPin = pinNumber;
    int lastPin = pinNumber;
    if (pinNumber == -1) {
        firstPin = 0;
        lastPin = 30;
    }
    int pinN;
    
    for (pinN = firstPin; pinN <= lastPin; pinN++) {
        if (GPIOAccess::isPinUsable(pinN)) {
            string pinStr = "Pin:" + to_string(pinN) + " Direction:";

            GPIOPin *pin = new GPIOPin(pinN);
            GPIO_Direction dir;
            pin->getDirection(dir);
            if (dir == GPIO_OUTPUT) {
                pinStr = pinStr + "Output";
            } else {
                pinStr = pinStr + "Input";
            }

            int val;
            pin->get(val);
            pinStr = pinStr + " Value:" + to_string(val);

            string pinInf;
            if (ForkAccess::getInfo(pinN, pinInf)) {
                pinStr = pinStr + "\n\t" + pinInf;
            }
            
            delete pin;
            
            bool aRep = appInfo->report;
            appInfo->report = true;
            appInfo->prtReport(pinStr);
            appInfo->report = aRep;
        }
    }

    return true;
}

string InfoOperation::help() {
    stringstream hStr;
    hStr << "info <pin-expr>";
    hStr << "\n\tDisplays information on given or all pins";
    hStr << "\n\t<pin-expr> is either the value: all";
    hStr << "\n\tor an expression that evaluates to one of:";
    hStr << "\n\t\t0,1,6,7,8,12,13,14,15,16,17,18,19,23,26";
    
    return hStr.str();
}
