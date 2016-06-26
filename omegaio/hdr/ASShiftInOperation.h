#ifndef ASShiftInOperation_H
#define ASShiftInOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class ASShiftInOperation : public Operation {
public:
    ASShiftInOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    Expression * dataPinExpr;
    Expression * clockPinExpr;
    ArduinoBitOrder bitOrder;

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
