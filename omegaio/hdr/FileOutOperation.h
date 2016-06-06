#ifndef FileOutOperation_H
#define FileOutOperation_H

#include "Operation.h"
#include "Expression.h"

class FileOutOperation : public Operation {
public:
    FileOutOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
    string fileName;
    Expression * expression;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);

private:
    bool writeToFile(string fileName, long int fileVal);
};

#endif
