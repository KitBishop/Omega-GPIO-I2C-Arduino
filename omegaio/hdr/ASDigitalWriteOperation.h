#ifndef ASDigitalWriteOperation_H
#define ASDigitalWriteOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class ASDigitalWriteOperation : public Operation {
public:
    ASDigitalWriteOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    Expression * pinExpr;
    Expression * valExpr;

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
