#ifndef I2CReadBufOperation_H
#define I2CReadBufOperation_H

#include "I2COperation.h"
#include "I2CRegExpression.h"

class I2CReadBufOperation : public I2COperation {
public:
    I2CReadBufOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);
    
    I2CRegExpression * regExpr;
    Expression * maxBytesExpr;

    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
