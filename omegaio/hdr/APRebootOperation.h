#ifndef APRebootOperation_H
#define APRebootOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class APRebootOperation : public Operation {
public:
    APRebootOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
