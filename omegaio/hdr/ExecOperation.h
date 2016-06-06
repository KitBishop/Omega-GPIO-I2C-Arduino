#ifndef ExecOperation_H
#define ExecOperation_H

#include "Operation.h"

class ExecOperation : public Operation {
public:
    ExecOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
    string command;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
