#include "I2CWriteOperation.h"
#include "Utilities.h"
#include "GPIOPin.h"

using namespace std;

I2CWriteOperation::I2CWriteOperation(int numBits)
    : I2COperation() {
    if (numBits == 8) {
        opType = opI2CWrite8;
    } else if(numBits == 16) {
        opType = opI2CWrite16;
    } else {
        opType = opI2CWrite32;
    }
}

bool I2CWriteOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
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
            appInfo->prtError(opType, "No value specified for '" + mapFromOpType(opType) + "'");
            return false;
        }
        
        valueExpr = Expression::create(opType, **paramIter, appInfo);

        if (valueExpr == NULL) {
            return false;
        }

        (*paramIter)++;
    } else {
        return false;
    }
    
    return true;
}

string I2CWriteOperation::toString() {
    return I2COperation::toString() 
            + " I2CRegisterExpression:" + regExpr->getExpressionString()
            + " Value:" + valueExpr->getExpressionString();
}

bool I2CWriteOperation::execute(AppInfo * appInfo) {
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
    long int val;
    if (!valueExpr->eval(val)) {
        return false;
    }

    I2CDevice * dev = appInfo->getI2CDevice(addr);
    
    if (noReg) {
        appInfo->prtReport(Operation::mapFromOpType(opType) 
            + " to address:" + to_string(addr)
            + " register:none"
            + " value:" + to_string(val));
    } else {
        appInfo->prtReport(Operation::mapFromOpType(opType) 
            + " to address:" + to_string(addr)
            + " register:" + to_string(reg)
            + " value:" + to_string(val));
    }
    
    I2C_Result i2cRes;
    switch (opType) {
        case opI2CWrite8:
            if (noReg) {
                i2cRes = dev->write8(val);
            } else {
                i2cRes = dev->write8(reg, val);
            }
            break;
            
        case opI2CWrite16:
            if (noReg) {
                i2cRes = dev->write16(val);
            } else {
                i2cRes = dev->write16(reg, val);
            }
            break;
            
        case opI2CWrite32:
            if (noReg) {
                i2cRes = dev->write32(val);
            } else {
                i2cRes = dev->write32(reg, val);
            }
            break;
    }

    if (i2cRes != I2C_OK) {
        appInfo->prtError(opType, "I2C write failed(" + i2cResultStr(i2cRes) + ")");
    }

    return true;
}

string I2CWriteOperation::help() {
    stringstream hStr;
    hStr << "i2cwrite8 <i2caddr-expr> <regaddr-expr> <value-expr>";
    hStr << "\ni2cwrite16 <i2caddr-expr> <regaddr-expr> <value-expr>";
    hStr << "\ni2cwrite32 <i2caddr-expr> <regaddr-expr> <value-expr>";
    hStr << "\n\tWrites an 8 bit or 16 bit or 32 bit value to I2C";
    hStr << "\n\t<i2caddr-expr> is an expression that evaluates to the";
    hStr << "\n\tI2C address.  Must be >= 0x03 and <= Ox7f";
    hStr << "\n\t<regaddr-expr> is an expression that evaluates to the";
    hStr << "\n\tI2C register address to be written to or ";
    hStr << "\n\t'none' for no register address.";
    hStr << "\n\tIf not 'none', must be >= 0 and <= Oxff";
    hStr << "\n\t<value-expr> is an expression that evaluates to the";
    hStr << "\n\tvalue to be written.";
    
    return hStr.str();
}
