#ifndef InfoOperation_H
#define InfoOperation_H

#include "PinOperation.h"

class InfoOperation : public PinOperation {
public:
    InfoOperation();

    virtual bool execute(AppInfo * appInfo);
    
    static string help();
};

#endif 
