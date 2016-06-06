#ifndef I2CAddrExpression_H
#define I2CAddrExpression_H

using namespace std;

#include <list>
#include <string>

#include "Expression.h"

class I2CAddrExpression : public Expression {
public:
    static I2CAddrExpression * create(OperationType parOp, string expStr, AppInfo * appInf);

    bool getAddress(long int &address);

private:
    I2CAddrExpression(string expStr, OperationType parOp, AppInfo * appInf);
};

#endif
