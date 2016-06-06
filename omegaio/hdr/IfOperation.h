#ifndef IfOperation_H
#define IfOperation_H

#include <list>

#include "Operation.h"
#include "Condition.h"

class IfOperation : public Operation {
public:
    IfOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
    Condition * condition;
    list<Node *> * thenBody;
    list<Node *> * elseBody;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
