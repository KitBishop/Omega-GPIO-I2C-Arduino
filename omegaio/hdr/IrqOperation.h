#ifndef IrqOperation_H
#define IrqOperation_H

#include "PinOperation.h"
#include "GPIOTypes.h"

class IrqOperation : public PinOperation {
public:
    IrqOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();

    GPIO_Irq_Type irqType;
    string cmd;
    Expression * debounceExpr;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
};

#endif
