#include <unistd.h>
#include <string>

#include "ExpledOperation.h"
#include "Utilities.h"
#include "RGBLED.h"
#include "ForkAccess.h"
#include "TimeHelper.h"

using namespace std;

ExpledOperation::ExpledOperation()
    : Operation(opExpled) {
    redExpr = NULL;
    greenExpr = NULL;
    blueExpr = NULL;
    allExpr = NULL;
}

bool ExpledOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (*paramIter == paramList->end()) {
        appInfo->prtError(opType, "No value specified for '" + mapFromOpType(opType) + "'");
        return false;
    }

    if ((*paramIter)->compare("rgb") != 0) {
        allExpr = Expression::create(opType, **paramIter, appInfo);

        if (allExpr == NULL) {
            return false;
        }

        (*paramIter)++;
    } else {
        (*paramIter)++;
        
        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No red value specified for '" + mapFromOpType(opType) + "' rgb");
            return false;
        }
        redExpr = Expression::create(opType, **paramIter, appInfo);

        if (redExpr == NULL) {
            appInfo->prtError(opType, "Invalid red value specified for '" + mapFromOpType(opType) + "' :" + **paramIter);
            return false;
        }

        (*paramIter)++;
        
        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No green value specified for '" + mapFromOpType(opType) + "' rgb");
            return false;
        }
        greenExpr = Expression::create(opType, **paramIter, appInfo);

        if (greenExpr == NULL) {
            appInfo->prtError(opType, "Invalid green value specified for '" + mapFromOpType(opType) + "' :" + **paramIter);
            return false;
        }

        (*paramIter)++;
        
        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No blue value specified for '" + mapFromOpType(opType) + "' rgb");
            return false;
        }
        blueExpr = Expression::create(opType, **paramIter, appInfo);

        if (blueExpr == NULL) {
            appInfo->prtError(opType, "Invalid blue value specified for '" + mapFromOpType(opType) + "' :" + **paramIter);
            return false;
        }

        (*paramIter)++;
    }

    return true;
}

string ExpledOperation::toString() {
    if (allExpr != NULL) {
        return Operation::toString()
                + " Red,Green,Blue:" + allExpr->getExpressionString();
    } else {
        return Operation::toString()
                + " Red:" + redExpr->getExpressionString()
                + " Green:" + greenExpr->getExpressionString()
                + " Blue:" + blueExpr->getExpressionString();
    }
}

bool ExpledOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    ForkAccess::stop(15);
    ForkAccess::stop(16);
    ForkAccess::stop(17);

    long int redValue;
    long int greenValue;
    long int blueValue;
    if (allExpr != NULL) {
        long int allValue;
    
        if (!allExpr->eval(allValue)) {
            return false;
        }
        
        if ((allValue < 0) || (allValue > 0xffffff)) {
            appInfo->prtError(opType, "Invalid value for '" + mapFromOpType(opType) + "':" + allExpr->getExpressionString() + "->" + to_string(allValue));
            return false;
        } else {
            redValue = (allValue >> 16) & 0xff;
            greenValue = (allValue >> 8) & 0xff;
            blueValue = allValue & 0xff;
            redValue = (redValue * 100) / 255;
            greenValue = (greenValue * 100) / 255;
            blueValue = (blueValue * 100) / 255;
        }
    } else {
        if (!redExpr->eval(redValue)) {
            return false;
        };
        if ((redValue < 0) || (redValue > 100)) {
            appInfo->prtError(opType, "Invalid red value for '" + mapFromOpType(opType) + "':" + redExpr->getExpressionString() + "->" + to_string(redValue));
            return false;
        }
        
        if (!greenExpr->eval(greenValue)) {
            return false;
        };
        if ((greenValue < 0) || (greenValue > 100)) {
            appInfo->prtError(opType, "Invalid green value for '" + mapFromOpType(opType) + "':" + greenExpr->getExpressionString() + "->" + to_string(greenValue));
            return false;
        }
        
        if (!blueExpr->eval(blueValue)) {
            return false;
        };
        if ((blueValue < 0) || (blueValue > 100)) {
            appInfo->prtError(opType, "Invalid blue value for '" + mapFromOpType(opType) + "':" + blueExpr->getExpressionString() + "->" + to_string(blueValue));
            return false;
        }
    }
    
    // Continuous Expled output requires a separate process
    pid_t pid = fork();

    if (pid == 0) {
        // child process, run the pwm
        RGBLED * led = new RGBLED();

        string s = "";
        
        appInfo->prtReport(s + "Starting Expled output:" +
            + " Red:" + to_string(redValue)
            + " Green:" + to_string(greenValue)
            + " Blue:" + to_string(blueValue));

        led->setColor(redValue, greenValue, blueValue);

        // Ensure child stays alive since Expled is running
        while (true) {
            sleepMicro(1000000LL);
        }

        delete led;
    }
    else {
        // parent process
        string forkInf = "";
        forkInf = forkInf + "EXPLED(red):" +
            + " Red:" + to_string(redValue)
            + " Green:" + to_string(greenValue)
            + " Blue:" + to_string(blueValue);        
        ForkAccess::noteInfo(17, pid, forkInf);

        forkInf = "";
        forkInf = forkInf + "EXPLED(green):" +
            + " Red:" + to_string(redValue)
            + " Green:" + to_string(greenValue)
            + " Blue:" + to_string(blueValue);        
        ForkAccess::noteInfo(16, pid, forkInf);

        forkInf = "";
        forkInf = forkInf + "EXPLED(blue):" +
            + " Red:" + to_string(redValue)
            + " Green:" + to_string(greenValue)
            + " Blue:" + to_string(blueValue);        
        ForkAccess::noteInfo(15, pid, forkInf);
    }
    
    return true;
}

string ExpledOperation::help() {
    stringstream hStr;
    hStr << "expled <value-expr>";
    hStr << "\nOr:";
    hStr << "\nexpled rgb <red-expr> <green-expr> <blue-expr>";
    hStr << "\n\tStarts a separate process to output given colour to";
    hStr << "\n\texpansion dock LED.";
    hStr << "\n\t<value-expr> is an expression that evaluates to a value";
    hStr << "\n\tthat specifies all the LED components as follows:";
    hStr << "\n\t  red = (value >> 16) & 0xFF";
    hStr << "\n\t  green = (value >> 8) & 0xFF";
    hStr << "\n\t  blue = value & 0xFF";
    hStr << "\n\twhere each is in the range '0x00' (off) to '0xFF' (fully on)";
    hStr << "\n\t<red-expr> <green-expr> <blue-expr> are expressions that";
    hStr << "\n\tspecify the LED colours individually";
    hStr << "\n\tEach must be in the range 0 (off) to 100 (fully on)";
    hStr << "\n\tThe separate process runs until the 'expledstop' operation";
    hStr << "\n\tis performed";
    hStr << "\n\tNOTE: expled uses pins:15 (blue), 16 (green), 17 (red)";
    
    return hStr.str();
}
