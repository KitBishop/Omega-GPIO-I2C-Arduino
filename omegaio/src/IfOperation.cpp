#include <unistd.h>
#include <string>
#include <cstdlib>

#include "IfOperation.h"
#include "Utilities.h"
#include "Processing.h"

using namespace std;

IfOperation::IfOperation()
    : Operation(opIf) {
    condition = NULL;
    thenBody = NULL;
    elseBody = NULL;
}

bool IfOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    condition = Condition::create(this, appInfo, paramList, paramIter);
    if (condition == NULL) {
        return false;
    }

    thenBody = new list<Node *>();
    
    OperationType endOpType;
    bool ok;
    ok = Processing::convertParamsToNodes(appInfo, thenBody, paramList, paramIter, opIf, &endOpType);
    if (ok && (endOpType == opElse)) {
        elseBody = new list<Node *>();
        ok = Processing::convertParamsToNodes(appInfo, elseBody, paramList, paramIter, opElse, NULL);
    }
    
    return ok;
}

string IfOperation::toString() {
    return Operation::toString() + " " + condition->toString();
}

bool IfOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    appInfo->prtReport("Executing if with condition:" + condition->toString());
    bool ok = true;
    if (condition->execute(this, appInfo)) {
        appInfo->prtReport("Executing if then body");
        ok = Processing::executeNodes(appInfo, thenBody);
    } else if (elseBody != NULL) {
        appInfo->prtReport("Executing if else body");
        ok = Processing::executeNodes(appInfo, elseBody);
    }

    return ok;
}

string IfOperation::help() {
    stringstream hStr;
    hStr << "if <conditional-expression>";
    hStr << "\nendif";
    hStr << "\nOr:";
    hStr << "\nif <conditional-expression>";
    hStr << "\nelse";
    hStr << "\nendif";
    hStr << "\n\tExecutes all operations between 'if' and 'else' or";
    hStr << "\n\t'endif' if the <conditional-expression> is true";
    hStr << "\n\tIf 'else' is used, executes all operations between 'else' and";
    hStr << "\n\t'endif' if the <conditional-expression> is false";
    hStr << "\n\t<conditional-expression> is true if the expression evaluates";
    hStr << "\n\tto non-zero.";
    hStr << "\n\tUse the help parameter -h:expression for help on expressions.";
    
    return hStr.str();
}
