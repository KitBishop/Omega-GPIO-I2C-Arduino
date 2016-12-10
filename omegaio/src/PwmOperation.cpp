#include "PwmOperation.h"
#include "Utilities.h"
#include "GPIOAccess.h"
#include "GPIOPin.h"
#include "ForkAccess.h"
#include "TimeHelper.h"

using namespace std;

PwmOperation::PwmOperation()
    : PinOperation() {
    opType = opPwm;
    pinAllAllowed = false;
    freqExpr = NULL;
    dutyExpr = NULL;
    durationExpr = NULL;
}

bool PwmOperation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (PinOperation::build(appInfo, paramList, paramIter)) {
        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No frequency specified for '" + mapFromOpType(opType) + "'");
            return false;
        }

        freqExpr = Expression::create(opType, **paramIter, appInfo);

        if (freqExpr == NULL) {
            return false;
        }

        (*paramIter)++;
        
        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No duty specified for '" + mapFromOpType(opType) + "'");
            return false;
        }

        dutyExpr = Expression::create(opType, **paramIter, appInfo);

        if (dutyExpr == NULL) {
            return false;
        }

        (*paramIter)++;
        
        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No duration specified for '" + mapFromOpType(opType) + "'");
            return false;
        }

        durationExpr = Expression::create(opType, **paramIter, appInfo);

        if (durationExpr == NULL) {
            return false;
        }

        (*paramIter)++;
    } else {
        return false;
    }
    
    return true;
}

string PwmOperation::toString() {
    return PinOperation::toString() + " Frequency:" + freqExpr->getExpressionString() + 
            " Duty:" + dutyExpr->getExpressionString() + " DurationMS:" + durationExpr->getExpressionString();
}

bool PwmOperation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    PinOperation::execute(appInfo);

    long int pinNumber;
    if (!pinExpr->getPinNumber(pinNumber)) {
        return false;
    }

    long int freq;
    if (!freqExpr->eval(freq)) {
        return false;
    }
    if (freq <= 0) {
        appInfo->prtError(opType, "Invalid frequency for '" + mapFromOpType(opType) + "':" + freqExpr->getExpressionString() + "->" + to_string(freq));
        return false;
    }
    
    long int duty;
    if (!dutyExpr->eval(duty)) {
        return false;
    }
    if ((duty < 0) || (duty > 100)) {
        appInfo->prtError(opType, "Invalid duty for '" + mapFromOpType(opType) + "':" + dutyExpr->getExpressionString() + "->" + to_string(duty));
        return false;
    }
    
    long int durationMS;
    if (!durationExpr->eval(durationMS)) {
        return false;
    }
    if (durationMS < 0) {
        appInfo->prtError(opType, "Invalid duration for '" + mapFromOpType(opType) + "':" + durationExpr->getExpressionString() + "->" + to_string(durationMS));
        return false;
    }

    // Continuous PWM output requires a separate process
    pid_t pid = fork();

    if (pid == 0) {
        // child process, run the pwm
        GPIOPin * pin = new GPIOPin(pinNumber);
        if (appInfo->setdir) {
            appInfo->prtReport("Setting pin:" + to_string(pinNumber) + " to output");
            pin->setDirection(GPIO_OUTPUT);
        }
        appInfo->prtReport("Starting PWM to pin:" + to_string(pinNumber) + " Frequency:" + to_string(freq) + " Duty:" + to_string(duty) + " DurationMS:" + to_string(durationMS));
        GPIO_Result r = pin->setPWM(freq, duty, durationMS);
        bool isok = r == GPIO_OK;
        if (!isok) {
            appInfo->prtError(opType, "Failed to start PWM (" + gpioResultStr(r) + ")");
        } else {
            // Ensure child stays alive while PWM is running
            bool running;
            pin->isPWMRunning(running);
            while (running) {
                sleepMicro(1000000LL);
                pin->isPWMRunning(running);
            }
        }
        delete pin;
        ForkAccess::stop(pinNumber);
    }
    else {
        // parent process
        string forkInf = "PWM: Frequency:" + to_string(freq) + " Duty:" + to_string(duty) + " DurationMS:" + to_string(durationMS);

        ForkAccess::noteInfo(pinNumber, pid, forkInf);
    }
    
    return true;
}

string PwmOperation::help() {
    stringstream hStr;
    hStr << "pwm <pin-expr> <freq-expr> <duty-expr> <duration-expr>";
    hStr << "\n\tStarts a separate process to perform PWM output";
    hStr << "\n\ton given pin with given information";
    hStr << "\n\t<pin-expr> is an expression that must evaluate to one of:";
    hStr << "\n\t\t0,1,6,7,8,12,13,14,15,16,17,18,19,23,26";
    hStr << "\n\t<freq-expr> is an expression that evaluates to the";
    hStr << "\n\tfrequency of the PWM pulses. Must be greater than 0";
    hStr << "\n\t<duty-expr> is an expression that evaluates to the";
    hStr << "\n\tduty cycle % of the PWM pulses. Must be >= 0 and <= 100";
    hStr << "\n\t<duration-expr> is an expression that evaluates to the";
    hStr << "\n\tduration of the output in milliseconds";
    hStr << "\n\tMust be >= 0 When 0, duration is indefinite";
    hStr << "\n\tThe separate process runs until the duration expires";
    hStr << "\n\tor the 'pwmstop' operation is performed on the same <pin-number>";
    
    return hStr.str();
}
