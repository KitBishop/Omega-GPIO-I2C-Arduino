#include "ASDigitalWriteOperation.h"
#include "Utilities.h"

using namespace std;

ASDigitalWriteOperation::ASDigitalWriteOperation()
    : Operation(opASDigitalWrite) {
    pinExpr = NULL;
    valExpr = NULL;
}

bool ASDigitalWriteOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No Arduino pin specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    pinExpr = Expression::create(opType, **paramIter, appInfo);

    if (pinExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No value specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    valExpr = Expression::create(opType, **paramIter, appInfo);

    if (valExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    return true;
}

string ASDigitalWriteOperation::toString() {
    return Operation::toString() + " ArduinoPin:" + pinExpr->getExpressionString() 
            + " Value:" + valExpr->getExpressionString();
}

bool ASDigitalWriteOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int pinNumber;
    if (!pinExpr->eval(pinNumber)) {
        return false;
    }
    long int val;
    if (!valExpr->eval(val)) {
        return false;
    }
    ArduinoPinVal pval;
    if (val == 0) {
        pval = LOW;
    } else {
        pval = HIGH;
    }
    
    ArduinoSystem * arduinoSys = appInfo->getArduinoSystem();
    
    if (appInfo->setdir) {
        appInfo->prtReport("Setting Arduino pin:" + to_string(pinNumber) + " to output");
        arduinoSys->pinMode(pinNumber, OUTPUT);
    }

    string valStr;
    if (pval == LOW) {
        valStr = "LOW";
    } else {
        valStr = "HIGH";
    }
    
    appInfo->prtReport("Writing to Arduino Pin:" + to_string(pinNumber) + " Value:" + valStr);
    
    Arduino_Result ares = arduinoSys->digitalWrite(pinNumber, pval);
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to set pin value (" + arduinoResultStr(ares) + ")");
    }
    
    return isok;
}

string ASDigitalWriteOperation::help() {
    stringstream hStr;
    hStr << "asdigitalwrite <arduinopin-expr> <val-expr>";
    hStr << "\n\tSets given Arduino System pin to the given value";
    hStr << "\n\tOn the Arduino, uses the function 'digitalWrite(pin, value)'";
    hStr << "\n\t<arduinopin-expr> is an expression that evaluates to";
    hStr << "\n\tpin number to use.";
    hStr << "\n\t<val-expr> is an expression that evaluates to give the";
    hStr << "\n\tvalue to set the pin to:";
    hStr << "\n\t  == 0 - sets pin to LOW";
    hStr << "\n\t  != 0 - sets pin to HIGH";
    
    return hStr.str();
}
