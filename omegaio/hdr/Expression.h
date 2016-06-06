#ifndef Expression_H
#define Expression_H

using namespace std;

#include <list>
#include <string>

#include "AppInfo.h"

enum TokenType {
    ttOperator,
    ttOperand
};

enum OperatorCategory {
    ocUnaryOp,
    ocMulOp,
    ocAddOp
};

enum OperatorName {
    // ocUnaryOp
    onUPlus,
    onUMinus,
    onLogNot,
    onBitNot,
    
    // ocMulOp
    onMul,
    onDiv,
    onRem,
    
    // ocAddOp
    onAdd,
    onSub,
    onShiftR,
    onShiftL,
    onLT,
    onGT,
    onLTE,
    onGTE,
    onEQ,
    onNE,
    onBitAnd,
    onBitOr,
    onBitXor,
    onLogAnd,
    onLogOr
};

enum OperandCategory {
    ocNum,
    ocVar,
    ocFile
};

typedef struct ExprToken{
    TokenType type;
    OperatorCategory operatorCategory;
    OperatorName operatorname;
    OperandCategory operandCategory;
    long int numberVal;
    string variableName;
    int offset;
} ExprToken;

class Expression {
public:
    static Expression * create(OperationType parOp, string expStr, AppInfo * appInf);

    bool eval(long int &val);

    static string help();
    
    string getExpressionString();
    
protected:
    Expression(string expStr, OperationType parOp, AppInfo * appInf);
    
    bool expr(int &offset, char termChar);

    AppInfo * appInfo;
    OperationType parentOp;
    bool expError;
    bool errDone;

private:
    bool term(int &offset);
    bool factor(int &offset);
    bool baseitem(int &offset);
    bool addop(ExprToken &addOpTk, int &offset);
    bool mulop(ExprToken &mulOpTk, int &offset);
    bool unaryop(ExprToken &unOpTk, int &offset);

    void expressionError(string txt, int off);
    
    list<ExprToken> rpnList;
    string expressionString;
};

#endif
