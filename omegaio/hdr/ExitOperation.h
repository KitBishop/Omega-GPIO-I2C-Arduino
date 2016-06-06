#ifndef ExitOperation_H
#define ExitOperation_H

#include "Operation.h"

class ExitOperation : public Operation {
public:
    ExitOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
    int result;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
