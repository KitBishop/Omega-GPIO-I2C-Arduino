#ifndef PwmOperation_H
#define PwmOperation_H

#include "PinOperation.h"

class PwmOperation : public PinOperation {
public:
    PwmOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);
    
    static string help();

    Expression * freqExpr;
    Expression * dutyExpr;
    Expression * durationExpr;
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
