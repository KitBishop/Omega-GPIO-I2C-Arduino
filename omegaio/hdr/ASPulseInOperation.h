#ifndef ASPulseInOperation_H
#define ASPulseInOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class ASPulseInOperation : public Operation {
public:
    ASPulseInOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    Expression * pinExpr;
    Expression * levelExpr;
    Expression * timeoutExpr;

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
