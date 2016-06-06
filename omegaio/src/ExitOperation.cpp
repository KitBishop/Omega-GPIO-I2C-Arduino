#include <unistd.h>
#include <string>
#include <cstdlib>

#include "ExitOperation.h"
#include "Utilities.h"

using namespace std;

ExitOperation::ExitOperation()
    : Operation(opExit) {
    result = 0;
}

bool ExitOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No result specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    if (!getInteger(**paramIter, result)) {
        appInfo->prtError(opType, "Invalid result for '" + mapFromOpType(opType) + "':" + **paramIter);
        return false;
    }

    (*paramIter)++;

    return true;
}

string ExitOperation::toString() {
    return Operation::toString()
            + " Result:" + to_string(result);
}

bool ExitOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    appInfo->prtReport("Exiting with result:" + to_string(result));
    
    appInfo->result = result;
    exit(result);
    
    return true;
}

string ExitOperation::help() {
    stringstream hStr;
    hStr << "exit <result>";
    hStr << "\n\tExits execution with given result";
    hStr << "\n\t<result> is the value to return on exit";
    
    return hStr.str();
}
