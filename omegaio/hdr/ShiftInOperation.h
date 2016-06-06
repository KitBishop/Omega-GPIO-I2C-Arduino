#ifndef ShiftInOperation_H
#define ShiftInOperation_H

#include "Operation.h"
#include "GPIOTypes.h"
#include "PinExpression.h"

class ShiftInOperation : public Operation {
public:
    ShiftInOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
    PinExpression * dataPin;
    PinExpression * clockPin;
    Expression * clockPeriodExpr;
    GPIO_Bit_Order bitOrder;
    
protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
