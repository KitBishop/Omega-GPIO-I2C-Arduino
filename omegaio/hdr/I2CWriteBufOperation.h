#ifndef I2CWriteBufOperation_H
#define I2CWriteBufOperation_H

#include "I2COperation.h"
#include "I2CRegExpression.h"

class I2CWriteBufOperation : public I2COperation {
public:
    I2CWriteBufOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);
    
    I2CRegExpression * regExpr;
    list<Expression *> byteExprList;

    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
