#ifndef I2CRegExpression_H
#define I2CRegExpression_H

using namespace std;

#include <list>
#include <string>

#include "Expression.h"

class I2CRegExpression : public Expression {
public:
    static I2CRegExpression * create(OperationType parOp, string expStr, AppInfo * appInf, bool allowNone);

    bool getRegister(long int &reg);
    bool getIsNone();

private:
    I2CRegExpression(string expStr, OperationType parOp, AppInfo * appInf);
    
    bool isNone;
};

#endif
