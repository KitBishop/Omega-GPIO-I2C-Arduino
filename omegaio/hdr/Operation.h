#ifndef Operation_H
#define Operation_H

#include <string>
#include <map>
#include <list>

#include "Node.h"

class Operation : public Node {
public:
    Operation(OperationType ot);
    virtual string toString();
    
    virtual bool execute(AppInfo * appInfo) = 0;
  
    OperationType opType;
    
    static Operation * create(AppInfo * appInfo, OperationType ot, list<string> * &paramList, list<string>::iterator * &paramIter);
    static OperationType mapOpType(string ot);
    static string mapFromOpType(OperationType ot);

protected:    
    virtual bool build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) = 0;

private:
    static map<string, OperationType> opTypeMap;
};

#endif
