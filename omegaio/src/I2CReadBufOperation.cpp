#include "I2CReadBufOperation.h"
#include "Utilities.h"
#include "GPIOPin.h"

using namespace std;

I2CReadBufOperation::I2CReadBufOperation()
    : I2COperation() {
    opType = opI2CReadBuf;
    regExpr = NULL;
    maxBytesExpr = NULL;
}

bool I2CReadBufOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (I2COperation::build(appInfo, paramList, paramIter)) {
        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No I2C register specified for '" + mapFromOpType(opType) + "'");
            return false;
        }
        
        regExpr = I2CRegExpression::create(opType, **paramIter, appInfo, true);

        if (regExpr == NULL) {
            return false;
        }

        (*paramIter)++;

        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No maximum bytes specified for '" + mapFromOpType(opType) + "'");
            return false;
        }
        
        maxBytesExpr = Expression::create(opType, **paramIter, appInfo);

        if (maxBytesExpr == NULL) {
            return false;
        }

        (*paramIter)++;
    } else {
        return false;
    }
    
    return true;
}

string I2CReadBufOperation::toString() {
    return I2COperation::toString() 
            + " I2CRegisterExpression:" + regExpr->getExpressionString()
            + " MaxBytes:" + maxBytesExpr->getExpressionString();
}

bool I2CReadBufOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    I2COperation::execute(appInfo);

    long int addr;
    if (!addrExpr->getAddress(addr)) {
        return false;
    }
    long int reg;
    bool noReg = regExpr->getIsNone();
    if (!noReg) {
        if (!regExpr->getRegister(reg)) {
            return false;
        }
    }
    long int max;
    if (!maxBytesExpr->eval(max)) {
        return false;
    }
    if ((max <= 0) || (max > 32)){
        appInfo->prtError(opType, "Invalid max bytes for '" + mapFromOpType(opType) + "':" + maxBytesExpr->getExpressionString() + "->" + to_string(max));
        return false;
    }

    I2CDevice * dev = appInfo->getI2CDevice(addr);
    
    if (noReg) {
        appInfo->prtReport(Operation::mapFromOpType(opType) 
            + " from address:" + to_string(addr)
            + " register:none"
            + " maxbytes:" + to_string(max));
    } else {
        appInfo->prtReport(Operation::mapFromOpType(opType) 
            + " from address:" + to_string(addr)
            + " register:" + to_string(reg)
            + " maxbytes:" + to_string(max));
    }
    
    I2C_Result i2cRes;
    I2C_Data i2cData;
    
    i2cData.size = 0;

    if (noReg) {
        i2cRes = dev->readBuffer(i2cData, max);
    } else {
        i2cRes = dev->readBuffer(reg, i2cData, max);
    }

    bool res = i2cRes == I2C_OK;
    if (i2cRes != I2C_OK) {
        appInfo->prtError(opType, "I2C read buffer failed(" + i2cResultStr(i2cRes) + ")");
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

string I2CReadBufOperation::help() {
    stringstream hStr;
    hStr << "i2creadbuf <i2caddr-expr> <regaddr-expr> <maxbytes-expr>";
    hStr << "\n\tReads and displays a buffer from I2C";
    hStr << "\n\t<i2caddr-expr> is an expression that evaluates to the";
    hStr << "\n\tI2C address.  Must be >= 0x03 and <= Ox7f";
    hStr << "\n\t<regaddr-expr> is an expression that evaluates to the";
    hStr << "\n\tI2C register address to be read from or ";
    hStr << "\n\t'none' for no register address.";
    hStr << "\n\tIf not 'none', must be >= 0 and <= Oxff";
    hStr << "\n\t<maxbytes-expr> is an expression that evaluates to the";
    hStr << "\n\tmaximum number of bytes to be read. Must be > 0 and <= 32";
    
    return hStr.str();
}
