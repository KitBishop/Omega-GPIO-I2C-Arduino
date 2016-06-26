#ifndef APSendBufOperation_H
#define APSendBufOperation_H

#include "Operation.h"
#include "Expression.h"

class APSendBufOperation : public Operation {
public:
    APSendBufOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);
    
    Expression * cmdExpr;
    list<Expression *> byteExprList;

    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
