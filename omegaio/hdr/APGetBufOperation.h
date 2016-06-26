#ifndef APGetBufOperation_H
#define APGetBufOperation_H

#include "Operation.h"
#include "Expression.h"

class APGetBufOperation : public Operation {
public:
    APGetBufOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);
    
    Expression * maxBytesExpr;

    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
