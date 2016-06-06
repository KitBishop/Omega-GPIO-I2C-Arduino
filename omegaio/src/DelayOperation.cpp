#include "DelayOperation.h"
#include "Utilities.h"
#include "TimeHelper.h"
#include "Expression.h"

#include <cstdlib>

using namespace std;

DelayOperation::DelayOperation()
    : Operation(opDelay) {
    durationExpr = NULL;
}

bool DelayOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No duration specified '" + mapFromOpType(opType) + "'");
        return false;
    }

    durationExpr = Expression::create(opType, **paramIter, appInfo);
    
    if (durationExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    return true;
}

string DelayOperation::toString() {
    return Operation::toString() + " DurationMS:" + durationExpr->getExpressionString();
}

bool DelayOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }
    
    long int durationMs;
    if (!durationExpr->eval(durationMs)) {
        return false;
    }
    
    if (durationMs < 0) {
        appInfo->prtError(opType, "Invalid duration for '" + mapFromOpType(opType) + "':" + durationExpr->getExpressionString() + "->" + to_string(durationMs));
        return false;
    }

    appInfo->prtReport("Delaying for :" + to_string(durationMs) + " MS");
    
    sleepMilli(durationMs);

    return true;
}

string DelayOperation::help() {
    stringstream hStr;
    hStr << "delay <delay-expr>";
    hStr << "\n\tPauses execution for the given period";
    hStr << "\n\t<delay-expr> is an expression that evaluates to";
    hStr << "\n\tthe delay period in milliseconds. Must be >=0";
    
    return hStr.str();
}
