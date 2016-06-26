#include "I2CReadOperation.h"
#include "Utilities.h"
#include "GPIOPin.h"

using namespace std;

I2CReadOperation::I2CReadOperation(int numBits)
    : I2COperation() {
    if (numBits == 8) {
        opType = opI2CRead8;
    } else if(numBits == 16) {
        opType = opI2CRead16;
    } else {
        opType = opI2CRead32;
    }
}

bool I2CReadOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
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
    } else {
        return false;
    }
    
    return true;
}

string I2CReadOperation::toString() {
    return I2COperation::toString() 
            + " I2CRegisterExpression:" + regExpr->getExpressionString();
}

bool I2CReadOperation::execute(AppInfo * appInfo) {
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

    I2CDevice * dev = appInfo->getI2CDevice(addr);
    
    if (noReg) {
        appInfo->prtReport(Operation::mapFromOpType(opType) 
            + " from address:" + to_string(addr)
            + " register:none");
    } else {
        appInfo->prtReport(Operation::mapFromOpType(opType) 
            + " from address:" + to_string(addr)
            + " register:" + to_string(reg));
    }
    
    I2C_Result i2cRes;
    long int val;
    unsigned char uc;
    unsigned int ui;
    unsigned long int ul;
    switch (opType) {
        case opI2CRead8:
            if (noReg) {
                i2cRes = dev->read8(uc);
                val = uc;
            } else {
                i2cRes = dev->read8(reg, uc);
                val = uc;
            }
            break;
            
        case opI2CRead16:
            if (noReg) {
                i2cRes = dev->read16(ui);
                val = ui;
            } else {
                i2cRes = dev->read16(reg, ui);
                val =ui;
            }
            break;
            
        case opI2CRead32:
            if (noReg) {
                i2cRes = dev->read32(ul);
                val = (long int)ul;
            } else {
                i2cRes = dev->read32(reg, ul);
                val = (long int)ul;
            }
            break;
    }

    bool res = i2cRes == I2C_OK;
    if (i2cRes != I2C_OK) {
        appInfo->prtError(opType, "I2C read failed(" + i2cResultStr(i2cRes) + ")");
    } else {
        appInfo->prtOutput(val);
        appInfo->result = val;
    }

    return res;
}

string I2CReadOperation::help() {
    stringstream hStr;
    hStr << "i2cread8 <i2caddr-expr> <regaddr-expr>";
    hStr << "\ni2cread16 <i2caddr-expr> <regaddr-expr>";
    hStr << "\ni2cread32 <i2caddr-expr> <regaddr-expr>";
    hStr << "\n\tReads, displays and returns an 8 bit or 16 bit or 32 bit";
    hStr << "\n\tvalue from I2C";
    hStr << "\n\t<i2caddr-expr> is an expression that evaluates to the";
    hStr << "\n\tI2C address.  Must be >= 0x03 and <= Ox7f";
    hStr << "\n\t<regaddr-expr> is an expression that evaluates to the";
    hStr << "\n\tI2C register address to be read from or ";
    hStr << "\n\t'none' for no register address.";
    hStr << "\n\tIf not 'none', must be >= 0 and <= Oxff";
    
    return hStr.str();
}
