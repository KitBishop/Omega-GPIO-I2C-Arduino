#ifndef GetDirectionOperation_H
#define GetDirectionOperation_H

#include "PinOperation.h"

class GetDirectionOperation : public PinOperation {
public:
    GetDirectionOperation();

    virtual bool execute(AppInfo * appInfo);

    static string help();
};

#endif
