#include "Condition.h"
#include "Operation.h"
#include "GPIOAccess.h"
#include "GPIOPin.h"
#include "ForkAccess.h"
#include "Utilities.h"

Condition::Condition() {
    condExpr = "";
    expression = NULL;
}

string Condition::toString() {
    string s = "Condition: Expression:'" + condExpr + "'";
    return s;
}

bool Condition::execute(Operation * parentOp, AppInfo * appInfo) {
    long int condVal;
    if (!expression->eval(condVal)) {
        appInfo->prtError(parentOp->opType, "Error evaluating condition:'" + condExpr + "' for:'" + Operation::mapFromOpType(parentOp->opType) + "'");
        return false;
    }
    
    return condVal != 0;
}

Condition * Condition::create(Operation * parentOp, AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter){
    if (*paramIter == paramList->end()) {
        appInfo->prtError(parentOp->opType, "No condition expression given for Condition for operation:'" + Operation::mapFromOpType(parentOp->opType) + "'");
        return NULL;
    }
    
    Condition * cond = NULL;
    
    Expression * expression = Expression::create(parentOp->opType, **paramIter, appInfo);
    
    if (expression == NULL) {
        return NULL;
    } else {
        cond = new Condition();
        cond ->condExpr = **paramIter;
        cond->expression = expression;
    }

    (*paramIter)++;

    return cond;
}
