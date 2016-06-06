#ifndef FileDeleteOperation_H
#define FileDeleteOperation_H

#include "Operation.h"
#include "Expression.h"

class FileDeleteOperation : public Operation {
public:
    FileDeleteOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
    string fileName;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);

private:
    bool writeToFile(string fileName, long int fileVal);
};

#endif
