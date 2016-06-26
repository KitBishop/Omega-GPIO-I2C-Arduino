#ifndef ArduinoPortOperation_H
#define ArduinoPortOperation_H

#include "Operation.h"
#include "I2CAddrExpression.h"

class ArduinoPortOperation : public Operation {
public:
    ArduinoPortOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);
    
    I2CAddrExpression * addrExpr;
    Expression * portExpr;

    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
