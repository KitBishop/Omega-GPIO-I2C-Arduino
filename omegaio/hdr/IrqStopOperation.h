#ifndef IrqStopOperation_H
#define IrqStopOperation_H

#include "PinOperation.h"

class IrqStopOperation : public PinOperation {
public:
    IrqStopOperation();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
};

#endif
