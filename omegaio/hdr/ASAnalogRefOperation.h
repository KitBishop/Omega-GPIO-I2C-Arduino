#ifndef ASAnalogRefOperation_H
#define ASAnalogRefOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class ASAnalogRefOperation : public Operation {
public:
    ASAnalogRefOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    ArduinoARefMode refMode;

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
