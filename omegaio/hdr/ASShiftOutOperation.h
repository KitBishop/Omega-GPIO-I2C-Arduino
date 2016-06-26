#ifndef ASShiftOutOperation_H
#define ASShiftOutOperation_H

#include "Operation.h"
#include "Expression.h"
#include "ArduinoSystem.h"

class ASShiftOutOperation : public Operation {
public:
    ASShiftOutOperation();
    virtual string toString();
  
    virtual bool execute(AppInfo * appInfo);

    Expression * dataPinExpr;
    Expression * clockPinExpr;
    ArduinoBitOrder bitOrder;
    Expression * valueExpr;

    static string help();

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
