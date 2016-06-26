#include "ASNoToneOperation.h"
#include "Utilities.h"

using namespace std;

ASNoToneOperation::ASNoToneOperation()
    : Operation(opASNoTone) {
    pinExpr = NULL;
}

bool ASNoToneOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
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

string ASNoToneOperation::toString() {
    return Operation::toString() + " ArduinoPin:" + pinExpr->getExpressionString();
}

bool ASNoToneOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int pinNumber;
    if (!pinExpr->eval(pinNumber)) {
        return false;
    }
        
    ArduinoSystem * arduinoSys = appInfo->getArduinoSystem();
    
    if (appInfo->setdir) {
        appInfo->prtReport("Setting Arduino pin:" + to_string(pinNumber) + " to output");
        arduinoSys->pinMode(pinNumber, OUTPUT);
    }

    appInfo->prtReport("Stopping tone output on Arduino Pin:" + to_string(pinNumber));
    
    Arduino_Result ares = arduinoSys->noTone(pinNumber);
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to stop tone output (" + arduinoResultStr(ares) + ")");
    }
    
    return isok;
}

string ASNoToneOperation::help() {
    stringstream hStr;
    hStr << "asnotone <arduinopin-expr>";
    hStr << "\n\tStops tone output on Arduino System pin";
    hStr << "\n\tOn the Arduino, uses the function 'noTone(pin)'";
    hStr << "\n\t<arduinopin-expr> is an expression that evaluates to";
    hStr << "\n\tpin number to use.";
    
    return hStr.str();
}
