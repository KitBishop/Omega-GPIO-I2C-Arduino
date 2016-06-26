#include "APSendBufOperation.h"
#include "Utilities.h"

using namespace std;

APSendBufOperation::APSendBufOperation()
    : Operation(opAPSendBuf) {
    cmdExpr = NULL;
    byteExprList = list<Expression *>();
}

bool APSendBufOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No command specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    cmdExpr = Expression::create(opType, **paramIter, appInfo);

    if (cmdExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No buffer byte list specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    stringstream ss(**paramIter);
    string item;
    while (getline(ss, item, ',')) {
        Expression * expr =Expression::create(opType, trim(item), appInfo);
        if (expr == NULL) {
            return false;
        }
        byteExprList.push_back(expr);
    }

    if ((byteExprList.size() < 1) || (byteExprList.size() > 32)) {
        appInfo->prtError(opType, "Invalid number of byte list items for '" + mapFromOpType(opType) + "'"
            + " :'" + **paramIter +"'");
        return false;
    }

    (*paramIter)++;

    return true;
}

string APSendBufOperation::toString() {
    stringstream ss;
    ss << Operation::toString() 
            << " Command:" 
            << cmdExpr->getExpressionString()
            << " ByteList:";
    
    int i = 0;
    ss << "{";
    for (auto ex : byteExprList) {
        ss << "'" << ex->getExpressionString() << "'";
        if (i != (byteExprList.size() - 1)) {
            ss << ",";
        }
        i++;
    }
    ss << "}";
    
    return ss.str();
}

bool APSendBufOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int command;
    if (!cmdExpr->eval(command)) {
        return false;
    }
    command = command & 0xff;

    I2C_Data i2cData;
    int i = 0;
    for (auto ex : byteExprList) {
        long int v;
        if (!ex->eval(v)) {
            return false;
        }
        i2cData.data[i] = v & 0xff;
        i++;
    }
    i2cData.size = byteExprList.size();

    ArduinoPort * arduinoPort = appInfo->getArduinoPort();
    if (arduinoPort == NULL) {
        appInfo->prtError(opType, "No current Arduino Port for '" + mapFromOpType(opType) + "'");
        return false;
    }
    
    stringstream ss;

    ss << "{";
    for (i = 0; i < i2cData.size; i++) {
        ss << (int)i2cData.data[i];
        if (i != (i2cData.size - 1)) {
            ss << ",";
        }
    }
    ss << "}";
    
    appInfo->prtReport(Operation::mapFromOpType(opType) 
        + " Command:" + to_string(command)
        + " Data:" + ss.str());
    
    Arduino_Result aRes = arduinoPort->sendBuffer(command, i2cData);


    bool res = aRes == ARDUINO_OK;
    if (!res) {
        appInfo->prtError(opType, "Arduino Port send buffer failed (" + arduinoResultStr(aRes) + ")");
    }

    return res;
}

string APSendBufOperation::help() {
    stringstream hStr;
    hStr << "apsendbuf <command-expr> <bufbytes-list>";
    hStr << "\n\tSends a buffer of bytesto the current Arduino Port";
    hStr << "\n\t<command-expr> is an expression that evaluates to the";
    hStr << "\n\tcommand to send.  Only the least significant 8 bits are used.";
    hStr << "\n\t<bufbytes-list> is a comma separated list of expressions each";
    hStr << "\n\trepresenting the value of one byte of the buffer.  It must be";
    hStr << "\n\tenclosed in quotes if it contains spaces or other special";
    hStr << "\n\tcharacters.  The list is of the form:";
    hStr << "\n\t   <val_expr>;<val_expr>;....";
    hStr << "\n\twhere there may be from at least 1 to at most 32 <val_expr>s";
    hStr << "\n\tEach <val_expr> is an expression that evaluates to the";
    hStr << "\n\tvalue of one byte.  The least significant 8 bits of each";
    hStr << "\n\tvalue are used for the byte.";
    hStr << "\n\tNOTE: Depending upon the implementation of the Port on the";
    hStr << "\n\tArduino, use of this operation will normally be followed by";
    hStr << "\n\tone of the operations 'apgetstatus', 'apget8','apget16',"; 
    hStr << "\n\t'apget32', 'apgetbuf' to retrieve the result of the operation.";
    
    return hStr.str();
}
