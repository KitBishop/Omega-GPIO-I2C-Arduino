#include "ArduinoSystemOperation.h"
#include "Utilities.h"

using namespace std;

ArduinoSystemOperation::ArduinoSystemOperation()
    : Operation(opArduinoSystem) {
    addrExpr = NULL;
    portExpr = NULL;
}

bool ArduinoSystemOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No I2C address specified for '" + mapFromOpType(opType) + "'");
        return false;
    }
    
    addrExpr = I2CAddrExpression::create(opType, **paramIter, appInfo);
    if (addrExpr == NULL) {
        return false;
    }

    (*paramIter)++;

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No port number specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    portExpr = Expression::create(opType, **paramIter, appInfo);

    if (portExpr == NULL) {
        return false;
    }

    (*paramIter)++;
    
    return true;
}

string ArduinoSystemOperation::toString() {
    return Operation::toString() 
            + " I2CAddrExpression:" + addrExpr->getExpressionString()
            + " PortExpression:" + portExpr->getExpressionString();
}

bool ArduinoSystemOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    long int addr;
    if (!addrExpr->getAddress(addr)) {
        return false;
    }
    long int port;
    if (!portExpr->eval(port)) {
        return false;
    }
    if ((port < 0) || (port > MAX_ARDUINO_PORT)) {
        appInfo->prtError(opType, "Invalid port number for '" + mapFromOpType(opType) + "':" + portExpr->getExpressionString() + "->" + to_string(port));
        return false;
    }

    appInfo->prtReport("Setting Arduino System.  Addr:" + to_string(addr) + " Port:" + to_string(port));

    appInfo->setArduinoSystem(addr, port);

    return true;
}

string ArduinoSystemOperation::help() {
    stringstream hStr;
    hStr << "arduinosys <i2caddr-expr> <port-expr>";
    hStr << "\n\tSets the Arduino System for usage by subsequent 'as*' operations";
    hStr << "\n\tBy default, if no Arduino System has been set the default system";
    hStr << "\n\ton I2C address 0x08 and port 0 is used";
    hStr << "\n\t<i2caddr-expr> is an expression that evaluates to the";
    hStr << "\n\tI2C address to use for the Arduino System.";
    hStr << "\n\tMust be >= 0x03 and <= Ox7f";
    hStr << "\n\t<port-expr> is an expression that evaluates to the";
    hStr << "\n\tArduino port number to be used for Arduino System access";
    hStr << "\n\tMust be >= 0 and <= " << MAX_ARDUINO_PORT;
    hStr << "\n\tNOTE: Usage of Arduino System access requires that an";
    hStr << "\n\tArduino be attached to the Omega for access with I2C";
    hStr << "\n\t(e.g. Arduino Dock) and that the Arduino be running";
    hStr << "\n\tan instance of OmegaArduinoSystemPort from the Arduino";
    hStr << "\n\tlibrary 'Arduino-Omega' with the corresponding I2C address";
    hStr << "\n\tand port number.";
    
    return hStr.str();
}
