#ifndef ArduinoSystemOperation_H
#define ArduinoSystemOperation_H

#include "Operation.h"
#include "I2CAddrExpression.h"

class ArduinoSystemOperation : public Operation {
public:
    ArduinoSystemOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);
    
    I2CAddrExpression * addrExpr;
    Expression * portExpr;

    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
