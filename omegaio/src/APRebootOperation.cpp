#include "APRebootOperation.h"
#include "Utilities.h"

using namespace std;

APRebootOperation::APRebootOperation()
    : Operation(opAPReboot) {
}

bool APRebootOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    return true;
}

string APRebootOperation::toString() {
    return Operation::toString();
}

bool APRebootOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    ArduinoPort * arduinoPort = appInfo->getArduinoPort();
    if (arduinoPort == NULL) {
        appInfo->prtError(opType, "No current Arduino Port for '" + mapFromOpType(opType) + "'");
        return false;
    }
    
    appInfo->prtReport("Rebooting Arduino Port");
    
    Arduino_Result ares = arduinoPort->reboot();
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to reboot (" + arduinoResultStr(ares) + ")");
    }
    
    return isok;
}

string APRebootOperation::help() {
    stringstream hStr;
    hStr << "apreboot";
    hStr << "\n\tReboots the current Arduino Port";
    
    return hStr.str();
}
