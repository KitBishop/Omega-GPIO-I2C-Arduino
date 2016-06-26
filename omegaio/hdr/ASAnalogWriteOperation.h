#ifndef ASAnalogWriteOperation_H
#define ASAnalogWriteOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class ASAnalogWriteOperation : public Operation {
public:
    ASAnalogWriteOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    Expression * pinExpr;
    Expression * valExpr;

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
