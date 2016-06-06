#ifndef ToneStopOperation_H
#define ToneStopOperation_H

#include "PinOperation.h"

class ToneStopOperation : public PinOperation {
public:
    ToneStopOperation();
    
    virtual bool execute(AppInfo * appInfo);
    
    static string help();
};

#endif
