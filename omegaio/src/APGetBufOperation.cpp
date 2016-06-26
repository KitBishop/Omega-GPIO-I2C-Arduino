#include "APGetBufOperation.h"
#include "Utilities.h"

using namespace std;

APGetBufOperation::APGetBufOperation()
    : Operation(opAPGetBuf) {
    maxBytesExpr = NULL;
}

bool APGetBufOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No maximum bytes specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    maxBytesExpr = Expression::create(opType, **paramIter, appInfo);

    if (maxBytesExpr == NULL) {
        return false;
    }

    (*paramIter)++;
    
    return true;
}

string APGetBufOperation::toString() {
    return Operation::toString() 
            + " MaxBytes:" + maxBytesExpr->getExpressionString();
}

bool APGetBufOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int max;
    if (!maxBytesExpr->eval(max)) {
        return false;
    }
    if ((max <= 0) || (max > 32)){
        appInfo->prtError(opType, "Invalid max bytes for '" + mapFromOpType(opType) + "':" + maxBytesExpr->getExpressionString() + "->" + to_string(max));
        return false;
    }

    ArduinoPort * arduinoPort = appInfo->getArduinoPort();
    if (arduinoPort == NULL) {
        appInfo->prtError(opType, "No current Arduino Port for '" + mapFromOpType(opType) + "'");
        return false;
    }
    
    appInfo->prtReport(Operation::mapFromOpType(opType) 
        + " maxbytes:" + to_string(max));
    
    Arduino_Result aRes;
    I2C_Data i2cData;
    i2cData.size = 0;
    
    aRes = arduinoPort->getBuffer(i2cData, max);

    bool res = aRes == ARDUINO_OK;
    if (!res) {
        appInfo->prtError(opType, "Arduino Port get buffer failed (" + arduinoResultStr(aRes) + ")");
    } else {
        stringstream ss;
        int i;
        
        if (appInfo->hexout) {
            ss << std::hex;
        } else {
            ss << std::dec;
        }

        for (i = 0; i < i2cData.size; i++) {
            if (appInfo->hexout) {
                ss << "0x";
            }
            ss << (unsigned int)i2cData.data[i] << " ";
        }

        appInfo->prtOutput(ss.str());
    }

    return res;
}

string APGetBufOperation::help() {
    stringstream hStr;
    hStr << "apgetbuf <maxbytes-expr>";
    hStr << "\n\tReads and displays a buffer from the current Arduino Port";
    hStr << "\n\t<maxbytes-expr> is an expression that evaluates to the";
    hStr << "\n\tmaximum number of bytes to be read. Must be > 0 and <= 32";
    hStr << "\n\tNOTE: Depending upon the implementation of the Port on the";
    hStr << "\n\tArduino, use of this operation may need to be called after";
    hStr << "\n\tperforming one of the operations 'apsendcmd', 'apsend8',"; 
    hStr << "\n\t'apsend16', 'apsend32', 'apsendbuf' to retrieve the result of";
    hStr << "\n\tthat operation.";
    
    return hStr.str();
}
