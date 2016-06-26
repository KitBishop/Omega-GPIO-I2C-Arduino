#include <stack>
#include <iostream>

#include "Expression.h"
#include "Utilities.h"
#include "Operation.h"

Expression::Expression(string expStr, OperationType parOp, AppInfo * appInf) {
    appInfo = appInf;
    parentOp = parOp;
    rpnList = list<ExprToken>();
    expressionString = expStr;
    expError = false;
    errDone = false;
}

string Expression::getExpressionString() {
    return expressionString;
}

void Expression::expressionError(string txt, int off) {
    appInfo->prtError(parentOp, txt + "'" + expressionString + "' at offset:" + to_string(off) + " for parameter for '" + Operation::mapFromOpType(parentOp) + "'");
}

Expression * Expression::create(OperationType parOp, string expStr, AppInfo * appInf) {
    Expression * exp = new Expression(expStr, parOp, appInf);

    int initialOffset = 0;

    if (exp->expr(initialOffset, '\0')) {
        if(exp->expError) {
            delete exp;
            return NULL;
        } else if (initialOffset != expStr.length()) {
            if (!exp->errDone) {
                exp->expressionError("Error in expression:", initialOffset);
            }
            delete exp;
            return NULL;
        }
        return exp;
    } else {
        delete exp;
        return NULL;
    }
}

bool Expression::eval(long int &val) {
    val = 0;

    if (expError) {
        expressionError("Attempting to evaluate invalid expression:", 0);
        return false;
    }
    
    stack<long int> valStack = stack<long int>();

    for (auto tk : rpnList) {
        if (tk.type == ttOperand) {
            if (tk.operandCategory == ocNum) {
                valStack.push(tk.numberVal);
            } else if (tk.operandCategory == ocVar) {
                long int v = appInfo->getVar(tk.variableName);
                valStack.push(v);
            } else /* ocFile */ {
                long int v = appInfo->fileExists(tk.variableName);
                valStack.push(v);
            }
        } else {
            if(tk.operatorCategory == ocUnaryOp) {
                if (valStack.empty()) {
                    expressionError("Stack error evaluating expression:", tk.offset);
                    return false;
                }
                long int v = valStack.top();
                valStack.pop();
                switch (tk.operatorname) {
                    case onUPlus:
                        valStack.push(v);
                        break;
                    case onUMinus:
                        valStack.push(-v);
                        break;
                    case onLogNot:
                        valStack.push(v == 0 ? 1 : 0);
                        break;
                    case onBitNot:
                        valStack.push(~v);
                        break;
                }
            } else {
                if (valStack.empty()) {
                    expressionError("Stack empty while evaluating expression:", tk.offset);
                    return false;
                }
                long int v2 = valStack.top();
                valStack.pop();
                if (valStack.empty()) {
                    expressionError("Stack empty while evaluating expression:", tk.offset);
                    return false;
                }
                long int v1 = valStack.top();
                valStack.pop();
                switch (tk.operatorname) {
                    case onMul:
                        valStack.push(v1 * v2);
                        break;
                    case onDiv:
                        if (v2 == 0) {
                            expressionError("Divide by zero evaluating expression:", tk.offset);
                            return false;
                        }
                        valStack.push(v1 / v2);
                        break;
                    case onRem:
                        if (v2 == 0) {
                            expressionError("Divide by zero evaluating expression:", tk.offset);
                            return false;
                        }
                        valStack.push(v1 % v2);
                        break;
                    case onAdd:
                        valStack.push(v1 + v2);
                        break;
                    case onSub:
                        valStack.push(v1 - v2);
                        break;
                    case onShiftR:
                        valStack.push(v1 >> v2);
                        break;
                    case onShiftL:
                        valStack.push(v1 << v2);
                        break;
                    case onLT:
                        valStack.push((v1 < v2) ? 1 : 0);
                        break;
                    case onGT:
                        valStack.push((v1 > v2) ? 1 : 0);
                        break;
                    case onLTE:
                        valStack.push((v1 <= v2) ? 1 : 0);
                        break;
                    case onGTE:
                        valStack.push((v1 >= v2) ? 1 : 0);
                        break;
                    case onEQ:
                        valStack.push((v1 == v2) ? 1 : 0);
                        break;
                    case onNE:
                        valStack.push((v1 != v2) ? 1 : 0);
                        break;
                    case onBitAnd:
                        valStack.push(v1 & v2);
                        break;
                    case onBitOr:
                        valStack.push(v1 | v2);
                        break;
                    case onBitXor:
                        valStack.push(v1 ^ v2);
                        break;
                    case onLogAnd:
                        valStack.push(((v1 != 0) && (v2 != 0)) ? 1 : 0);
                        break;
                    case onLogOr:
                        valStack.push(((v1 != 0) || (v2 != 0)) ? 1 : 0);
                        break;
                }
            }
        }
    }
    
    if (valStack.size() != 1) {
        expressionError("Stack error evaluating expression:", expressionString.length());
        return false;
    }
    
    val = valStack.top();
    return true;
}

bool Expression::expr(int &offset, char termChar) {
    while ((offset < expressionString.length()) && (expressionString.at(offset) == ' ')) {
        offset++;
    }
    if ((offset >= expressionString.length() || expError)) {
        return false;
    }
    if (term(offset)) {
        bool isOk = true;

        while (isOk) {
            ExprToken addOpTk;
            if (addop(addOpTk, offset)) {
                if (term(offset)) {
                    rpnList.push_back(addOpTk);
                } else {
                    isOk = false;
                }
            } else {
                isOk = false;
            }
        }

        if ((termChar == '\0') && (offset >= expressionString.length())) {
            return true;
        }

        if ((termChar == ')') && (offset < expressionString.length()) && (expressionString.at(offset) == ')')) {
            offset++;
            return true;
        }

        if (!errDone) {
            expressionError("Error in expression:", offset);
        }
        errDone = true;
        expError = true;
        
        return false;
    }

    return false;
}

bool Expression::addop(ExprToken &addOpTk, int &offset) {
    while ((offset < expressionString.length()) && (expressionString.at(offset) == ' ')) {
        offset++;
    }
    if ((offset >= expressionString.length() || expError)) {
        return false;
    }

    int origOff = offset;

    addOpTk.type = ttOperator;
    addOpTk.operatorCategory = ocAddOp;
    addOpTk.offset = origOff;

    bool isOk = true;

    char c1 = expressionString.at(offset);
    char c2;
    switch (c1) {
        case '+':
            offset++;
            addOpTk.operatorname = onAdd;
            break;

        case '-':
            offset++;
            addOpTk.operatorname = onSub;
            break;

        case '>':
            offset++;
            if (offset >= expressionString.length()) {
                isOk = false;
            } else {
                c2 = expressionString.at(offset);            
                switch (c2) {
                    case '=':
                        offset++;
                        addOpTk.operatorname = onGTE;
                        break;

                    default:
                        addOpTk.operatorname = onGT;
                }
            }
            break;

        case '<':
            offset++;
            if (offset >= expressionString.length()) {
                isOk = false;
            } else {
                c2 = expressionString.at(offset);            
                switch (c2) {
                    case '=':
                        offset++;
                        addOpTk.operatorname = onLTE;
                        break;

                    default:
                        addOpTk.operatorname = onLT;
                }
            }
            break;

        case '&':
            offset++;
            if (offset >= expressionString.length()) {
                isOk = false;
            } else {
                c2 = expressionString.at(offset);            
                switch (c2) {
                    case '&':
                        offset++;
                        addOpTk.operatorname = onLogAnd;
                        break;

                    default:
                        addOpTk.operatorname = onBitAnd;
                }
            }
            break;

        case '|':
            offset++;
            if (offset >= expressionString.length()) {
                isOk = false;
            } else {
                c2 = expressionString.at(offset);            
                switch (c2) {
                    case '|':
                        offset++;
                        addOpTk.operatorname = onLogOr;
                        break;

                    default:
                        addOpTk.operatorname = onBitOr;
                }
            }
            break;

        case '^':
            offset++;
            addOpTk.operatorname = onBitXor;
            break;

        case '=':
            offset++;
            if (offset >= expressionString.length()) {
                isOk = false;
            } else {
                c2 = expressionString.at(offset);            
                if (c2 == '=') {
                    offset++;
                    addOpTk.operatorname = onEQ;
                } else {
                    isOk = false;
                }
            }
            break;
            
        case '!':
            offset++;
            if (offset >= expressionString.length()) {
                isOk = false;
            } else {
                c2 = expressionString.at(offset);            
                if (c2 == '=') {
                    offset++;
                    addOpTk.operatorname = onNE;
                } else {
                    isOk = false;
                }
            }
            break;
            
        default:
            isOk = false;
            break;
    }

    if (!isOk) {
        offset = origOff;
    }
    return isOk;
}

bool Expression::term(int &offset) {
    while ((offset < expressionString.length()) && (expressionString.at(offset) == ' ')) {
        offset++;
    }
    if ((offset >= expressionString.length() || expError)) {
        return false;
    }
    if (factor(offset)) {

        bool isOk = true;

        while (isOk) {
            ExprToken mulOpTk;
            if (mulop(mulOpTk, offset)) {
                if (factor(offset)) {
                    rpnList.push_back(mulOpTk);
                } else {
                    isOk = false;
                }
            } else {
                isOk = false;
            }
        }

        return true;
    }

    return false;
}

bool Expression::mulop(ExprToken &mulOpTk, int &offset) {
    while ((offset < expressionString.length()) && (expressionString.at(offset) == ' ')) {
        offset++;
    }
    if ((offset >= expressionString.length() || expError)) {
        return false;
    }

    int origOff = offset;

    mulOpTk.type = ttOperator;
    mulOpTk.operatorCategory = ocMulOp;
    mulOpTk.offset = origOff;

    bool isOk = true;

    char c1 = expressionString.at(offset);
    char c2;
    switch (c1) {
        case '*':
            offset++;
            mulOpTk.operatorname = onMul;
            break;

        case '/':
            offset++;
            mulOpTk.operatorname = onDiv;
            break;

        case '%':
            offset++;
            mulOpTk.operatorname = onRem;
            break;

        case '<':
            offset++;
            if (offset < expressionString.length()) {
                c2 = expressionString.at(offset);
                if (c2 == '<') {
                    offset++;
                    mulOpTk.operatorname = onShiftL;
                } else {
                    isOk = false;
                }
            } else {
                isOk = false;
            }
            break;

        case '>':
            offset++;
            if (offset < expressionString.length()) {
                c2 = expressionString.at(offset);
                if (c2 == '>') {
                    offset++;
                    mulOpTk.operatorname = onShiftR;
                } else {
                    isOk = false;
                }
            } else {
                isOk = false;
            }
            break;

        default:
            isOk = false;
            break;
    }

    if (!isOk) {
        offset = origOff;
    }
    
    return isOk;
}

bool Expression::factor(int &offset) {
    while ((offset < expressionString.length()) && (expressionString.at(offset) == ' ')) {
        offset++;
    }
    if ((offset >= expressionString.length() || expError)) {
        return false;
    }
    ExprToken unOpTk;
    bool haveUnOp = false;
    
    if (unaryop(unOpTk, offset)) {
        haveUnOp = true;
    }
    
    if (baseitem(offset)) {
        if (haveUnOp) {
            rpnList.push_back(unOpTk);
        }
        return true;
    }
    return false;
}

bool Expression::unaryop(ExprToken &unOpTk, int &offset) {
    while ((offset < expressionString.length()) && (expressionString.at(offset) == ' ')) {
        offset++;
    }
    if ((offset >= expressionString.length() || expError)) {
        return false;
    }

    int origOff = offset;

    unOpTk.type = ttOperator;
    unOpTk.operatorCategory = ocUnaryOp;
    unOpTk.offset = origOff;

    bool isOk = true;

    char c1 = expressionString.at(offset);

    switch (c1) {
        case '+':
            offset++;
            unOpTk.operatorname = onUPlus;
            break;

        case '-':
            offset++;
            unOpTk.operatorname = onUMinus;
            break;

        case '!':
            offset++;
            unOpTk.operatorname = onLogNot;
            break;

        case '~':
            offset++;
            unOpTk.operatorname = onBitNot;
            break;

        default:
            isOk = false;
            break;
    }

    if (!isOk) {
        offset = origOff;
    }
    return isOk;
}

bool Expression::baseitem(int &offset) {
    while ((offset < expressionString.length()) && (expressionString.at(offset) == ' ')) {
        offset++;
    }
    if ((offset >= expressionString.length() || expError)) {
        expError = true;
        return false;
    }

    int origOffset = offset;
    
    bool isOk = true;
    char c1 = expressionString.at(offset);
    if (c1 == '(') {
        offset++;
        return expr(offset, ')');
    } else if ((c1 >= '0') && (c1 <= '9')) {
        long int v = 0;
        if ((c1 == '0') && (offset < expressionString.length() -1) && 
                ((expressionString.at(offset+1) == 'x') || (expressionString.at(offset+1) == 'X'))) {
            offset += 2;
            bool done = false;
            while ((offset < expressionString.length()) && !done) {
                char c2 = expressionString.at(offset);
                if ((c2 >= '0') && (c2 <= '9')) {
                    v = (v * 16) + (c2 - '0');
                    offset++;
                } else if ((c2 >= 'a') && (c2 <= 'f')) {
                    v = (v * 16) + (c2 - 'a') + 10;
                    offset++;
                } else if ((c2 >= 'A') && (c2 <= 'F')) {
                    v = (v * 16) + (c2 - 'A') + 10;
                    offset++;
                } else {
                    done = true;
                }
            }
        } else {
            v = c1 - '0';
            offset++;
            bool done = false;
            while ((offset < expressionString.length()) && !done) {
                char c2 = expressionString.at(offset);
                if ((c2 >= '0') && (c2 <= '9')) {
                    v = (v * 10) +(c2 - '0');
                    offset++;
                } else {
                    done = true;
                }
            }
        }
        ExprToken numTk;
        numTk.type = ttOperand;
        numTk.offset = origOffset;
        numTk.operandCategory = ocNum;
        numTk.numberVal = v;
        rpnList.push_back(numTk);
    } else if (c1 == '$') {
        offset++;
        if (offset < expressionString.length()) {
            char c2 = expressionString.at(offset);
            if ((c2 == '!') || (c2 == '?')) {
                offset++;
                string onm = "";
                onm = onm + c1 + c2;
                ExprToken varTk;
                varTk.type = ttOperand;
                varTk.offset = origOffset;
                varTk.operandCategory = ocVar;
                varTk.variableName = onm;
                rpnList.push_back(varTk);
            } else if ((c2 >= '0') && (c2 <= '9')) {
                long int v = c2 - '0';
                offset++;
                bool done = false;
                while ((offset < expressionString.length()) && !done) {
                    char c3 = expressionString.at(offset);
                    if ((c3 >= '0') && (c3 <= '9')) {
                        v = (v * 10) +(c3 - '0');
                        offset++;
                    } else {
                        done = true;
                    }
                }
                string onm = "";
                onm = onm + c1 + to_string(v);
                ExprToken varTk;
                varTk.type = ttOperand;
                varTk.offset = origOffset;
                varTk.operandCategory = ocVar;
                varTk.variableName = onm;
                rpnList.push_back(varTk);
            } else if (c2 == '[') {
                string onm= "";
                offset++;
                bool done = false;
                while ((offset < expressionString.length()) && !done) {
                    char c3 = expressionString.at(offset);
                    if (c3 == ']') {
                        done = true;
                    } else {
                        onm = onm + c3;
                    }
                    offset++;
                }
                if (!done) {
                    isOk = false;
                } else {
                    ExprToken varTk;
                    varTk.type = ttOperand;
                    varTk.offset = origOffset;
                    varTk.operandCategory = ocFile;
                    varTk.variableName = onm;
                    rpnList.push_back(varTk);
                }
            } else {
                isOk = false;
            }
        } else {
            isOk = false;
        }
    } else if (((c1 >= 'a') && (c1 <= 'z')) || ((c1 >= 'A') && (c1 <= 'Z'))) {
        string onm = "";
        onm = onm + c1;
        offset++;
        bool done = false;
        while ((offset < expressionString.length()) && !done) {
            char c2 = expressionString.at(offset);
            if (((c2 >= 'a') && (c2 <= 'z')) || ((c2 >= 'A') && (c2 <= 'Z')) || ((c2 >= '0') && (c2 <= '9'))) {
                onm = onm + c2;
                offset++;
            } else {
                done = true;
            }
        }
        ExprToken varTk;
        varTk.type = ttOperand;
        varTk.offset = origOffset;
        varTk.operandCategory = ocVar;
        varTk.variableName = onm;
        rpnList.push_back(varTk);
    } else {
        isOk =false;
    } 
    if (!isOk) {
        if (!errDone) {
            expressionError("Error in expression:", offset);
        }
        errDone = true;
        expError = true;
    }
    
    return isOk;
}

string Expression::help() {
    stringstream hStr;
    hStr << "An expression is represented by an <expression_string>";
    hStr << "\n\tAn expression can be used in a variety of places.";
    hStr << "\n\tThe value of an expression is the result of evaluation";
    hStr << "\n\tof the expression string at run time.";
    hStr << "\n\t<expression-string> must be enclosed in \" characters";
    hStr << "\n\tif it contains spaces or other special character.";
    hStr << "\n\tFormulated as a standard integer expression using:";
    hStr << "\n\t- Parentheses: ( and )";
    hStr << "\n\t- Unary Operators: + - ! ~";
    hStr << "\n\t- Binary Operators:";
    hStr << "\n\t    Multiplication: * / % >> <<";
    hStr << "\n\t    Addition: + - & | ^ && ||";
    hStr << "\n\t    Comparison: > >= < <= == !=";
    hStr << "\n\t- Integer constant values, decimal or";
    hStr << "\n\t  hex (starting with '0x' or '0X')";
    hStr << "\n\t- <varref> which will be replaced by actual value at run time.";
    hStr << "\n\t  A <varref> is one of:";
    hStr << "\n\t  - <variable-name> = current value of variable";
    hStr << "\n\t    as assigned in an 'assign' operation ";
    hStr << "\n\t    or 0 if variable has never been assigned";
    hStr << "\n\t  - $n or $nn = current value of pin n or nn";
    hStr << "\n\t  - $! = value of latest result set by earlier operations";
    hStr << "\n\t    The operations that set the result are:";
    hStr << "\n\t      read          get-direction pulsein   shiftin   frequency";
    hStr << "\n\t      filein        filedelete";
    hStr << "\n\t      i2cprobe      i2cread8      i2cread16 i2cread32";
    hStr << "\n\t      asdigitalread asanalogread  asshiftin aspulsein";
    hStr << "\n\t      apgetstatus   apget8        apget16   apget32";
    hStr << "\n\t      exec          assign";
    
    hStr << "\n\t  - $? = status of last executed operation: 0=error, 1=ok";
    hStr << "\n\t  - $[<file-name>] = file <file-name> exists; 0=no, 1=yes";

    hStr << "\n";
    hStr << "\n\tWhen an expression is used as a condition (as in the 'while'";
    hStr << "\n\tand 'if' operations), a zero value represents 'false' and a";
    hStr << "\n\tnon-zero value represents 'true'.";
    
    return hStr.str();
}

/*
expr --> term { addop term }*
term --> factor { mulop factor }*
factor --> unaryop baseitem | baseitem
baseitem --> number | variable | ( expr )
number --> digit number | digit
digit --> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
variable --> { variableselector }
variableselector --> lower case letter a-z | upper case letter A-Z | ? | !
        
Operators higher in the chart have a higher precedence, meaning that the C compiler evaluates them first. Operators on the same line in the chart have the same precedence, and the "Associativity" column on the right gives their evaluation order.

Operator Precedence Chart
Operator Type       Precedence  Operator    Desc            Associativity   Category    Name
=============       ==========  ========    ====            =============   ========    ====
Unary Operators         1       +           unary plus      right-to-left   unaryop     uplus
                                -           unary minus                                 uminus
                                !           logical NOT                                 lognot
                                ~           bitwise NOT                                 bitnot
Binary Operators        2       *           multiplication  left-to-right   mulop       mul
                                /           division                                    div
                                %           remainder                                   rem
                        3       +           addition                        addop       add
                                -           subtraction                                 sub
                        4       >>          bit shift right                 shiftop     shiftr
                                <<          bit shift left                              shiftl
                        5       <           less than                       relop       lt
                                >           greater than                                gt
                                <=          less than or equal                          lte
                                >=          greater than or equal                       gte
                                ==          equal                                       eq
                                !=          not equal                                   ne
                        6       &           bitwise AND                     logop       bitand
                                ^           bitwise XOR                                 bitxor
                                |           bitwise OR                                  bitor
                                &&          logical AND                                 logand
                                ||          logical OR                                  logor
 */

/*
expr --> expr + term | term
term --> term * factor | factor
factor --> - ( expr ) | ( expr ) | signed-number
signed-number --> - number | number
number --> number digit | digit
digit --> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
 */
