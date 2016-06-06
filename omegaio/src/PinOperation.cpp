#include <cstdlib>

#include "PinOperation.h"
#include "Utilities.h"
#include "GPIOAccess.h"
#include "ForkAccess.h"

using namespace std;

PinOperation::PinOperation()
    : Operation(opInvalid) {
    pinExpr = NULL;
    pinAllAllowed = false;
}

bool PinOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No pin number specified for '" + mapFromOpType(opType) + "'");
        return false;
    }
    
    pinExpr = PinExpression::create(opType, **paramIter, appInfo, pinAllAllowed);
    if (pinExpr == NULL) {
        return false;
    }

    (*paramIter)++;
    return true;
}

string PinOperation::toString() {
    return Operation::toString() + " PinNumberExpression:" + pinExpr->getExpressionString();
}

bool PinOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int pinNumber;
    if (!pinExpr->getPinNumber(pinNumber)) {
        return false;
    }
    int firstPin = pinNumber;
    int lastPin = pinNumber;
    if (pinNumber == -1) {
        firstPin = 0;
        lastPin = 30;
    } 
    for (int pinN = firstPin; pinN <= lastPin; pinN++) {
        if (GPIOAccess::isPinUsable(pinN)) {
            ForkAccess::stop(pinN);
        }
    }
    
    return true;
}