#include <unistd.h>
#include <string>
#include <cstdlib>
#include <iostream>

#include "AssignOperation.h"
#include "Utilities.h"

using namespace std;

AssignOperation::AssignOperation()
    : Operation(opAssign) {
    assignName = "";
    expression = NULL;
}

bool AssignOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No variable specified for '" + mapFromOpType(opType) + "'");
        return false;
    }
    
    bool isOk = true;

    char c = (*paramIter)->at(0);
    
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
        int i;
        assignName = "";
        for (i = 0; (i < (*paramIter)->length()) && isOk; i++) {
            c = (*paramIter)->at(i);
            if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9'))) {
                assignName = assignName + c;
            } else {
                isOk = false;
            }
        }
    } else {
        isOk = false;
    }
    
    if (!isOk) {
        appInfo->prtError(opType, "Invalid variable name for '" + mapFromOpType(opType) + "':" + **paramIter);
        return false;
    }
    
    (*paramIter)++;

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No expression specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    expression = Expression::create(opType, **paramIter, appInfo);
    
    if (expression == NULL) {
        return false;
    }

    (*paramIter)++;

    return true;
}

string AssignOperation::toString() {
    return Operation::toString()
            + " Variable:" + assignName
            + " Expression:" + expression->getExpressionString();
}

bool AssignOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    string s = "";
    s = s + "Assigning:" + assignName + "=" + expression->getExpressionString();
    appInfo->prtReport(s);
    
    long int exprVal;
    if (expression->eval(exprVal)) {
        appInfo->setVar(assignName, exprVal);
        appInfo->prtOutput(exprVal);
//        appInfo->prtOutput(to_string(exprVal));
        appInfo->result = exprVal;
    } else {
        appInfo->prtError(opType, "Error evaluating expression:" + expression->getExpressionString());
        return false;
    }
    
    return true;
}

string AssignOperation::help() {
    stringstream hStr;
    hStr << "assign <variable-name> <expression>";
    hStr << "\n\tAssigns the given expression to the given variable";
    hStr << "\n\t<variable-name> is the user variable to assign to.";
    hStr << "\n\tAny sequence of letters and digits starting with a letter.";
    hStr << "\n\tNames are case sensitive";
    hStr << "\n\t<expression> is the expression value to assign.";
    hStr << "\n\tUse the help parameter -h:expression for help on expressions.";
    
    return hStr.str();
}
