#include "APGetOperation.h"
#include "Utilities.h"

using namespace std;

APGetOperation::APGetOperation(int numBits)
    : Operation(opInvalid) {
    if (numBits == 0) {
        opType = opAPGetStatus;
    } else if (numBits == 8) {
        opType = opAPGet8;
    } else if(numBits == 16) {
        opType = opAPGet16;
    } else {
        opType = opAPGet32;
    }
}

bool APGetOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
        
    return true;
}

string APGetOperation::toString() {
    return Operation::toString();
}

bool APGetOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    ArduinoPort * arduinoPort = appInfo->getArduinoPort();
    if (arduinoPort == NULL) {
        appInfo->prtError(opType, "No current Arduino Port for '" + mapFromOpType(opType) + "'");
        return false;
    }

    appInfo->prtReport(Operation::mapFromOpType(opType));
    
    Arduino_Result aRes;
    long int val;
    switch (opType) {
        case opAPGetStatus:
            aRes = arduinoPort->getStatus();
            break;
            
        case opAPGet8:
            unsigned char vc;
            aRes = arduinoPort->get8(vc);
            val = vc;
            break;
            
        case opAPGet16:
            unsigned int v16;
            aRes = arduinoPort->get16(v16);
            val = v16;
            break;
            
        case opAPGet32:
            unsigned long v32;
            aRes = arduinoPort->get32(v32);
            val = v32;
            break;
    }

    if (aRes != ARDUINO_OK) {
        appInfo->prtError(opType, "Arduino Port get failed (" + arduinoResultStr(aRes) + ")");
    } else {
        if (opType != opAPGetStatus) {
            appInfo->prtOutput(val);
            appInfo->result = val;
        }
    }

    return true;
}

string APGetOperation::help() {
    stringstream hStr;
    hStr << "apgetstatus";
    hStr << "\napget8";
    hStr << "\napget16";
    hStr << "\napget32";
    hStr << "\n\tGets, displays and returns a status or an 8 bit or 16 bit or";
    hStr << "\n\t32 bit value from the current Arduino Port";
    hStr << "\n\tNOTE: Depending upon the implementation of the Port on the";
    hStr << "\n\tArduino, use of one of these operations will normally be";
    hStr << "\n\tcalled after performing one of the operations 'apsendcmd',"; 
    hStr << "\n\t'apsend8', 'apsend16', 'apsend32', 'apsendbuf' to retrieve the";
    hStr << "\n\tresult of that operation.";
    
    return hStr.str();
}
