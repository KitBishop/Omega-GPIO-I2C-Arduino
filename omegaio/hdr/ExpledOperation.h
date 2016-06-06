#ifndef ExpledOperation_H
#define ExpledOperation_H

#include "Operation.h"
#include "Expression.h"

class ExpledOperation : public Operation {
public:
    ExpledOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
    Expression * redExpr;
    Expression * greenExpr;
    Expression * blueExpr;
    Expression * allExpr;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
