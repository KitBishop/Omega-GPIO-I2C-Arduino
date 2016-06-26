#ifndef ASRetriesOperation_H
#define ASRetriesOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class ASRetriesOperation : public Operation {
public:
    ASRetriesOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);
    
    Expression * delayExpr;
    Expression * countExpr;

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
