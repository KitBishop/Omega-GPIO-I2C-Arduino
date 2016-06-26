#include "ArduinoPortOperation.h"
#include "Utilities.h"

using namespace std;

ArduinoPortOperation::ArduinoPortOperation()
    : Operation(opArduinoPort) {
    addrExpr = NULL;
    portExpr = NULL;
}

bool ArduinoPortOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
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

string ArduinoPortOperation::toString() {
    return Operation::toString() 
            + " I2CAddrExpression:" + addrExpr->getExpressionString()
            + " PortExpression:" + portExpr->getExpressionString();
}

bool ArduinoPortOperation::execute(AppInfo * appInfo) {
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

    appInfo->prtReport("Setting Arduino Port.  Addr:" + to_string(addr) + " Port:" + to_string(port));

    appInfo->setArduinoPort(addr, port);

    return true;
}

string ArduinoPortOperation::help() {
    stringstream hStr;
    hStr << "arduinoport <i2caddr-expr> <port-expr>";
    hStr << "\n\tSets the Arduino Port for usage by subsequent 'ap*' operations";
    hStr << "\n\t<i2caddr-expr> is an expression that evaluates to the";
    hStr << "\n\tI2C address to use for the Arduino Port.";
    hStr << "\n\tMust be >= 0x03 and <= Ox7f";
    hStr << "\n\t<port-expr> is an expression that evaluates to the";
    hStr << "\n\tArduino port number to be used for Arduino Port access";
    hStr << "\n\tMust be >= 0 and <= " << MAX_ARDUINO_PORT;
    hStr << "\n\tNOTE: Usage of Arduino Port access requires that an Arduino";
    hStr << "\n\tbe attached to the Omega for access with I2C";
    hStr << "\n\t(e.g. Arduino Dock) and that the Arduino be running";
    hStr << "\n\tan instance of OmegaPort from the Arduino library";
    hStr << "\n\t'Arduino-Omega' with the corresponding I2C address and port";
    hStr << "\n\tnumber and with the method 'processCommand' implemented.";
    
    return hStr.str();
}
