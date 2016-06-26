#ifndef Option_H
#define Option_H

#include <string>

#include "Node.h"
#include "Operation.h"

enum OptOper {
    on,
    off,
    param
};

class Option : public Node {
public:
    Option(char optC, OptOper op, string * param);
    virtual string toString();

    virtual bool execute(AppInfo * appInfo);

    char optChar;
    OptOper oper;
    string * optParam;

    static Option * create(AppInfo * appInfo, string optStr);
    static string operationHelp(OperationType opType);
    static string optionHelp(char optC);
    static string atHelp();
    static string basicHelpBlock(string head, OperationType op1, OperationType op2);
    static string fullHelpBlock(string head, OperationType op1, OperationType op2);
    static string basicHelp(AppInfo * appInfo);
    static string progInfoHelp(AppInfo * appInfo, string head, bool withVersion);
    static string sourcesHelp();
};

#endif
