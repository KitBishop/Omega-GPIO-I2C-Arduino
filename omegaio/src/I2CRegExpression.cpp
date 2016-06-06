#include <stack>
#include <iostream>

#include "Operation.h"
#include "I2CRegExpression.h"
#include "Utilities.h"

I2CRegExpression::I2CRegExpression(string expStr, OperationType parOp, AppInfo * appInf)
    : Expression(expStr, parOp, appInf) {
    isNone = false;
}

I2CRegExpression * I2CRegExpression::create(OperationType parOp, string expStr, AppInfo * appInf, bool allowNone) {
    I2CRegExpression * exp = new I2CRegExpression(expStr, parOp, appInf);

    int initialOffset = 0;

    if (expStr.compare("none") == 0) {
        if (allowNone) {
            exp->isNone = true;
            return exp;
        } else {
            appInf->prtError(parOp, "I2C register 'none' not allowed for '" + Operation::mapFromOpType(parOp) + "'");
            delete exp;
            return NULL;
        }
    } else {
        if (exp->expr(initialOffset, '\0')) {
            if(exp->expError) {
                delete exp;
                return NULL;
            } else if (initialOffset != expStr.length()) {
                appInf->prtError(parOp, "Error in I2C register expression:'" + expStr + "' at offset:" + to_string(initialOffset) + " for '" + Operation::mapFromOpType(parOp) +"'");
                delete exp;
                return NULL;
            }
            return exp;
        } else {
            delete exp;
            return NULL;
        }
    }
}

bool I2CRegExpression::getRegister(long int &reg) {
    if (eval(reg)) {
        if ((reg < 0) || (reg > 0xff)) {
            appInfo->prtError(parentOp, "Unusable I2C register (" + to_string(reg) + ") for '" + Operation::mapFromOpType(parentOp) + "':" + getExpressionString() + "->" + to_string(reg));
            return false;
        }        
        return true;
    } else {
        return false;
    }
}

bool I2CRegExpression::getIsNone() {
    return isNone;
}
