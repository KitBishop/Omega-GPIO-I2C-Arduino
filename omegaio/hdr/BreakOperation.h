#ifndef BreakOperation_H
#define BreakOperation_H

#include "Operation.h"

class BreakOperation : public Operation {
public:
    BreakOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
