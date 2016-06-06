#include <cstdio>
#include <list>
#include <iostream>
#include <string>
#include <cstdlib>

#include "Node.h"
#include "Option.h"
#include "Operation.h"
#include "Utilities.h"
#include "Processing.h"

using namespace std;

int main(int argc, char** argv) {
    auto * paramList = new list<string>();
    auto * nodeList = new list<Node *>();
    auto * appInfo = new AppInfo();
    long int exitResult = 0;
    bool isok = true;
    
    appInfo->appName = argv[0];

    isok = Processing::loadParameters(appInfo, paramList, argc, argv);
    if (isok) {
        if (appInfo->debug) {
            Processing::printParameters(*paramList);
        }

        list<string>::iterator * paramIter = new list<string>::iterator();
        *paramIter = paramList->begin();
        isok = Processing::convertParamsToNodes(appInfo, nodeList, paramList, paramIter, opInvalid, NULL);
        if (appInfo->debug) {
            Processing::printNodes("", *nodeList);
        }
        delete paramList;
        if (isok) {
            if (nodeList->empty() || !appInfo->haveOp) {
                isok = false;
                appInfo->prtError(opInvalid, "No operations supplied");
            } else {
                isok = Processing::executeNodes(appInfo, nodeList);
            }
            delete nodeList;
            if (isok) {
                exitResult = appInfo->result;
            }
        } else {
            delete nodeList;
        }
    } else {
        delete paramList;
        delete nodeList;
    }
    
    if (!isok) {
        cerr << endl;
        cerr << Option::progInfoHelp();
        cerr << "Help is available by using one of:" << endl;
        cerr << "\t" << appInfo->appName << " -?  - for basic usage"  << endl;
        cerr << "\t" << appInfo->appName << " -h  - for full help"  << endl;
        if (appInfo->errOpType != opInvalid) {
            cerr << "\t" << appInfo->appName << " -h:" << Operation::mapFromOpType(appInfo->errOpType) << "  - for help on failed operation" << endl;
        }
        exitResult = -1;
    }
    
    delete appInfo;
    
    exit(exitResult);
}

