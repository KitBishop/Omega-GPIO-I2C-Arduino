#ifndef PulseInOperation_H
#define PulseInOperation_H

#include "PinOperation.h"

class PulseInOperation : public PinOperation {
public:
    PulseInOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    Expression * timeOutExpr;
    Expression * pulseLevelExpr;
    
    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
