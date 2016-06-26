#include "ASShiftInOperation.h"
#include "Utilities.h"

using namespace std;

ASShiftInOperation::ASShiftInOperation()
    : Operation(opASShiftIn) {
    dataPinExpr = NULL;
    clockPinExpr = NULL;
    bitOrder = LSBFIRST;
}

bool ASShiftInOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No Arduino data pin specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    dataPinExpr = Expression::create(opType, **paramIter, appInfo);

    if (dataPinExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No Arduino clock pin specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    clockPinExpr = Expression::create(opType, **paramIter, appInfo);

    if (clockPinExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No bit order specified for: '" + mapFromOpType(opType) + "'");
        return false;
    }

    if ((**paramIter).compare("lsb") == 0) {
        bitOrder = LSBFIRST;
    } else if ((**paramIter).compare("msb") == 0) {
        bitOrder = MSBFIRST;
    } else {
        appInfo->prtError(opType, "Invalid bit order for '" + mapFromOpType(opType) + "':" + **paramIter);
        return false;
    }
    
    (*paramIter)++;

    return true;
}

string ASShiftInOperation::toString() {
    string orderStr;
    if (bitOrder == LSBFIRST) {
        orderStr = "lsb";
    } else {
        orderStr = "msb";
    }
    return Operation::toString() + " ArduinoDataPin:" + dataPinExpr->getExpressionString()
            + " ArduinoClockPin:" + clockPinExpr->getExpressionString()
            + " BitOrder:" + orderStr;
}

bool ASShiftInOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int dataPinNumber;
    if (!dataPinExpr->eval(dataPinNumber)) {
        return false;
    }
    long int clockPinNumber;
    if (!clockPinExpr->eval(clockPinNumber)) {
        return false;
    }
    
    ArduinoSystem * arduinoSys = appInfo->getArduinoSystem();
    
    if (appInfo->setdir) {
        appInfo->prtReport("Setting Arduino pin:" + to_string(dataPinNumber) + " to input");
        arduinoSys->pinMode(dataPinNumber, INPUT);
        appInfo->prtReport("Setting Arduino pin:" + to_string(clockPinNumber) + " to output");
        arduinoSys->pinMode(clockPinNumber, OUTPUT);
    }
    
    string orderStr;
    if (bitOrder == LSBFIRST) {
        orderStr = "lsb";
    } else {
        orderStr = "msb";
    }
    appInfo->prtReport("Performing shift in from Arduino. DataPin:" + to_string(dataPinNumber)
        + " ClockPin:" + to_string(clockPinNumber)
        + " BitOrder:" + orderStr);
    
    unsigned char val;
    Arduino_Result ares = arduinoSys->shiftIn((unsigned char)dataPinNumber, (unsigned char)clockPinNumber, bitOrder, val);
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to perform shift in (" + arduinoResultStr(ares) + ")");
    } else {
        appInfo->prtOutput(val);
        appInfo->result = val;
    }
    
    return isok;
}

string ASShiftInOperation::help() {
    stringstream hStr;
    hStr << "asshiftin <arduinodatapin-expr> <arduinoclockpin-expr> <bitorder>";
    hStr << "\n\tReads, displays and returns result of performing shift in";
    hStr << "\n\toperation using given pins on Arduino System";
    hStr << "\n\tOn the Arduino, uses the function";
    hStr << "\n\t'shiftIn(dataPin, clockPin, bitOrder)'";
    hStr << "\n\t<arduinodatapin-expr> is an expression that evaluates to";
    hStr << "\n\tthe data pin number to use.";
    hStr << "\n\t<arduinoclockpin-expr> is an expression that evaluates to";
    hStr << "\n\tthe clock pin number to use.";
    hStr << "\n\t<bitorder> specifies the order in which the data bits are";
    hStr << "\n\ttransferred.  Must be one of:";
    hStr << "\n\t  - lsb = least significant bit first";
    hStr << "\n\t  - msb = most significant bit first";
    
    return hStr.str();
}
