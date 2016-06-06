#ifndef PulseOutOperation_H
#define PulseOutOperation_H

#include "PinOperation.h"

class PulseOutOperation : public PinOperation {
public:
    PulseOutOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    Expression * pulseLenExpr;
    Expression * pulseLevelExpr;
    
    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
