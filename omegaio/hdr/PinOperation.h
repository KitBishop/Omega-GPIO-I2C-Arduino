#ifndef PinOperation_H
#define PinOperation_H

#include "Operation.h"
#include "PinExpression.h"

class PinOperation : public Operation {
public:
    PinOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    PinExpression * pinExpr;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);

    bool pinAllAllowed;
};

#endif
