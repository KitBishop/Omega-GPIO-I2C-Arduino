#ifndef PwmStopOperation_H
#define PwmStopOperation_H

#include "PinOperation.h"

class PwmStopOperation : public PinOperation {
public:
    PwmStopOperation();
    
    virtual bool execute(AppInfo * appInfo);
    
    static string help();
};

#endif
