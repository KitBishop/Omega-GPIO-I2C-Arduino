#ifndef ExpledStopOperation_H
#define ExpledStopOperation_H

#include "Operation.h"

class ExpledStopOperation : public Operation {
public:
    ExpledStopOperation();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
