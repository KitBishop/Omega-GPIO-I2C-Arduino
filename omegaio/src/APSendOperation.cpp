#include "APSendOperation.h"
#include "Utilities.h"

using namespace std;

APSendOperation::APSendOperation(int numBits)
    : Operation(opInvalid) {
    if (numBits == 0) {
        opType = opAPSendCommand;
    } else if (numBits == 8) {
        opType = opAPSend8;
    } else if(numBits == 16) {
        opType = opAPSend16;
    } else {
        opType = opAPSend32;
    }
    cmdExpr = NULL;
    valueExpr = NULL;
}

bool APSendOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No command specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    cmdExpr = Expression::create(opType, **paramIter, appInfo);

    if (cmdExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    if (opType != opAPSendCommand) {
        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No value specified for '" + mapFromOpType(opType) + "'");
            return false;
        }

        valueExpr = Expression::create(opType, **paramIter, appInfo);

        if (valueExpr == NULL) {
            return false;
        }

        (*paramIter)++;
    }
        
    return true;
}

string APSendOperation::toString() {
    string str = Operation::toString() 
            + " Command:" + cmdExpr->getExpressionString();
    if (opType != opAPSendCommand) {
        str = str + " Value:" + valueExpr->getExpressionString();
    }
    return str;
}

bool APSendOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int command;
    if (!cmdExpr->eval(command)) {
        return false;
    }
    command = command & 0xff;
    
    long int val;
    if (opType != opAPSendCommand) {
        if (!valueExpr->eval(val)) {
            return false;
        }
    }
    if (opType == opAPSend8) {
        val = val & 0xff;
    } else if(opType == opAPSend16) {
        val = val & 0xffff;
    }
    
    ArduinoPort * arduinoPort = appInfo->getArduinoPort();
    if (arduinoPort == NULL) {
        appInfo->prtError(opType, "No current Arduino Port for '" + mapFromOpType(opType) + "'");
        return false;
    }

    if (opType != opAPSendCommand) {
        appInfo->prtReport(Operation::mapFromOpType(opType) 
            + " Command:" + to_string(command)
            + " Value:" + to_string(val));
    } else {
        appInfo->prtReport(Operation::mapFromOpType(opType) 
            + " Command:" + to_string(command));
    }
    
    Arduino_Result aRes;
    switch (opType) {
        case opAPSendCommand:
            aRes = arduinoPort->sendCmd(command);
            break;
            
        case opAPSend8:
            aRes = arduinoPort->send8(command, val & 0xff);
            break;
            
        case opAPSend16:
            aRes = arduinoPort->send16(command, val & 0xffff);
            break;
            
        case opAPSend32:
            aRes = arduinoPort->send32(command, val);
            break;
    }

    if (aRes != ARDUINO_OK) {
        appInfo->prtError(opType, "Arduino Port send failed (" + arduinoResultStr(aRes) + ")");
    }

    return true;
}

string APSendOperation::help() {
    stringstream hStr;
    hStr << "apsendcmd <command-expr>";
    hStr << "\napsend8 <command-expr> <value-expr>";
    hStr << "\napsend16 <command-expr> <value-expr>";
    hStr << "\napsend32 <command-expr> <value-expr>";
    hStr << "\n\tSends a command to the current Arduino Port with either";
    hStr << "\n\tno data or an 8 bit or 16 bit or 32 bit data value";
    hStr << "\n\t<command-expr> is an expression that evaluates to the";
    hStr << "\n\tcommand to send.  Only the least significant 8 bits are used.";
    hStr << "\n\t<value-expr> is an expression that evaluates to the";
    hStr << "\n\tvalue to be sent.";
    hStr << "\n\tNOTE: Depending upon the implementation of the Port on the";
    hStr << "\n\tArduino, use of one of these operations will normally be";
    hStr << "\n\tfollowed by one of the operations 'apgetstatus', 'apget8',"; 
    hStr << "\n\t'apget16', 'apget32', 'apgetbuf' to retrieve the result of";
    hStr << "\n\tthe operation.";
    
    return hStr.str();
}
