#ifndef APRetriesOperation_H
#define APRetriesOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class APRetriesOperation : public Operation {
public:
    APRetriesOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);
    
    Expression * delayExpr;
    Expression * countExpr;

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
