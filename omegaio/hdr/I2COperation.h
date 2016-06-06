#ifndef I2COperation_H
#define I2COperation_H

#include "Operation.h"
#include "I2CAddrExpression.h"

class I2COperation : public Operation {
public:
    I2COperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    I2CAddrExpression * addrExpr;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
