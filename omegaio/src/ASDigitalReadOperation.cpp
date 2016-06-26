#include "ASDigitalReadOperation.h"
#include "Utilities.h"

using namespace std;

ASDigitalReadOperation::ASDigitalReadOperation()
    : Operation(opASDigitalRead) {
    pinExpr = NULL;
}

bool ASDigitalReadOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
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

string ASDigitalReadOperation::toString() {
    return Operation::toString() + " ArduinoPin:" + pinExpr->getExpressionString();
}

bool ASDigitalReadOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int pinNumber;
    if (!pinExpr->eval(pinNumber)) {
        return false;
    }
    
    ArduinoSystem * arduinoSys = appInfo->getArduinoSystem();
    
    if (appInfo->setdir) {
        appInfo->prtReport("Setting Arduino pin:" + to_string(pinNumber) + " to input");
        arduinoSys->pinMode(pinNumber, INPUT);
    }

    appInfo->prtReport("Reading from Arduino Pin:" + to_string(pinNumber));
    
    ArduinoPinVal pval;
    Arduino_Result ares = arduinoSys->digitalRead(pinNumber, pval);
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to read pin value (" + arduinoResultStr(ares) + ")");
    } else {
        appInfo->prtOutput(pval);
        appInfo->result = pval;
    }
    
    return isok;
}

string ASDigitalReadOperation::help() {
    stringstream hStr;
    hStr << "asdigitalread <arduinopin-expr>";
    hStr << "\n\tReads, displays and returns value of given Arduino System pin";
    hStr << "\n\tOn the Arduino, uses the function 'digitalRead(pin)'";
    hStr << "\n\t<arduinopin-expr> is an expression that evaluates to";
    hStr << "\n\tpin number to use.";
    
    return hStr.str();
}
