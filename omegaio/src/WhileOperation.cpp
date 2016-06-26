#include <unistd.h>
#include <string>
#include <cstdlib>

#include "WhileOperation.h"
#include "Utilities.h"
#include "Processing.h"

using namespace std;

WhileOperation::WhileOperation()
    : Operation(opWhile) {
    condition = NULL;
    body = NULL;
}

bool WhileOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    condition = Condition::create(this, appInfo, paramList, paramIter);
    if (condition == NULL) {
        return false;
    }

    body = new list<Node *>();
    
    return Processing::convertParamsToNodes(appInfo, body, paramList, paramIter, opWhile, NULL);
}

string WhileOperation::toString() {
    return Operation::toString() + " " + condition->toString();
}

bool WhileOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    bool ok = true;

    appInfo->prtReport("Starting while loop:" + condition->toString());
    while (ok && condition->execute(this, appInfo) && !appInfo->breaking) {
        appInfo->prtReport("Starting loop body");
        ok = Processing::executeNodes(appInfo, body);
        if (appInfo->continuing) {
            appInfo->breaking = false;
            appInfo->continuing = false;
        }
    }

    if (appInfo->breaking) {
        appInfo->breaking = false;
    }

    return ok;
}

string WhileOperation::help() {
    stringstream hStr;
    hStr << "while <conditional-expression>";
    hStr << "\nendwhile";
    hStr << "\n\tRepeatedly execute all operations between 'while' and 'endwhile'";
    hStr << "\n\tso long as <conditional-expression> is true";
    hStr << "\n\t<conditional-expression> is true if the expression evaluates";
    hStr << "\n\tto non-zero.";
    hStr << "\n\tUse the help parameter -h:expression for help on expressions.";
    
    return hStr.str();
}
