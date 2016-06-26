#include "ASAnalogReadOperation.h"
#include "Utilities.h"

using namespace std;

ASAnalogReadOperation::ASAnalogReadOperation()
    : Operation(opASAnalogRead) {
    pinExpr = NULL;
}

bool ASAnalogReadOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No Arduino pin specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    pinExpr = Expression::create(opType, **paramIter, appInfo);

    if (pinExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    return true;
}

string ASAnalogReadOperation::toString() {
    return Operation::toString() + " ArduinoPin:" + pinExpr->getExpressionString();
}

bool ASAnalogReadOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int pinNumber;
    if (!pinExpr->eval(pinNumber)) {
        return false;
    }
    if ((pinNumber >= 0) && (pinNumber <= (A5-A0))) {
        pinNumber = pinNumber + A0;
    }
    if ((pinNumber < A0) || (pinNumber > A5)) {
        appInfo->prtError(opType, "Invalid analog pin number for '" + mapFromOpType(opType) + "':" + pinExpr->getExpressionString() + "->" + to_string(pinNumber));
        return false;
    }
    
    ArduinoSystem * arduinoSys = appInfo->getArduinoSystem();
    
    appInfo->prtReport("Reading from Arduino Analog Pin:" + to_string(pinNumber));
    
    unsigned int val;
    Arduino_Result ares = arduinoSys->analogRead(pinNumber, val);
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to read analog pin value (" + arduinoResultStr(ares) + ")");
    } else {
        appInfo->prtOutput(val);
        appInfo->result = val;
    }
    
    return isok;
}

string ASAnalogReadOperation::help() {
    stringstream hStr;
    hStr << "asanalogread <arduinopin-expr>";
    hStr << "\n\tReads, displays and returns analog value of given Arduino System";
    hStr << "\n\tpin";
    hStr << "\n\tOn the Arduino, uses the function 'analogRead(pin)'";
    hStr << "\n\t<arduinopin-expr> is an expression that evaluates to";
    hStr << "\n\tpin number to use.";
    hStr << "\n\tNOTE: Arduino normally refers to analog pins as A0 to A5 which";
    hStr << "\n\tcorrespond to actual pin numbers 14 to 19.";
    hStr << "\n\tFor convenience, this operation will convert pin numbers 0 to 5";
    hStr << "\n\tto use 14 to 19";
    hStr << "\n\tAny pin number other than 0 to 5 or 14 to 19 is invalid.";
    
    return hStr.str();
}
