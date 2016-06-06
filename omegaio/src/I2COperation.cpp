#include <cstdlib>

#include "I2COperation.h"
#include "Utilities.h"
#include "GPIOAccess.h"
#include "ForkAccess.h"

using namespace std;

I2COperation::I2COperation()
    : Operation(opInvalid) {
    addrExpr = NULL;
}

bool I2COperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No I2C address specified for '" + mapFromOpType(opType) + "'");
        return false;
    }
    
    addrExpr = I2CAddrExpression::create(opType, **paramIter, appInfo);
    if (addrExpr == NULL) {
        return false;
    }

    (*paramIter)++;
    return true;
}

string I2COperation::toString() {
    return Operation::toString() + " I2CAddrExpression:" + addrExpr->getExpressionString();
}

bool I2COperation::execute(AppInfo * appInfo) {
    return true;
}