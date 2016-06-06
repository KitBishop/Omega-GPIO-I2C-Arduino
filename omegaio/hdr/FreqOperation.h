#ifndef FreqOperation_H
#define FreqOperation_H

#include "PinOperation.h"

class FreqOperation : public PinOperation {
public:
    FreqOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    Expression * sampleTimeExpr;
    
    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
