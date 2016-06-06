#ifndef FileInOperation_H
#define FileInOperation_H

#include "Operation.h"

class FileInOperation : public Operation {
public:
    FileInOperation();
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo);

    static string help();
    
    string assignName;
    string fileName;

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter);
    
private:    
    bool readFromFile(string fileName, long int &fileVal);
};

#endif
