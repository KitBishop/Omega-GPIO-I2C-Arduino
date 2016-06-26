#include "ASToneOperation.h"
#include "Utilities.h"

using namespace std;

ASToneOperation::ASToneOperation()
    : Operation(opASTone) {
    pinExpr = NULL;
    freqExpr = NULL;
    durationExpr = NULL;
}

bool ASToneOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
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
        appInfo->prtError(opType, "No frequency specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    freqExpr = Expression::create(opType, **paramIter, appInfo);

    if (freqExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No duration specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    durationExpr = Expression::create(opType, **paramIter, appInfo);

    if (durationExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    return true;
}

string ASToneOperation::toString() {
    return Operation::toString() + " ArduinoPin:" + pinExpr->getExpressionString() 
            + " Frequency:" + freqExpr->getExpressionString()
            + " DurationMS:" + durationExpr->getExpressionString();
}

bool ASToneOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int pinNumber;
    if (!pinExpr->eval(pinNumber)) {
        return false;
    }
        
    long int freq;
    if (!freqExpr->eval(freq)) {
        return false;
    }
    if (freq <= 0) {
        appInfo->prtError(opType, "Invalid frequency for '" + mapFromOpType(opType) + "':" + freqExpr->getExpressionString() + "->" + to_string(freq));
        return false;
    }
    
    long int duration;
    if (!durationExpr->eval(duration)) {
        return false;
    }
    if (duration < 0) {
        appInfo->prtError(opType, "Invalid duration for '" + mapFromOpType(opType) + "':" + durationExpr->getExpressionString() + "->" + to_string(duration));
        return false;
    }
    
    ArduinoSystem * arduinoSys = appInfo->getArduinoSystem();
    
    if (appInfo->setdir) {
        appInfo->prtReport("Setting Arduino pin:" + to_string(pinNumber) + " to output");
        arduinoSys->pinMode(pinNumber, OUTPUT);
    }

    appInfo->prtReport("Starting tone output on Arduino Pin:" + to_string(pinNumber) 
        + " Frequency:" + to_string(freq)
        + " DurationMS:" + to_string(duration));
    
    Arduino_Result ares = arduinoSys->tone(pinNumber, freq, duration);
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to start tone output (" + arduinoResultStr(ares) + ")");
    }
    
    return isok;
}

string ASToneOperation::help() {
    stringstream hStr;
    hStr << "astone <arduinopin-expr> <freq-expr> <duration-expr>";
    hStr << "\n\tStarts tone output to Arduino System pin";
    hStr << "\n\tOn the Arduino, uses the function";
    hStr << "\n\t'tone(pin, frequency, duration)'";
    hStr << "\n\t<arduinopin-expr> is an expression that evaluates to";
    hStr << "\n\tpin number to use.";
    hStr << "\n\t<freq-expr> is an expression that evaluates to give the";
    hStr << "\n\tfrequency of the tone.  Must be > 0";
    hStr << "\n\t<duration-expr> is an expression that evaluates to";
    hStr << "\n\tthe duration in milliseconds for which the tone is output.";
    hStr << "\n\tMust be >= 0  If 0, tone is continuous.";
    hStr << "\n\tNOTE: The tone continues until the duration expires or";
    hStr << "\n\tanother 'astone' operation is used or the 'asnotone' operation";
    hStr << "\n\tis used on the same pin.";
    
    return hStr.str();
}
