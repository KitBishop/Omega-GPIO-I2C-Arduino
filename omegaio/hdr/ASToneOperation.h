#ifndef ASToneOperation_H
#define ASToneOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class ASToneOperation : public Operation {
public:
    ASToneOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    Expression * pinExpr;
    Expression * freqExpr;
    Expression * durationExpr;

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
