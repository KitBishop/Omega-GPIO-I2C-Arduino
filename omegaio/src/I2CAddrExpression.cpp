#include <stack>
#include <iostream>

#include "Operation.h"
#include "I2CAddrExpression.h"
#include "Utilities.h"

I2CAddrExpression::I2CAddrExpression(string expStr, OperationType parOp, AppInfo * appInf)
    : Expression(expStr, parOp, appInf) {
}

I2CAddrExpression * I2CAddrExpression::create(OperationType parOp, string expStr, AppInfo * appInf) {
    I2CAddrExpression * exp = new I2CAddrExpression(expStr, parOp, appInf);

    int initialOffset = 0;

    if (exp->expr(initialOffset, '\0')) {
        if(exp->expError) {
            delete exp;
            return NULL;
        } else if (initialOffset != expStr.length()) {
            appInf->prtError(parOp, "Error in I2C address expression:'" + expStr + "' at offset:" + to_string(initialOffset) + " for '" + Operation::mapFromOpType(parOp) +"'");
            delete exp;
            return NULL;
        }
        return exp;
    } else {
        delete exp;
        return NULL;
    }
}

bool I2CAddrExpression::getAddress(long int &address) {
    if (eval(address)) {
        if ((address < 0x03) || (address >0x7f)) {
            appInfo->prtError(parentOp, "Unusable I2C address (" + to_string(address) + ") for '" + Operation::mapFromOpType(parentOp) + "':" + getExpressionString() + "->" + to_string(address));
            return false;
        }        
        return true;
    } else {
        return false;
    }
}
