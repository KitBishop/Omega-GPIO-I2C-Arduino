#ifndef Node_H
#define Node_H

#include "AppInfo.h"

using namespace std;

class Node {
public:
    Node(NodeType t);
    virtual string toString();

    virtual bool execute(AppInfo * appInfo) = 0;
    
    NodeType type;
};

#endif

