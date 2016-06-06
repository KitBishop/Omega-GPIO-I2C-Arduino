#ifndef SetOutputOperation_H
#define SetOutputOperation_H

#include "PinOperation.h"

class SetOutputOperation : public PinOperation {
public:
    SetOutputOperation();

    virtual bool execute(AppInfo * appInfo);

    static string help();
};

#endif
