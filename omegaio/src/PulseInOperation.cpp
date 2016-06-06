#include "PulseInOperation.h"
#include "Utilities.h"
#include "GPIOAccess.h"
#include "GPIOPin.h"

using namespace std;

PulseInOperation::PulseInOperation()
    : PinOperation() {
    opType = opPulseIn;
    pinAllAllowed = false;
    timeOutExpr = NULL;
    pulseLevelExpr = NULL;
}

bool PulseInOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (PinOperation::build(appInfo, paramList, paramIter)) {
        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No pulse level specified for '" + mapFromOpType(opType) + "'");
            return false;
        }

        pulseLevelExpr = Expression::create(opType, **paramIter, appInfo);

        if (pulseLevelExpr == NULL) {
            return false;
        }

        (*paramIter)++;
    
        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No timeout specified for '" + mapFromOpType(opType) + "'");
            return false;
        }

        timeOutExpr = Expression::create(opType, **paramIter, appInfo);

        if (timeOutExpr == NULL) {
            return false;
        }

        (*paramIter)++;

    } else {
        return false;
    }
    
    return true;
}

string PulseInOperation::toString() {
    return PinOperation::toString() + " Level:" + pulseLevelExpr->getExpressionString() + " TimeOut:" + timeOutExpr->getExpressionString();
}

bool PulseInOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    PinOperation::execute(appInfo);

    long int pinNumber;
    if (!pinExpr->getPinNumber(pinNumber)) {
        return false;
    }
    long int pulseLevel;
    if (!pulseLevelExpr->eval(pulseLevel)) {
        return false;
    }
    if (pulseLevel != 0) {
        pulseLevel = 1;
    }
    long int timeOutUS;
    if (!timeOutExpr->eval(timeOutUS)) {
        return false;
    }
    if (timeOutUS < 0) {
        appInfo->prtError(opType, "Invalid time out for '" + mapFromOpType(opType) + "':" + timeOutExpr->getExpressionString() + "->" + to_string(timeOutUS));
        return false;
    }
    
    GPIOPin * pin = new GPIOPin(pinNumber);
    if (appInfo->setdir) {
        appInfo->prtReport("Setting pin:" + to_string(pinNumber) + " to input");
        pin->setDirection(GPIO_INPUT);
    }
    appInfo->prtReport("Performing pulse input on pin:" + to_string(pinNumber) + " Level:" + to_string(pulseLevel) + " TimeOut:" + to_string(timeOutUS));
    long int pulseInLen;
    GPIO_Result r = pin->pulseIn(pulseLevel, timeOutUS, pulseInLen);
    bool isok = r == GPIO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to perform pulse in (" + gpioResultStr(r) + ")");
    } else {
        appInfo->prtOutput(pulseInLen);
        appInfo->result = pulseInLen;
    }
    delete pin;

    return isok;
}

string PulseInOperation::help() {
    stringstream hStr;
    hStr << "pulsein <pin-expr> <level-expr> <timeout-expr>";
    hStr << "\n\tWaits for and displays and returns duration of pulse input on";
    hStr << "\n\tgiven pin";

    hStr << "\n\t<pin-expr> is an expression that must evaluate to one of:";
    hStr << "\n\t\t0,1,6,7,8,12,13,14,15,16,17,18,19,23,26";

    hStr << "\n\t<level-expr> is an expression that evaluates to the pulse level.";
    hStr << "\n\tA nonzero value represents a level of 1";
    hStr << "\n\t<timeout-expr> is an expression that evaluates to the time to";
    hStr << "\n\twait for the pulse and its completion in micro-seconds.";
    hStr << "\n\tMust be >=0  If 0, timeout is indefinite";
    
    return hStr.str();
}
