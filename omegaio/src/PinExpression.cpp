#include <stack>
#include <iostream>

#include "Operation.h"
#include "PinExpression.h"
#include "Utilities.h"
#include "GPIOAccess.h"

PinExpression::PinExpression(string expStr, OperationType parOp, AppInfo * appInf)
    : Expression(expStr, parOp, appInf) {
    isAll = false;
}

PinExpression * PinExpression::create(OperationType parOp, string expStr, AppInfo * appInf, bool allowAll) {
    PinExpression * exp = new PinExpression(expStr, parOp, appInf);

    int initialOffset = 0;

    if (expStr.compare("all") == 0) {
        if (allowAll) {
            exp->isAll = true;
            return exp;
        } else {
            appInf->prtError(parOp, "Pin number 'all' not allowed for '" + Operation::mapFromOpType(parOp) + "'");
            delete exp;
            return NULL;
        }
    } else {
        if (exp->expr(initialOffset, '\0')) {
            if(exp->expError) {
                delete exp;
                return NULL;
            } else if (initialOffset != expStr.length()) {
                appInf->prtError(parOp, "Error in pin number expression:'" + expStr + "' at offset:" + to_string(initialOffset) + " for '" + Operation::mapFromOpType(parOp) +"'");
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

bool PinExpression::getPinNumber(long int &pinNum) {
    if (isAll) {
        pinNum = -1;
        return true;
    }
    
    if (eval(pinNum)) {
        if (!GPIOAccess::isPinUsable(pinNum)) {
            appInfo->prtError(parentOp, "Unusable pin number (" + to_string(pinNum) + ") for '" + Operation::mapFromOpType(parentOp) + "':" + getExpressionString() + "->" + to_string(pinNum));
            return false;
        }        
        return true;
    } else {
        return false;
    }
}
