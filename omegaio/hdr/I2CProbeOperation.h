#ifndef I2CProbeOperation_H
#define I2CProbeOperation_H

#include "I2COperation.h"

class I2CProbeOperation : public I2COperation {
public:
    I2CProbeOperation();
    
    virtual bool execute(AppInfo * appInfo);
    
    static string help();
};

#endif
