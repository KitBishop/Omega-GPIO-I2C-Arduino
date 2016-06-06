#include "SetOperation.h"
#include "Utilities.h"
#include "GPIOAccess.h"
#include "GPIOPin.h"

using namespace std;

SetOperation::SetOperation()
    : PinOperation() {
    opType = opSet;
    pinAllAllowed = true;
    valueExpr = NULL;
}

bool SetOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (PinOperation::build(appInfo, paramList, paramIter)) {
        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No value specified for '" + mapFromOpType(opType) + "'");
            return false;
        }
        
        valueExpr = Expression::create(opType, **paramIter, appInfo);

        if (valueExpr == NULL) {
            return false;
        }

        (*paramIter)++;
    } else {
        return false;
    }
    
    return true;
}

string SetOperation::toString() {
    return PinOperation::toString() + " Value:" + valueExpr->getExpressionString();
}

bool SetOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    PinOperation::execute(appInfo);
            
    long int pinNumber;
    if (!pinExpr->getPinNumber(pinNumber)) {
        return false;
    }
    long int value;
    if (!valueExpr->eval(value)) {
        return false;
    }
    if (value != 0) {
        value = 1;
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
            if (appInfo->setdir) {
                appInfo->prtReport("Setting pin:" + to_string(pinN) + " to output");
                pin->setDirection(GPIO_OUTPUT);
            }
            appInfo->prtReport("Setting pin:" + to_string(pinN) + " to value:" + to_string(value));
            GPIO_Result r = pin->set(value);
            bool isok = r == GPIO_OK;
            if (!isok) {
                appInfo->prtError(opType, "Failed to set pin (" + gpioResultStr(r) + ")");
            }
            delete pin;
        }
    }
    return isok;
}

string SetOperation::help() {
    stringstream hStr;
    hStr << "set <pin-expr> <value-expr>";
    hStr << "\n\tSets given pin(s) to the given value";
    hStr << "\n\t<pin-expr> is either the value: all";
    hStr << "\n\tor an expression that evaluates to one of:";
    hStr << "\n\t\t0,1,6,7,8,12,13,14,15,16,17,18,19,23,26";
    hStr << "\n\t<value-expr> is an expression that evaluates to the";
    hStr << "\n\tvalue to output. A value of zero outputs a 0,";
    hStr << "\n\tnon-zero outputs a 1";
    
    return hStr.str();
}
