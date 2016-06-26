#ifndef APGetOperation_H
#define APGetOperation_H

#include "Operation.h"
#include "Expression.h"

class APGetOperation : public Operation {
public:
    APGetOperation(int numBits);
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);
    
    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
