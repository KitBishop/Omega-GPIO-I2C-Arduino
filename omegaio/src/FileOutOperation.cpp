#include <unistd.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "FileOutOperation.h"
#include "Utilities.h"

using namespace std;

FileOutOperation::FileOutOperation()
    : Operation(opFileOut) {
    fileName = "";
    expression = NULL;
}

bool FileOutOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No file name specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    fileName = **paramIter;

    (*paramIter)++;

    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No expression specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    expression = Expression::create(opType, **paramIter, appInfo);
    
    if (expression == NULL) {
        return false;
    }

    (*paramIter)++;

    return true;
}

string FileOutOperation::toString() {
    return Operation::toString()
            + " FileName:" + fileName
            + " Expression:" + expression->getExpressionString();
}

bool FileOutOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    string s = "";
    s = s + "Writing expression:'" + expression->getExpressionString() + "' to file:" + fileName;
    appInfo->prtReport(s);
    
    bool isOk = true;
    long int exprVal;
    if (expression->eval(exprVal)) {
        isOk = writeToFile(fileName, exprVal);
    } else {
        isOk = false;
    }
    
    if (!isOk) {
        appInfo->prtError(opType, "Error writing expression to file:" + expression->getExpressionString() + "->" + to_string(exprVal));
    }
    
    return isOk;
}

string FileOutOperation::help() {
    stringstream hStr;
    hStr << "fileout <file-name> <expression>";
    hStr << "\n\tWrites the value of the expression as the first line";
    hStr << "\n\tof given file.";
    hStr << "\n\tAlong with 'filein', and 'filedelete' operations and";
    hStr << "\n\t$[<filename>] expression item, provides a rudimentary";
    hStr << "\n\tmechanism for communicating with other programs.";
    hStr << "\n\t<file-name> is the name of the file to write to.";
    hStr << "\n\tMust be enclosed in \" characters if it contains spaces";
    hStr << "\n\tor other special character.";
    hStr << "\n\t<expression> is the expression value to write.";
    hStr << "\n\tUse the help parameter -h:expression for help on expressions.";
    
    return hStr.str();
}

bool FileOutOperation::writeToFile(string fileName, long int fileVal) {
    ofstream myfile;

    myfile.open(fileName);
    
    if (myfile.is_open()) {
        myfile << fileVal;
        myfile << "\n";

        myfile.close();
        
        return true;
    }
    
    return false;
}
