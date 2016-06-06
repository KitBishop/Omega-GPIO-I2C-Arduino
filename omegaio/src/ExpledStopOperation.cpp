#include "ExpledStopOperation.h"
#include "Utilities.h"
#include "ForkAccess.h"

using namespace std;

ExpledStopOperation::ExpledStopOperation()
    : Operation(opExpledStop) {
}


bool ExpledStopOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    appInfo->prtReport("Stopping ExpLed output");
    
    ForkAccess::stop(15);
    ForkAccess::stop(16);
    ForkAccess::stop(17);

    return true;
}

bool ExpledStopOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    return true;
}

string ExpledStopOperation::help() {
    stringstream hStr;
    hStr << "expledstop";
    hStr << "\n\tStops any separate process that is currently performing";
    hStr << "\n\texpled output";
    
    return hStr.str();
}
