#ifndef SetOperation_H
#define SetOperation_H

#include "PinOperation.h"

class SetOperation : public PinOperation {
public:
    SetOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    Expression * valueExpr;

    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
