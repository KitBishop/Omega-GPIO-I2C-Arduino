#include <unistd.h>
#include <string>
#include <cstdlib>

#include "ContinueOperation.h"
#include "Utilities.h"

using namespace std;

ContinueOperation::ContinueOperation()
    : Operation(opContinue) {
}

bool ContinueOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    return true;
}

string ContinueOperation::toString() {
    return Operation::toString();
}

bool ContinueOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    appInfo->prtReport("Continuing while loop or execution");
    
    appInfo->breaking = true;
    appInfo->continuing = true;
    
    return true;
}

string ContinueOperation::help() {
    stringstream hStr;
    hStr << "continue";
    hStr << "\n\tIf within a 'while' loop, immediately continues next execution";
    hStr << "\n\tof the loop body skipping execution of rest of body.";
    hStr << "\n\tIf not within a 'while' loop, ends all execution";
    
    return hStr.str();
}
