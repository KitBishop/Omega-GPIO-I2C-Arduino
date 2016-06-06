#include "FreqOperation.h"
#include "Utilities.h"
#include "GPIOAccess.h"
#include "GPIOPin.h"

using namespace std;

FreqOperation::FreqOperation()
    : PinOperation() {
    opType = opFreq;
    pinAllAllowed = false;
    sampleTimeExpr = NULL;
}

bool FreqOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (PinOperation::build(appInfo, paramList, paramIter)) {
        bool isok = true;

        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No sample time specified for '" + mapFromOpType(opType) + "'");
            return false;
        }

        sampleTimeExpr = Expression::create(opType, **paramIter, appInfo);

        if (sampleTimeExpr == NULL) {
            return false;
        }

        (*paramIter)++;
    } else {
        return false;
    }
    
    return true;
}

string FreqOperation::toString() {
    return PinOperation::toString() + " SampleTimeMS:" + sampleTimeExpr->getExpressionString();
}

bool FreqOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    PinOperation::execute(appInfo);
            
    long int pinNumber;
    if (!pinExpr->getPinNumber(pinNumber)) {
        return false;
    }
    
    long int sampleTimeMS;
    if (!sampleTimeExpr->eval(sampleTimeMS)) {
        return false;
    }
    
    if (sampleTimeMS <= 0) {
        appInfo->prtError(opType, "Invalid sample time for '" + mapFromOpType(opType) + "':" + sampleTimeExpr->getExpressionString() + "->" + to_string(sampleTimeMS));
        return false;
    }
    
    GPIOPin * pin = new GPIOPin(pinNumber);
    if (appInfo->setdir) {
        appInfo->prtReport("Setting pin:" + to_string(pinNumber) + " to input");
        pin->setDirection(GPIO_INPUT);
    }
    appInfo->prtReport("Getting frequency on pin:" + to_string(pinNumber) + " SampleTimeMS:" + to_string(sampleTimeMS));
    long int freq;
    GPIO_Result r = pin->getFrequency(sampleTimeMS, freq);
    bool isok = (r == GPIO_OK);
    delete pin;
    if (!isok) {
        appInfo->prtError(opType, "Failed to perform frequency input (" + gpioResultStr(r) + ")");
    } else {
        appInfo->prtOutput(freq);
        appInfo->result = freq;
    }

    return isok;
}

string FreqOperation::help() {
    stringstream hStr;
    hStr << "frequency <pin-expr> <sampletime-expr>";
    hStr << "\n\tObtains, displays and returns frequency of input on";
    hStr << "\n\tgiven pin";
    hStr << "\n\tOperates by counting the number of pulses during the sample time";
    hStr << "\n\t<pin-expr> is an expression that must evaluate to one of:";
    hStr << "\n\t\t0,1,6,7,8,12,13,14,15,16,17,18,19,23,26";
    hStr << "\n\t<sampletime-expr> is an expression that evaluates to the time";
    hStr << "\n\tin milliseconds over which the frequency is taken. Must be >0";
    
    return hStr.str();
}
