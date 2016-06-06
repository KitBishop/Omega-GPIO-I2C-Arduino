#ifndef ShiftOutOperation_H
#define ShiftOutOperation_H

#include "Operation.h"
#include "GPIOTypes.h"
#include "PinExpression.h"

class ShiftOutOperation : public Operation {
public:
    ShiftOutOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
    PinExpression * dataPin;
    PinExpression * clockPin;
    Expression * clockPeriodExpr;
    GPIO_Bit_Order bitOrder;
    
    Expression * valueExpr;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
