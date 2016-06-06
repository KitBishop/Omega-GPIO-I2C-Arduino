#include "I2CProbeOperation.h"
#include "Utilities.h"
#include "GPIOPin.h"

using namespace std;

I2CProbeOperation::I2CProbeOperation()
    : I2COperation() {
    opType = opI2CProbe;
}


bool I2CProbeOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    I2COperation::execute(appInfo);

    long int addr;
    if (!addrExpr->getAddress(addr)) {
        return false;
    }

    I2CDevice * dev = appInfo->getI2CDevice(addr);
    
    appInfo->prtReport("I2CProbe to address:" + to_string(addr));
    
    I2C_Result i2cRes = dev->probe();

    int v;
    if (i2cRes == I2C_OK) {
        v = 1;
    } else {
        appInfo->prtError(opType, "I2C probe failed(" + i2cResultStr(i2cRes) + ")");
        v = 0;
    }

    appInfo->prtOutput(v);
//    appInfo->prtOutput(to_string(v));
    appInfo->result = v;

    return true;
}

string I2CProbeOperation::help() {
    stringstream hStr;
    hStr << "i2cprobe <i2caddr-expr>";
    hStr << "\n\tProbes an I2C address and displays and returns result";
    hStr << "\n\t<i2caddr-expr> is an expression that evaluates to the";
    hStr << "\n\tI2C address.  Must be >= 0x03 and <= Ox7f";
    
    return hStr.str();
}
