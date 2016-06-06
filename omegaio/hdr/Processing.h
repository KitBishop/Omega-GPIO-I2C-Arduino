#ifndef Processing_H
#define Processing_H

#include <cstdio>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
//#include <cstdlib>

#include "Node.h"
#include "Option.h"
#include "Operation.h"
#include "AppInfo.h"
#include "Utilities.h"

class Processing {
public:
    static bool processFileParameter(AppInfo * appInfo, list<string> * &paramList, string fileName);
    static bool loadParameters(AppInfo * appInfo, list<string> * &paramList, int argc, char** argv);
    static void printParameters(list<string> &paramList);
    static bool convertParamsToNodes(AppInfo * appInfo, list<Node *> * &nodeList, list<string> * &paramList, list<string>::iterator * &paramIterIn, OperationType parentOpType, OperationType * endOpType);
    static void printNodes(string prefix, list<Node *> &nodeList);
    static bool executeNodes(AppInfo * appInfo, list<Node *> * &nodeList);
};

#endif
