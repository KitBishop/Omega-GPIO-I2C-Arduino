#include "ASRebootOperation.h"
#include "Utilities.h"

using namespace std;

ASRebootOperation::ASRebootOperation()
    : Operation(opASReboot) {
}

bool ASRebootOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    return true;
}

string ASRebootOperation::toString() {
    return Operation::toString();
}

bool ASRebootOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    ArduinoSystem * arduinoSys = appInfo->getArduinoSystem();
    
    appInfo->prtReport("Rebooting Arduino System");
    
    Arduino_Result ares = arduinoSys->reboot();
    
    bool isok = ares == ARDUINO_OK;
    if (!isok) {
        appInfo->prtError(opType, "Failed to reboot (" + arduinoResultStr(ares) + ")");
    }
    
    return isok;
}

string ASRebootOperation::help() {
    stringstream hStr;
    hStr << "asreboot";
    hStr << "\n\tReboots the current Arduino System";
    
    return hStr.str();
}
