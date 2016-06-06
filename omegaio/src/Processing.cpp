#include <iostream>
#include <string.h>
#include "Processing.h"
#include "WhileOperation.h"
#include "IfOperation.h"

bool Processing::processFileParameter(AppInfo * appInfo, list<string> * &paramList, string fileName) {
    ifstream paramFile;
    int lineNum = 0;
    bool res = true;

    paramFile.open(fileName);

    if (!paramFile.is_open()) {
        prterr("Unable to open parameter file:" + fileName + '\n');
        return false;
    }

    while (!paramFile.eof()) {
        string aLine;
        getline(paramFile, aLine);
        aLine = trim(aLine);
        lineNum++;
        if ((aLine.length() > 0) && (aLine.front() != '#')) {

            string aToken;
            stringstream aLineStream(aLine);

            while (aLineStream >> aToken) {
                if (aToken.front() == '"') {
                    string qToken = string(aToken.substr(1, string::npos));
                    bool qDone = qToken.back() == '"';
                    if (qDone) {
                        paramList->push_back(qToken.substr(0, qToken.length() - 1));
                    } else {
                        while (!qDone && (aLineStream >> aToken)) {
                            if (aToken.back() != '"') {
                                qToken = qToken + ' ' + aToken;
                            } else {
                                if (aToken[aToken.length()- 2] == '\\') {
                                    qToken = qToken + ' ' + aToken.substr(0, aToken.length() - 1) + '"';
                                } else {
                                    qToken = qToken + ' ' + aToken.substr(0, aToken.length() - 1);
                                    paramList->push_back(qToken);
                                    qDone = true;
                                }
                            }
                        }
                    }
                    if (!qDone) {
                        prterr("Unterminated quoted parameter in parameter file:" + fileName + " Line number:" + to_string(lineNum) + '\n');
                        res = false;
                    }
                } else {
                    if (aToken.compare("-d") == 0) {
                        appInfo->debug = true;
                    }
                    paramList->push_back(aToken);
                }
            }
        }
    }

    paramFile.close();

    return res;
}

bool Processing::loadParameters(AppInfo * appInfo, list<string> * &paramList, int argc, char** argv) {
    int i;
    bool ok = true;

    for (i = 1; (i < argc) && ok; i++) {
        if (argv[i][0] == '@') {
            string fnm = string(argv[i]).substr(1, string::npos);
            ok = processFileParameter(appInfo, paramList, fnm);
        } else {
            if (strcmp(argv[i], "-d") == 0) {
                appInfo->debug = true;
            }
            paramList->push_back(argv[i]);
        }
    }

    return ok;
}

void Processing::printParameters(list<string> &paramList) {
    int n = 0;
    cerr << endl << "Input data supplied:" << endl;
    for (auto it : paramList) {
        cerr << n << ":[" << it << ']' << endl;
        n++;
    }
}

bool Processing::convertParamsToNodes(AppInfo * appInfo, list<Node *> * &nodeList, list<string> * &paramList, list<string>::iterator * &paramIter, OperationType parentOpType, OperationType * endOpType) {
    bool ok = true;
    bool done = false;

    while ((*paramIter != paramList->end()) && ok && !done) {
        done = false;
        if ((*paramIter)->front() == '-') {
            if ((*paramIter)->compare("-d") != 0) {
                Option * opt = Option::create(appInfo, **paramIter);
                if (opt != NULL) {
                    nodeList->push_back(opt);
                    (*paramIter)++;
                } else {
                    ok = false;
                }
            } else {
                (*paramIter)++;
            }
        } else {
            OperationType ot = Operation::mapOpType(**paramIter);
            // Special case opExpression - it's not really and operation
            if (ot == opInvalid) {
                appInfo->prtError(ot, "Invalid operation type:" + **paramIter);
                ok = false;
            } else {
                switch (ot) {
                    case opEndWhile:
                        if (parentOpType != opWhile) {
                            appInfo->prtError(opWhile, "Unexpected operation type:" + Operation::mapFromOpType(ot));
                            ok = false;
                        } else {
                            if (endOpType != NULL) {
                                *endOpType = opEndWhile;
                            }
                        }
                        done = true;
                        break;

                    case opElse:
                        if (parentOpType != opIf) {
                            appInfo->prtError(opIf, "Unexpected operation type:" + Operation::mapFromOpType(ot));
                            ok = false;
                        } else {
                            if (endOpType != NULL) {
                                *endOpType = opElse;
                            }
                        }
                        done = true;
                        break;

                    case opEndIf:
                        if ((parentOpType != opIf) && (parentOpType != opElse)) {
                            appInfo->prtError(opIf, "Unexpected operation type:" + Operation::mapFromOpType(ot));
                            ok = false;
                        } else {
                            if (endOpType != NULL) {
                                *endOpType = opEndIf;
                            }
                        }
                        done = true;
                        break;

                    default:
                        break;
                }
                (*paramIter)++;
                if (ok && !done) {
                    Operation * op = Operation::create(appInfo, ot, paramList, paramIter);
                    if (op != NULL) {
                        nodeList->push_back(op);
                        appInfo->haveOp = true;
                    } else {
                        ok = false;
                    }
                }
            }
        }
    }

    if (ok && (*paramIter == paramList->end()) && (parentOpType != opInvalid) && !done) {
        appInfo->prtError(parentOpType, "Unterminated '" + Operation::mapFromOpType(parentOpType) + "'");
        ok = false;
    }
    
    return ok;
}

void Processing::printNodes(string prefix, list<Node *> &nodeList) {
    if (prefix.length() == 0) {
        cerr << endl << "Processed option and operation data:" << endl;
    }
    for (auto nd : nodeList) {
        cerr << prefix << nd->toString() << endl;

        if (nd->type == nodeTypeOperation) {
            if (((Operation *)nd)->opType == opWhile) {
                cerr << prefix + "While body:" << endl;
                printNodes(prefix + "    ", *((WhileOperation *)nd)->body);
            } else if (((Operation *)nd)->opType == opIf) {
                if (((IfOperation *)nd)->thenBody != NULL) {
                    cerr << prefix + "If then body:" << endl;
                    printNodes(prefix + "    ", *((IfOperation *)nd)->thenBody);
                }
                if (((IfOperation *)nd)->elseBody != NULL) {
                    cerr << prefix + "If else body:" << endl;
                    printNodes(prefix + "    ", *((IfOperation *)nd)->elseBody);
                }
            }
        }
    }
}

bool Processing::executeNodes(AppInfo * appInfo, list<Node *> * &nodeList) {
    bool ok = true;
    for (auto nd : *nodeList) {
        if (!nd->execute(appInfo) && !appInfo->ignore) {
            ok = false;
            break;
        }
    }
    
    return ok;
}
