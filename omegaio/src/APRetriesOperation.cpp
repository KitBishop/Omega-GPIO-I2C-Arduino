#include "APRetriesOperation.h"
#include "Utilities.h"

using namespace std;

APRetriesOperation::APRetriesOperation()
    : Operation(opAPRetries) {
    delayExpr = NULL;
    countExpr = NULL;
}

bool APRetriesOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No retry delay specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    delayExpr = Expression::create(opType, **paramIter, appInfo);

    if (delayExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No retry count specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    countExpr = Expression::create(opType, **paramIter, appInfo);

    if (countExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    return true;
}

string APRetriesOperation::toString() {
    return Operation::toString() + " RetryDelayMS:" + delayExpr->getExpressionString()
             + " RetryCount:" + countExpr->getExpressionString();
}

bool APRetriesOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int delay;
    if (!delayExpr->eval(delay)) {
        return false;
    }
    if (delay < 0) {
        appInfo->prtError(opType, "Invalid retry delay for '" + mapFromOpType(opType) + "':" + delayExpr->getExpressionString() + "->" + to_string(delay));
        return false;
    }

    long int count;
    if (!countExpr->eval(count)) {
        return false;
    }
    
    ArduinoPort * arduinoPort = appInfo->getArduinoPort();
    if (arduinoPort == NULL) {
        appInfo->prtError(opType, "No current Arduino Port for '" + mapFromOpType(opType) + "'");
        return false;
    }
    
    appInfo->prtReport("Setting Arduino Port retries. DelayMS:" + to_string(delay)
        + " Count:" + to_string(count));
    
    arduinoPort->setRetryDelay(delay);
    arduinoPort->setRetryCount(count);
    
    return true;
}

string APRetriesOperation::help() {
    stringstream hStr;
    hStr << "apretries <delay-expr> <count-expr>";
    hStr << "\n\tSets the retry information for the current Arduino Port";
    hStr << "\n\t<delay-expr> is an expression that evaluates to";
    hStr << "\n\tthe delay in milliseconds between retries on error.";
    hStr << "\n\tMust be >= 0";
    hStr << "\n\t<count-expr> is an expression that evaluates to";
    hStr << "\n\tthe maximum number of retries to perform on error.";
    hStr << "\n\tLess than 0 represents unlimited retries.";
    
    return hStr.str();
}
