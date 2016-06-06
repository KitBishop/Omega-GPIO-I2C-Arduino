#ifndef I2CWriteOperation_H
#define I2CWriteOperation_H

#include "I2COperation.h"
#include "I2CRegExpression.h"

class I2CWriteOperation : public I2COperation {
public:
    I2CWriteOperation(int numBits);
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);
    
    I2CRegExpression * regExpr;
    Expression * valueExpr;

    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
