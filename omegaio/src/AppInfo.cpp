#include <fstream>
#include <iostream>

#include "AppInfo.h"
#include "Utilities.h"
#include "Operation.h"
#include "GPIOAccess.h"

AppInfo::AppInfo() {
    output = true;   // -o -> true = output results to stdout.  Default is true
    report = false;  // -r -> true = output operations to stderr.  Default is false
    errors = true;   // -e -> true = output errors to stderr.  Default is true
    ignore = false;  // -i -> false = ignore execution errors.  Default is false
    setdir = false;  // -s -> true = set pin to required direction.  Default is false
    hexout = false;  // -x -> true = output in hex.  Default is false
    
    breaking = false;
    continuing = false;
    
    debug = false;
    
    haveOp = false;
    
    result = 0;
    
    errOpType = opInvalid;
    
    varMap = map<string, long int>();
    i2cDevices = map<int, I2CDevice *>();
    
    arduinoSystem = NULL;
    arduinoPort = NULL;

}

void AppInfo::prtError(OperationType errOT, string s) {
    if (errOT != opInvalid) {
        errOpType = errOT;
    }
    if (errors) {
        prterr(s);
    }
}

void AppInfo::prtOutput(unsigned long v) {
    if (output) {
        if (hexout) {
            cout << std::hex << "0x";
        } else {
            cout << std::dec;
        }
        cout << v << endl;
    }
}

void AppInfo::prtOutput(string s) {
    if (output) {
        cout << s << endl;
    }
}

void AppInfo::prtReport(string s) {
    if (report) {
        cerr << s << endl;
    }
}

long int AppInfo::getVar(string s) {
    if (s.compare("$?") == 0) {
        if (errOpType != opInvalid) {
            return 0;
        } else {
            return 1;
        }
    }

    if (s.compare("$!") == 0) {
        return result;
    }
    
    if ((s.length() >= 3) && (s.at(0) == '$') && (s.at(1) == '[') && (s.at(s.length() - 1) == ']')) {
        string fnm = s.substr(2, s.length() - 3);
        return fileExists(fnm);
    }
    
    if ((s.length() >= 2) && (s.at(0) == '$') && (s.at(1) >= '0') && (s.at(1) <= '9')) {
        int pin;
        if (getInteger(s.substr(1, string::npos), pin)) {
            if (GPIOAccess::isPinUsable(pin)) {
                int v = 0;
                GPIOAccess::get(pin, v);
                return v;
            }
        }
    } else {
        auto vItem = varMap.find(s);
        if (vItem != varMap.end()) {
            return vItem->second;
        }
    }
    
    return 0;
}

I2CDevice * AppInfo::getI2CDevice(int addr) {
    auto dev = i2cDevices.find(addr);
    if (dev != i2cDevices.end()) {
        return dev->second;
    } else {
        I2CDevice * newDev = new I2CDevice(addr);
        i2cDevices.insert(std::make_pair(addr, newDev));
        return newDev;
    }
}

void AppInfo::setVar(string s, long int v) {
    auto vItem = varMap.find(s);
    if (vItem != varMap.end()) {
        varMap.erase(s);
    }
    varMap.insert(std::make_pair(s, v));
}

string AppInfo::replaceVars(string s) {
    int i;
    string newS = "" + s;
    
    string var = "$?";
    string targ = "{" + var + "}";
    string rep = to_string(getVar(var));
    newS = replaceString(newS, targ, rep);

    var = "$!";
    targ = "{" + var + "}";
    rep = to_string(getVar(var));
    newS = replaceString(newS, targ, rep);
    
    for (i = 0; i < 30; i++) {
        if (GPIOAccess::isPinUsable(i)) {
            var = "$" + to_string(i);
            targ = "{" + var + "}";
            rep = to_string(getVar(var));
            newS = replaceString(newS, targ, rep);
        }
    }
    
    for (auto vi : varMap) {
        var = vi.first;
        targ = "{" + var + "}";
        rep = to_string(getVar(var));
        newS = replaceString(newS, targ, rep);
    }

    int lookStart = 0;
    while (lookStart < (newS.length() - 3)) {
        for (i = lookStart; i < newS.length() - 3 ; i++) {
            if ((newS.at(i) == '{') && 
                (newS.at(i + 1) == '$') &&
                (newS.at(i + 2) == '[')) {
                int j;
                var = "";
                bool done = false;
                for (j = i + 1; j < newS.length() - 1; j++) {
                    if ((newS.at(j) == ']') && (newS.at(j + 1) == '}')) {
                        var = var + "]";
                        done = true;
                        break;
                    } else {
                        var = var + newS.at(j);
                    }
                }
                if (done) {
                    targ = "{" + var + "}";
                    rep = to_string(getVar(var));
                    newS = replaceString(newS, targ, rep);
                } else {
                    lookStart = j;
                }
            } else {
                lookStart = i + 1;
            }
        }
    }
    
    return newS;
}

long int AppInfo::fileExists(string fnm) {
    ifstream myfile;

    myfile.open(fnm);

    if (myfile.is_open()) {
        return 1;
    } else {
        return 0;
    }
}

void AppInfo::setArduinoSystem(int addr, int port) {
    if (arduinoSystem != NULL) {
        delete arduinoSystem;
    }
    
    arduinoSystem = new ArduinoSystem(addr, port);
}

ArduinoSystem * AppInfo::getArduinoSystem() {
    if (arduinoSystem == NULL) {
        arduinoSystem = new ArduinoSystem(0x08, 0);
    }
    return arduinoSystem;
}

void AppInfo::setArduinoPort(int addr, int port) {
    if (arduinoPort != NULL) {
        delete arduinoPort;
    }
    
    arduinoPort = new ArduinoPort(addr, port);
}

ArduinoPort * AppInfo::getArduinoPort() {
    return arduinoPort;
}
