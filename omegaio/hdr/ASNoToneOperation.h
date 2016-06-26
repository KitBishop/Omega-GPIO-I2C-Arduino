#ifndef ASNoToneOperation_H
#define ASNoToneOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class ASNoToneOperation : public Operation {
public:
    ASNoToneOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    Expression * pinExpr;

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
