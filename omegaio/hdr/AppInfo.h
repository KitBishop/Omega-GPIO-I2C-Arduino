#ifndef AppInfo_H
#define AppInfo_H

#include <string>
#include <map>

#include "Types.h"
#include "I2CDevice.h"

using namespace std;

#define applicationVersion  "1.0.0"

class AppInfo {
public:
    AppInfo();

    // Options:
    // -v -> implies output=true, report=true, errors=true
    // -q -> implies output=false, report=false, errors=false
    // -o -> true = output results to stdout.  Default is true
    // -r -> true = output operations to stderr.  Default is false
    // -e -> true = output errors to stderr.  Default is true
    // -i -> false = ignore execution errors.  Default is false
    // -s -> true = set pin to required direction.  Default is false
    // -x -> false = output in hex. Default is false
    // -h{=<topic>} -> output help to stderr
    
    bool output;   // -o -> true = output results to stdout.  Default is true
    bool report;   // -r -> true = output operations to stderr.  Default is false
    bool errors;   // -e -> true = output errors to stderr.  Default is true
    bool ignore;   // -i -> false = ignore execution errors.  Default is false
    bool setdir;   // -s -> true = set pin to required direction.  Default is false
    bool hexout;   // -x -> true = output in hex.  Default is false
    
    bool breaking;
    bool debug;
    
    bool haveOp;
    
    long int result;
    
    OperationType errOpType;
    
    string appName;
    
    map<string, long int> varMap;
    map<int, I2CDevice *> i2cDevices;
    
    void prtError(OperationType errOT, string s);
    void prtOutput(unsigned long v);
    void prtOutput(string s);
    void prtReport(string s);
    
    long int getVar(string s);
    I2CDevice * getI2CDevice(int addr);
    void setVar(string s, long int v);
    string replaceVars(string s);
    long int fileExists(string fnm);
};

#endif /* APPINFO_H */
