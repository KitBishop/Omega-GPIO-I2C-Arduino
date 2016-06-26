#include "ASPinModeOperation.h"
#include "Utilities.h"

using namespace std;

ASPinModeOperation::ASPinModeOperation()
    : Operation(opASPinMode) {
    pinExpr = NULL;
    pinMode = INPUT;
}

bool ASPinModeOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
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
        appInfo->prtError(opType, "No pin mode specified for Arduino pin: '" + mapFromOpType(opType) + "'");
        return false;
    }

    if ((**paramIter).compare("input") == 0) {
        pinMode = INPUT;
    } else if ((**paramIter).compare("output") == 0) {
        pinMode = OUTPUT;
    } else if ((**paramIter).compare("input_pullup") == 0) {
        pinMode = INPUT_PULLUP;
    } else {
        appInfo->prtError(opType, "Invalid pin mode for '" + mapFromOpType(opType) + "':" + **paramIter);
        return false;
    }
    
    (*paramIter)++;

    return true;
}

string ASPinModeOperation::toString() {
    string str = Operation::toString();
    str = str + " ArduinoPin:" + pinExpr->getExpressionString() + " Mode:";
    if (pinMode == INPUT) {
        str = str + "INPUT";
    } else if (pinMode == OUTPUT) {
        str = str + "OUTPUT";
    } else {
        str = str + "INPUT_PULLUP";
    }
    return str;
}

bool ASPinModeOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int pinNumber;
    if (!pinExpr->eval(pinNumber)) {
        return false;
    }
    
    ArduinoSystem * arduinoSys = appInfo->getArduinoSystem();
    
    string modeStr;
    if (pinMode == INPUT) {
        modeStr = "INPUT";
    } else if (pinMode == OUTPUT) {
        modeStr = "OUTPUT";
    } else {
        modeStr = "INPUT_PULLUP";
    }
    
    appInfo->prtReport("Setting Arduino Pin Mode. Pin:" + to_string(pinNumber) + " Mode:" + modeStr);
    
    Arduino_Result ares = arduinoSys->pinMode(pinNumber, pinMode);
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to set pin mode (" + arduinoResultStr(ares) + ")");
    }
    
    return isok;
}

string ASPinModeOperation::help() {
    stringstream hStr;
    hStr << "aspinmode <arduinopin-expr> <pinmode>";
    hStr << "\n\tSets given Arduino System pin mode to the given value";
    hStr << "\n\t<arduinopin-expr> is an expression that evaluates to";
    hStr << "\n\tpin number to use.";
    hStr << "\n\t<pinmode> is the mode to use for the pin and must be one of:";
    hStr << "\n\t  - input";
    hStr << "\n\t  - input_pullup";
    hStr << "\n\t  - output";
    
    return hStr.str();
}
