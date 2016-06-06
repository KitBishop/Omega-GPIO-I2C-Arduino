#ifndef Condition_H
#define Condition_H

#include <string>
#include <list>

#include "AppInfo.h"
#include "Operation.h"
#include "Expression.h"

class Condition {
public:
    Condition();
    string toString();
    
    bool execute(Operation * parentOp, AppInfo * appInfo);
  
    string condExpr;
    Expression * expression;
    
    static Condition * create(Operation * parentOp, AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
