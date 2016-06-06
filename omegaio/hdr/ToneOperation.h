#ifndef ToneOperation_H
#define ToneOperation_H

#include "PinOperation.h"

class ToneOperation : public PinOperation {
public:
    ToneOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
    Expression * freqExpr;
    Expression * durationExpr;
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
