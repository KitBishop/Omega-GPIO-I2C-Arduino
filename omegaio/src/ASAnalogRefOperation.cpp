#include "ASAnalogRefOperation.h"
#include "Utilities.h"

using namespace std;

ASAnalogRefOperation::ASAnalogRefOperation()
    : Operation(opASAnalogRef) {
    refMode = DEFAULT;
}

bool ASAnalogRefOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No mode specified for: '" + mapFromOpType(opType) + "'");
        return false;
    }

    if ((**paramIter).compare("default") == 0) {
        refMode = DEFAULT;
    } else if ((**paramIter).compare("internal") == 0) {
        refMode = INTERNAL;
    } else if ((**paramIter).compare("external") == 0) {
        refMode = EXTERNAL;
    } else {
        appInfo->prtError(opType, "Invalid mode for '" + mapFromOpType(opType) + "':" + **paramIter);
        return false;
    }
    
    (*paramIter)++;

    return true;
}

string ASAnalogRefOperation::toString() {
    string str = Operation::toString();
    str = str + " Mode:";
    if (refMode == DEFAULT) {
        str = str + "DEFAULT";
    } else if (refMode == INTERNAL) {
        str = str + "INTERNAL";
    } else {
        str = str + "EXTERNAL";
    }
    return str;
}

bool ASAnalogRefOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    ArduinoSystem * arduinoSys = appInfo->getArduinoSystem();
    
    string modeStr;
    if (refMode == DEFAULT) {
        modeStr = "DEFAULT";
    } else if (refMode == INTERNAL) {
        modeStr = "INTERNAL";
    } else {
        modeStr = "EXTERNAL";
    }
    
    appInfo->prtReport("Setting Arduino Analog Reference: Mode:" + modeStr);
    
    Arduino_Result ares = arduinoSys->analogReference(refMode);
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to set analog reference mode (" + arduinoResultStr(ares) + ")");
    }
    
    return isok;
}

string ASAnalogRefOperation::help() {
    stringstream hStr;
    hStr << "asanalogref <mode>";
    hStr << "\n\tSets Arduino System analog reference mode to the given value";
    hStr << "\n\t<mode> is the mode to use for the analog reference and must be";
    hStr << "\n\tone of:";
    hStr << "\n\t  - default";
    hStr << "\n\t  - internal";
    hStr << "\n\t  - external";
    
    return hStr.str();
}
