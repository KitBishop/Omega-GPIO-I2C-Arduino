#include "ASPulseInOperation.h"
#include "Utilities.h"

using namespace std;

ASPulseInOperation::ASPulseInOperation()
    : Operation(opASPulseIn) {
    pinExpr = NULL;
    levelExpr = NULL;
    timeoutExpr = NULL;
}

bool ASPulseInOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
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
        appInfo->prtError(opType, "No level specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    levelExpr = Expression::create(opType, **paramIter, appInfo);

    if (levelExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No time out specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    timeoutExpr = Expression::create(opType, **paramIter, appInfo);

    if (timeoutExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    return true;
}

string ASPulseInOperation::toString() {
    return Operation::toString() + " ArduinoPin:" + pinExpr->getExpressionString()
            + " Level:" + levelExpr->getExpressionString()
            + " TimeOut:" + timeoutExpr->getExpressionString();
}

bool ASPulseInOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int pinNumber;
    if (!pinExpr->eval(pinNumber)) {
        return false;
    }
    long int level;
    if (!levelExpr->eval(level)) {
        return false;
    }
    ArduinoPinVal plevel;
    if (level == 0) {
        plevel = LOW;
    } else {
        plevel = HIGH;
    }
    long int timeout;
    if (!timeoutExpr->eval(timeout)) {
        return false;
    }
    if (timeout < 0) {
        appInfo->prtError(opType, "Invalid time out for '" + mapFromOpType(opType) + "':" + timeoutExpr->getExpressionString() + "->" + to_string(timeout));
        return false;
    }
    
    ArduinoSystem * arduinoSys = appInfo->getArduinoSystem();
    
    if (appInfo->setdir) {
        appInfo->prtReport("Setting Arduino pin:" + to_string(pinNumber) + " to input");
        arduinoSys->pinMode(pinNumber, INPUT);
    }

    string levelStr;
    if (plevel == LOW) {
        levelStr = "LOW";
    } else {
        levelStr = "HIGH";;
    }
    appInfo->prtReport("Performing pulse in from Arduino Pin:" + to_string(pinNumber)
        + " Level:" + levelStr
        + " TimeOut:" + to_string(timeout));
    
    unsigned long val;
    Arduino_Result ares;

    if (timeout == 0) {
        ares = arduinoSys->pulseIn(pinNumber, plevel, val);
    } else {
        ares = arduinoSys->pulseIn(pinNumber, plevel, timeout, val);
    }
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to perform pulse in (" + arduinoResultStr(ares) + ")");
    } else {
        appInfo->prtOutput(val);
        appInfo->result = val;
    }
    
    return isok;
}

string ASPulseInOperation::help() {
    stringstream hStr;
    hStr << "aspulsein <arduinopin-expr> <level-expr> <timeout-expr>";
    hStr << "\n\tPerforms a pulse in operation on given Arduino System pin";
    hStr << "\n\tand displays and returns the value in microseconds";
    hStr << "\n\tOn the Arduino, uses the function 'pulseIn(pin, level, timeout)'";
    hStr << "\n\t<arduinopin-expr> is an expression that evaluates to";
    hStr << "\n\tpin number to use.";
    hStr << "\n\t<level-expr> is an expression that evaluates to give the";
    hStr << "\n\tvalue of the pulse level to input:";
    hStr << "\n\t  == 0 - for a LOW level pulse";
    hStr << "\n\t  != 0 - for a HIGH level pulse";
    hStr << "\n\t<timeout-expr> is an expression that evaluates to";
    hStr << "\n\tthe time in microseconds to wait for the pulse.";
    hStr << "\n\tMust be >= 0";
    
    return hStr.str();
}
