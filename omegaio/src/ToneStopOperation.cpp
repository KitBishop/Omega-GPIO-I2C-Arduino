#include "ToneStopOperation.h"
#include "Utilities.h"
#include "ForkAccess.h"

using namespace std;

ToneStopOperation::ToneStopOperation()
    : PinOperation() {
    opType = opToneStop;
    pinAllAllowed = false;
}


bool ToneStopOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int pinNumber;
    if (!pinExpr->getPinNumber(pinNumber)) {
        return false;
    }

    appInfo->prtReport("Stopping Tone output on pin:" + to_string(pinNumber));
    
    ForkAccess::stop(pinNumber);

    return true;
}

string ToneStopOperation::help() {
    stringstream hStr;
    hStr << "tonestop <pin-expr>";
    hStr << "\n\tStops any separate process that is currently performing tone";
    hStr << "\n\toutput on given pin";
    hStr << "\n\t<pin-expr> is an expression that must evaluate to one of:";
    hStr << "\n\t\t0,1,6,7,8,12,13,14,15,16,17,18,19,23,26";
    
    return hStr.str();
}
