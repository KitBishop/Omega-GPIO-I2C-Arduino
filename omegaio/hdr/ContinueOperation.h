#ifndef ContinueOperation_H
#define ContinueOperation_H

#include "Operation.h"

class ContinueOperation : public Operation {
public:
    ContinueOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
