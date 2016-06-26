#include "ASShiftOutOperation.h"
#include "Utilities.h"

using namespace std;

ASShiftOutOperation::ASShiftOutOperation()
    : Operation(opASShiftOut) {
    dataPinExpr = NULL;
    clockPinExpr = NULL;
    bitOrder = LSBFIRST;
    valueExpr = NULL;
}

bool ASShiftOutOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
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

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No value specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    valueExpr = Expression::create(opType, **paramIter, appInfo);

    if (valueExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    return true;
}

string ASShiftOutOperation::toString() {
    string orderStr;
    if (bitOrder == LSBFIRST) {
        orderStr = "lsb";
    } else {
        orderStr = "msb";
    }
    return Operation::toString() + " ArduinoDataPin:" + dataPinExpr->getExpressionString()
            + " ArduinoClockPin:" + clockPinExpr->getExpressionString()
            + " BitOrder:" + orderStr 
            + " Value:" + valueExpr->getExpressionString();
}

bool ASShiftOutOperation::execute(AppInfo * appInfo) {
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
    long int val;
    if (!valueExpr->eval(val)) {
        return false;
    }
    val = val & 0xff;
    
    ArduinoSystem * arduinoSys = appInfo->getArduinoSystem();
    
    if (appInfo->setdir) {
        appInfo->prtReport("Setting Arduino pin:" + to_string(dataPinNumber) + " to output");
        arduinoSys->pinMode(dataPinNumber, OUTPUT);
        appInfo->prtReport("Setting Arduino pin:" + to_string(clockPinNumber) + " to output");
        arduinoSys->pinMode(clockPinNumber, OUTPUT);
    }
    
    string orderStr;
    if (bitOrder == LSBFIRST) {
        orderStr = "lsb";
    } else {
        orderStr = "msb";
    }
    appInfo->prtReport("Performing shift out to Arduino. DataPin:" + to_string(dataPinNumber)
        + " ClockPin:" + to_string(clockPinNumber)
        + " BitOrder:" + orderStr
        + " Value:" + to_string(val));
    
    Arduino_Result ares = arduinoSys->shiftOut((unsigned char)dataPinNumber, (unsigned char)clockPinNumber, bitOrder, val);
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to perform shift out (" + arduinoResultStr(ares) + ")");
    }
    
    return isok;
}

string ASShiftOutOperation::help() {
    stringstream hStr;
    hStr << "asshiftout <arduinodatapin-expr> <arduinoclockpin-expr> <bitorder>";
    hStr << "\n\t    <value-expr>";
    hStr << "\n\tPerforms shift out operation using given pins on Arduino System";
    hStr << "\n\tOn the Arduino, uses the function";
    hStr << "\n\t'shiftOut(dataPin, clockPin, bitOrder, value)'";
    hStr << "\n\t<arduinodatapin-expr> is an expression that evaluates to";
    hStr << "\n\tthe data pin number to use.";
    hStr << "\n\t<arduinoclockpin-expr> is an expression that evaluates to";
    hStr << "\n\tthe clock pin number to use.";
    hStr << "\n\t<bitorder> specifies the order in which the data bits are";
    hStr << "\n\ttransferred.  Must be one of:";
    hStr << "\n\t  - lsb = least significant bit first";
    hStr << "\n\t  - msb = most significant bit first";
    hStr << "\n\t<value-expr> is an expression that evaluates to";
    hStr << "\n\tthe value to be sent.  Only the least significant 8 bits";
    hStr << "\n\tof the value are used (i.e. 0 to 255).";
    
    return hStr.str();
}
