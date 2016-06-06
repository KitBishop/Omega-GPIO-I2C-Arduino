#ifndef AssignOperation_H
#define AssignOperation_H

#include "Operation.h"
#include "Expression.h"

class AssignOperation : public Operation {
public:
    AssignOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
    string assignName;
    Expression * expression;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
