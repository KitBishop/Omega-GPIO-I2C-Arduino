#include "PulseOutOperation.h"
#include "Utilities.h"
#include "GPIOAccess.h"
#include "GPIOPin.h"

using namespace std;

PulseOutOperation::PulseOutOperation()
    : PinOperation() {
    opType = opPulseOut;
    pinAllAllowed = false;
    pulseLenExpr = NULL;
    pulseLevelExpr = NULL;
}

bool PulseOutOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (PinOperation::build(appInfo, paramList, paramIter)) {
        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No pulse length specified for '" + mapFromOpType(opType) + "'");
            return false;
        }

        pulseLenExpr = Expression::create(opType, **paramIter, appInfo);

        if (pulseLenExpr == NULL) {
            return false;
        }

        (*paramIter)++;

        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No pulse level specified for '" + mapFromOpType(opType) + "'");
            return false;
        }

        pulseLevelExpr = Expression::create(opType, **paramIter, appInfo);

        if (pulseLevelExpr == NULL) {
            return false;
        }

        (*paramIter)++;
        
    } else {
        return false;
    }
    
    (*paramIter)++;
    return true;
}

string PulseOutOperation::toString() {
    return PinOperation::toString() + " Length:" + pulseLenExpr->getExpressionString() + " Level:" + pulseLevelExpr->getExpressionString();
}

bool PulseOutOperation::execute(AppInfo * appInfo) {
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
    long int pulseLenUS;
    if (!pulseLenExpr->eval(pulseLenUS)) {
        return false;
    }
    if (pulseLenUS <= 0) {
        appInfo->prtError(opType, "Invalid pulse length for '" + mapFromOpType(opType) + "':" + pulseLenExpr->getExpressionString() + "->" + to_string(pulseLenUS));
        return false;
    }

    GPIOPin * pin = new GPIOPin(pinNumber);
    if (appInfo->setdir) {
        appInfo->prtReport("Setting pin:" + to_string(pinNumber) + " to output");
        pin->setDirection(GPIO_OUTPUT);
    }
    appInfo->prtReport("Performing pulse output on pin:" + to_string(pinNumber) + " Length:" + to_string(pulseLenUS) + " Level:" + to_string(pulseLevel));
    GPIO_Result r = pin->pulseOut(pulseLenUS, pulseLevel);
    bool isok = r == GPIO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to perform pulse out (" + gpioResultStr(r) + ")");
    }
    delete pin;

    return isok;
}

string PulseOutOperation::help() {
    stringstream hStr;
    hStr << "pulseout <pin-expr> <duration-expr> <level-expr>";
    hStr << "\n\tOutputs a single pulse to given pin with supplied data";
    hStr << "\n\t<pin-expr> is an expression that must evaluate to one of:";
    hStr << "\n\t\t0,1,6,7,8,12,13,14,15,16,17,18,19,23,26";
    hStr << "\n\t<duration-expr> is an expression that evaluates to the";
    hStr << "\n\tlength of pulse in micro-seconds. Must be >0";
    hStr << "\n\t<level-expr> is an expression that evaluates to the pulse level.";
    hStr << "\n\tA nonzero value represents a level of 1";
    
    return hStr.str();
}
