#ifndef SetInputOperation_H
#define SetInputOperation_H

#include "PinOperation.h"

class SetInputOperation : public PinOperation {
public:
    SetInputOperation();

    virtual bool execute(AppInfo * appInfo);

    static string help();
};

#endif
