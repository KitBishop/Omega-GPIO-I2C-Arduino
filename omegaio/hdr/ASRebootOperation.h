#ifndef ASRebootOperation_H
#define ASRebootOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class ASRebootOperation : public Operation {
public:
    ASRebootOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
