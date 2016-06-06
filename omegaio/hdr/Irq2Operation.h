#ifndef Irq2Operation_H
#define Irq2Operation_H

#include "PinOperation.h"
#include "GPIOTypes.h"

class Irq2Operation : public PinOperation {
public:
    Irq2Operation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();

    string cmd1;
    string cmd2;
    Expression * debounceExpr;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
