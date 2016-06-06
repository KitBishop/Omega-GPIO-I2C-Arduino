#include <unistd.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "FileDeleteOperation.h"
#include "Utilities.h"

using namespace std;

FileDeleteOperation::FileDeleteOperation()
    : Operation(opFileOut) {
    fileName = "";
}

bool FileDeleteOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No file name specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    fileName = **paramIter;

    (*paramIter)++;
    
    return true;
}

string FileDeleteOperation::toString() {
    return Operation::toString()
            + " FileName:" + fileName;
}

bool FileDeleteOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    string s = "";
    s = s + "Deleting file:" + fileName;
    appInfo->prtReport(s);
 
    string cmd = "rm -rf '" + fileName + "'";
    appInfo->result = system(cmd.c_str());

    return true;
}

string FileDeleteOperation::help() {
    stringstream hStr;
    hStr << "filedelete <file-name>";
    hStr << "\n\tDeletes the file with given name.";
    hStr << "\n\tAlong with 'filein', and 'fileout' operations and";
    hStr << "\n\t$[<filename>] expression item, provides a rudimentary";
    hStr << "\n\tmechanism for communicating with other programs.";
    hStr << "\n\t<file-name> is the name of the file to delete.";
    hStr << "\n\tMust be enclosed in \" characters if it contains spaces";
    hStr << "\n\tor other special character.";
    
    return hStr.str();
}

bool FileDeleteOperation::writeToFile(string fileName, long int fileVal) {
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
