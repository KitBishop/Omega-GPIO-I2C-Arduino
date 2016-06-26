#include "ASAnalogWriteOperation.h"
#include "Utilities.h"

using namespace std;

ASAnalogWriteOperation::ASAnalogWriteOperation()
    : Operation(opASAnalogWrite) {
    pinExpr = NULL;
    valExpr = NULL;
}

bool ASAnalogWriteOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
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

string ASAnalogWriteOperation::toString() {
    return Operation::toString() + " ArduinoPin:" + pinExpr->getExpressionString() 
            + " Value:" + valExpr->getExpressionString();
}

bool ASAnalogWriteOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int pinNumber;
    if (!pinExpr->eval(pinNumber)) {
        return false;
    }
    if ((pinNumber != 3)
            && (pinNumber != 5)
            && (pinNumber != 6)
            && (pinNumber != 9)
            && (pinNumber != 10)
            && (pinNumber != 11)){
        appInfo->prtError(opType, "Invalid pin number for '" + mapFromOpType(opType) + "':" + pinExpr->getExpressionString() + "->" + to_string(pinNumber));
        return false;
    }
        
    long int val;
    if (!valExpr->eval(val)) {
        return false;
    }
    val = val & 0xff;
    
    ArduinoSystem * arduinoSys = appInfo->getArduinoSystem();
    
    if (appInfo->setdir) {
        appInfo->prtReport("Setting Arduino pin:" + to_string(pinNumber) + " to output");
        arduinoSys->pinMode(pinNumber, OUTPUT);
    }

    appInfo->prtReport("Writing to analog value to Arduino Pin:" + to_string(pinNumber) + " Value:" + to_string(val));
    
    Arduino_Result ares = arduinoSys->analogWrite(pinNumber, val);
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to set pin analog value (" + arduinoResultStr(ares) + ")");
    }
    
    return isok;
}

string ASAnalogWriteOperation::help() {
    stringstream hStr;
    hStr << "asanalogwrite <arduinopin-expr> <val-expr>";
    hStr << "\n\tWrites given analog value to Arduino System pin";
    hStr << "\n\tOn the Arduino, uses the function 'analogWrite(pin, value)'";
    hStr << "\n\t<arduinopin-expr> is an expression that evaluates to";
    hStr << "\n\tpin number to use.";
    hStr << "\n\tNOTE: The Arduino can only perform analog write functionality";
    hStr << "\n\ton pins 3, 5, 6, 9, 10, and 11 any other pin values are invalid.";
    hStr << "\n\t<val-expr> is an expression that evaluates to give the";
    hStr << "\n\tvalue to set the pin to.  Only the least significant 8 bits";
    hStr << "\n\tof the value are used (i.e. 0 to 255).";
    hStr << "\n\tNOTE: The Arduino does NOT perform true analog output.";
    hStr << "\n\tRather, it produces PWM output on the pin used with a value of";
    hStr << "\n\t0 producing 0% duty cycle and 255 producing 100% duty cycle.";
    
    return hStr.str();
}
