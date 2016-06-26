#ifndef ASPinModeOperation_H
#define ASPinModeOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class ASPinModeOperation : public Operation {
public:
    ASPinModeOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    Expression * pinExpr;
    ArduinoPinMode pinMode;

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
