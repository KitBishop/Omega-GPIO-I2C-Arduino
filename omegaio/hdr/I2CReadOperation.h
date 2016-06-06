#ifndef I2CReadOperation_H
#define I2CReadOperation_H

#include "I2COperation.h"
#include "I2CRegExpression.h"

class I2CReadOperation : public I2COperation {
public:
    I2CReadOperation(int numBits);
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);
    
    I2CRegExpression * regExpr;

    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
