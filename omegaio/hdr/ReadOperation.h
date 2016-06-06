#ifndef ReadOperation_H
#define ReadOperation_H

#include "PinOperation.h"

class ReadOperation : public PinOperation {
public:
    ReadOperation();
    
    virtual bool execute(AppInfo * appInfo);
    
    static string help();
};

#endif
