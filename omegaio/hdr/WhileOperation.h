#ifndef WhileOperation_H
#define WhileOperation_H

#include <list>

#include "Operation.h"
#include "Condition.h"

class WhileOperation : public Operation {
public:
    WhileOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
    Condition * condition;
    list<Node *> * body;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
