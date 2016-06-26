#ifndef APSendOperation_H
#define APSendOperation_H

#include "Operation.h"
#include "Expression.h"

class APSendOperation : public Operation {
public:
    APSendOperation(int numBits);
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);
    
    Expression * cmdExpr;
    Expression * valueExpr;

    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
