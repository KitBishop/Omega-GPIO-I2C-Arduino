#include "I2CWriteBufOperation.h"
#include "Utilities.h"
#include "GPIOPin.h"

using namespace std;

I2CWriteBufOperation::I2CWriteBufOperation()
    : I2COperation() {
    opType = opI2CWriteBuf;
    regExpr = NULL;
    byteExprList = list<Expression *>();
}

bool I2CWriteBufOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
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
    } else {
        return false;
    }
    
    return true;
}

string I2CWriteBufOperation::toString() {
    stringstream ss;
    ss << I2COperation::toString() 
            << " I2CRegisterExpression:" 
            << regExpr->getExpressionString()
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

bool I2CWriteBufOperation::execute(AppInfo * appInfo) {
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

    I2CDevice * dev = appInfo->getI2CDevice(addr);
    
    stringstream ss;

    ss << "{";
    for (i = 0; i < i2cData.size; i++) {
        ss << (int)i2cData.data[i];
        if (i != (i2cData.size - 1)) {
            ss << ",";
        }
    }
    ss << "}";
    if (noReg) {
        appInfo->prtReport(Operation::mapFromOpType(opType) 
            + " to address:" + to_string(addr)
            + " register:none"
            + " data:" + ss.str());
    } else {
        appInfo->prtReport(Operation::mapFromOpType(opType) 
            + " to address:" + to_string(addr)
            + " register:" + to_string(reg)
            + " data:" + ss.str());
    }
    
    I2C_Result i2cRes;

    if (noReg) {
        i2cRes = dev->writeBuffer(i2cData);
    } else {
        i2cRes = dev->writeBuffer(reg, i2cData);
    }

    bool res = i2cRes == I2C_OK;
    if (i2cRes != I2C_OK) {
        appInfo->prtError(opType, "I2C write buffer failed(" + i2cResultStr(i2cRes) + ")");
    }

    return res;
}

string I2CWriteBufOperation::help() {
    stringstream hStr;
    hStr << "i2writebuf <i2caddr-expr> <regaddr-expr> <bufbytes-list>";
    hStr << "\n\tWrites a buffer of bytes to I2C";
    hStr << "\n\t<i2caddr-expr> is an expression that evaluates to the";
    hStr << "\n\tI2C address.  Must be >= 0x03 and <= Ox7f";
    hStr << "\n\t<regaddr-expr> is an expression that evaluates to the";
    hStr << "\n\tI2C register address to be written to or ";
    hStr << "\n\t'none' for no register address.";
    hStr << "\n\tIf not 'none', must be >= 0 and <= Oxff";
    hStr << "\n\t<bufbytes-list> is a comma separated list of expressions each";
    hStr << "\n\trepresenting the value of one byte of the buffer.  It must be";
    hStr << "\n\tenclosed in quotes if it contains spaces or other special";
    hStr << "\n\tcharacters.  The list is of the form:";
    hStr << "\n\t   <val_expr>;<val_expr>;....";
    hStr << "\n\twhere there may be from at least 1 to at most 32 <val_expr>s";
    hStr << "\n\tEach <val_expr> is an expression that evaluates to the";
    hStr << "\n\tvalue of one byte.  The least significant 8 bits of each";
    hStr << "\n\tvalue are used for the byte.";
    
    return hStr.str();
}
