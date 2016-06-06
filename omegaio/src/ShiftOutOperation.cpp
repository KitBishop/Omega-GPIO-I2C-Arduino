#include <unistd.h>
#include <string>

#include "ShiftOutOperation.h"
#include "Utilities.h"
#include "GPIOShiftOut.h"
#include "GPIOAccess.h"
#include "ForkAccess.h"

using namespace std;

ShiftOutOperation::ShiftOutOperation()
    : Operation(opShiftOut) {
    dataPin = NULL;
    clockPin = NULL;
    clockPeriodExpr = NULL;
    bitOrder = GPIO_MSB_FIRST;
    
    valueExpr = NULL;
}

bool ShiftOutOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No data pin specified for '" + mapFromOpType(opType) + "'");
        return false;
    }
    
    dataPin = PinExpression::create(opType, **paramIter, appInfo, false);
    if (dataPin == NULL) {
        appInfo->prtError(opType, "Invalid data pin expression for '" + mapFromOpType(opType) + "':" + **paramIter);
        return false;
    }

    (*paramIter)++;
    
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No clock pin specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    clockPin = PinExpression::create(opType, **paramIter, appInfo, false);
    if (clockPin == NULL) {
        appInfo->prtError(opType, "Invalid clock pin expression for '" + mapFromOpType(opType) + "':" + **paramIter);
        return false;
    }

    (*paramIter)++;

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No clock period specified for '" + mapFromOpType(opType) + "'");
        return false;
    }
    clockPeriodExpr = Expression::create(opType, **paramIter, appInfo);
    if (clockPeriodExpr == NULL) {
        appInfo->prtError(opType, "Invalid clock period for '" + mapFromOpType(opType) + "':" + **paramIter);
        return false;
    }
    (*paramIter)++;

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No bit order specified for '" + mapFromOpType(opType) + "'");
        return false;
    }
    if ((*paramIter)->compare("msb")) {
        bitOrder = GPIO_MSB_FIRST;
    } else if ((*paramIter)->compare("lsb")) {
        bitOrder = GPIO_LSB_FIRST;
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
        appInfo->prtError(opType, "Invalid value for '" + mapFromOpType(opType) + "':" + **paramIter);
        return false;
    }
    (*paramIter)++;

    return true;
}

string ShiftOutOperation::toString() {
    string s;
    s = Operation::toString()
            + " DataPin:" + dataPin->getExpressionString()
            + " ClockPin:" + clockPin->getExpressionString()
            + " ClockPeriodNS:" + clockPeriodExpr->getExpressionString();
    if (bitOrder == GPIO_MSB_FIRST) {
        s = s + " BitOrder:msb";
    } else {
        s = s + " BitOrder:lsb";
    }
    s = s + " Value:" + valueExpr->getExpressionString();
    
    return s;
}

bool ShiftOutOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int dataPinN;
    long int clockPinN;
    
    if (!dataPin->getPinNumber(dataPinN)) {
        return false;
    }
    if (!clockPin->getPinNumber(clockPinN)) {
        return false;
    }
    
    if (dataPinN == clockPinN) {
        appInfo->prtError(opType, "Data and clock pin numbers must be different for '" + mapFromOpType(opType) + "'");
        return false;
    }
    long int clockPeriodNS;
    if (!clockPeriodExpr->eval(clockPeriodNS)) {
        return false;
    }
    if (clockPeriodNS < 100) {
        appInfo->prtError(opType, "Invalid clock period for '" + mapFromOpType(opType) + "':" + clockPeriodExpr->getExpressionString() + "->" + to_string(clockPeriodNS));
    }
    long int value;
    if (!valueExpr->eval(value)) {
        return false;
    }
    if ((value < 0) || (value > 255)) {
        appInfo->prtError(opType, "Invalid value for '" + mapFromOpType(opType) + "':" + valueExpr->getExpressionString() + "->" + to_string(value));
    }

    ForkAccess::stop(dataPinN);
    ForkAccess::stop(clockPinN);

    string s = "Performing ShiftOut operation:";
                + " DataPin:" + to_string(dataPinN)
            + " ClockPin:" + to_string(clockPinN)
            + " ClockPeriodNS:" + to_string(clockPeriodNS);
    if (bitOrder == GPIO_MSB_FIRST) {
        s = s + " BitOrder:msb";
    } else {
        s = s + " BitOrder:lsb";
    }
    s = s + " Value:" + to_string(value);

    appInfo->prtReport(s);

    GPIOShiftOut * so = new GPIOShiftOut(dataPinN, clockPinN);
    so->setBitOrder(bitOrder);
    so->setClockPeriodNS(clockPeriodNS);
    so->write(value);

    delete so;
    
    return true;
}

string ShiftOutOperation::help() {
    stringstream hStr;
    hStr << "shiftout <datapin-expr> <clockpin-expr> <clockperiod-expr>";
    hStr << "\n\t\t<bitord> <value-expr>";
    hStr << "\n\tPerforms a serial output of a value on a data pin";
    hStr << "\n\tclocked by a clock pin";
    hStr << "\n\t<datapin-expr> specifies the data pin number";
    hStr << "\n\t<clockpin-expr> specifies the clock pin number";
    hStr << "\n\tThey must be expressions that evaluates to one of:";
    hStr << "\n\t\t0,1,6,7,8,12,13,14,15,16,17,18,19,23,26";
    hStr << "\n\tand evaluate to different pin numbers";
    hStr << "\n\t<clockperiod-expr> is an expression that evaluates to";
    hStr << "\n\tthe period of the clock in nano-seconds";
    hStr << "\n\tMust not be less than 100";
    hStr << "\n\t<bitord> is the order the bits are transferred";
    hStr << "\n\tMust be one of:";
    hStr << "\n\t\tmsb - most significant bit first";
    hStr << "\n\t\tlsb - least significant bit first";
    hStr << "\n\t<value-expr> is an expression that evaluates to";
    hStr << "\n\tthe value sent";
    hStr << "\n\tMust be >= 0 and <= 255";
    
    return hStr.str();
}
