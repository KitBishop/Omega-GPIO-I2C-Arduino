#ifndef PinExpression_H
#define PinExpression_H

using namespace std;

#include <list>
#include <string>

#include "Expression.h"

class PinExpression : public Expression {
public:
    static PinExpression * create(OperationType parOp, string expStr, AppInfo * appInf, bool allowAll);

    bool getPinNumber(long int &pinNum);

private:
    PinExpression(string expStr, OperationType parOp, AppInfo * appInf);

    bool isAll;
};

#endif
