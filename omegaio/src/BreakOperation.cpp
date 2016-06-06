#include <unistd.h>
#include <string>
#include <cstdlib>

#include "BreakOperation.h"
#include "Utilities.h"

using namespace std;

BreakOperation::BreakOperation()
    : Operation(opBreak) {
}

bool BreakOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    return true;
}

string BreakOperation::toString() {
    return Operation::toString();
}

bool BreakOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    appInfo->prtReport("Breaking while loop or execution");
    
    appInfo->breaking = true;
    
    return true;
}

string BreakOperation::help() {
    stringstream hStr;
    hStr << "break";
    hStr << "\n\tIf within a 'while' loop, breaks execution of the loop";
    hStr << "\n\tIf not within a 'while' loop, ends all execution";
    
    return hStr.str();
}
