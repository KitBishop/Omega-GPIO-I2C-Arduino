#include <cstdlib>

#include "Irq2Operation.h"
#include "Utilities.h"
#include "GPIOAccess.h"
#include "GPIOPin.h"
#include "ForkAccess.h"

using namespace std;

class GPIO_Irq2_Command_Handler_Object : public GPIO_Irq_Handler_Object {
public:
    GPIO_Irq2_Command_Handler_Object(const char * com1, const char * com2) {
        strcpy(this->cmd1, com1);
        strcpy(this->cmd2, com2);
    }

    void handleIrq(int pinNum, GPIO_Irq_Type type) {
        if (type == GPIO_IRQ_RISING) {
            if (strstr(cmd1, "[debug]") == cmd1) {
                char dbgcmd[300];
                sprintf(dbgcmd, "echo 'GPIO Irq Debug: Pin=%d Type=Rising' && ", pinNum);
                strcat(dbgcmd, cmd1 + 7);
                system(dbgcmd);
            } else {
                system(cmd1);
            }
        } else {
            if (strstr(cmd2, "[debug]") == cmd2) {
                char dbgcmd[300];
                sprintf(dbgcmd, "echo 'GPIO Irq Debug: Pin=%d Type=Falling' && ", pinNum);
                strcat(dbgcmd, cmd2 + 7);
                system(dbgcmd);
            } else {
                system(cmd2);
            }
        }
    }
    
private:
    char cmd1[200];
    char cmd2[200];
};

Irq2Operation::Irq2Operation()
    : PinOperation() {
    opType = opIrq2;
    pinAllAllowed = false;
    cmd1 = "";
    cmd2 = "";
    debounceExpr = NULL;
}

bool Irq2Operation::build(AppInfo * appInfo, list<string> * &paramList, list<string>::iterator * &paramIter) {
    if (PinOperation::build(appInfo, paramList, paramIter)) {

        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No Rising command specified for '" + mapFromOpType(opType) + "'");
            return false;
        }
        
        cmd1 = **paramIter;
        
        (*paramIter)++;

        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No Falling command specified for '" + mapFromOpType(opType) + "'");
            return false;
        }
        
        cmd2 = **paramIter;
        
        (*paramIter)++;

        if (*paramIter == paramList->end()) {
            appInfo->prtError(opType, "No debounce time specified for '" + mapFromOpType(opType) + "'");
            return false;
        }

        debounceExpr = Expression::create(opType, **paramIter, appInfo);

        if (debounceExpr == NULL) {
            return false;
        }
        
        (*paramIter)++;
    } else {
        return false;
    }
    
    return true;
}

string Irq2Operation::toString() {
    string oinf = PinOperation::toString();
    oinf += " RisingCommand:'" + cmd1 + "' FallingCommand:'" + cmd2 + "' DebounceMS:" + debounceExpr->getExpressionString();
    
    return oinf;
}

bool Irq2Operation::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    PinOperation::execute(appInfo);

    long int pinNumber;
    if (!pinExpr->getPinNumber(pinNumber)) {
        return false;
    }

    long int debounceMS;
    if (!debounceExpr->eval(debounceMS)) {
        return false;
    };
    
    if (debounceMS < 0) {
        appInfo->prtError(opType, "Invalid debounce time for '" + mapFromOpType(opType) + "':" + debounceExpr->getExpressionString() + "->" + to_string(debounceMS));
        return false;
    }
    
    // IRQ2 handling requires a separate process
    pid_t pid = fork();


    if (pid == 0) {
        string cmd1ex = appInfo->replaceVars(cmd1);
        string cmd2ex = appInfo->replaceVars(cmd2);
        // child process, run the pwm
        GPIOPin * pin = new GPIOPin(pinNumber);
        if (appInfo->setdir) {
            appInfo->prtReport("Setting pin:" + to_string(pinNumber) + " to input");
            pin->setDirection(GPIO_INPUT);
        }

        string oinf = "Enabling IRQ2 handling on pin:" + to_string(pinNumber);
        oinf += " RisingCommand:'" + cmd1ex + "' FallingCommand:'" + cmd2ex + "' DebounceMS:" + debounceExpr->getExpressionString();
        appInfo->prtReport(oinf);

        GPIO_Irq2_Command_Handler_Object * irq2HandlerObj = new GPIO_Irq2_Command_Handler_Object(cmd1ex.c_str(), cmd2ex.c_str());
        GPIO_Result r = pin->setIrq(GPIO_IRQ_BOTH, irq2HandlerObj, debounceMS);
        bool isok = r == GPIO_OK;
        if (!isok) {
            appInfo->prtError(opType, "Failed to enable IRQ handling (" + gpioResultStr(r) + ")");
        } else {
            // Ensure child stays alive since IRQ is running
            while (true) {
                sleepMicro(1000000LL);
            }
        }
        delete pin;
        ForkAccess::stop(pinNumber);
    }
    else {
        string cmd1ex = appInfo->replaceVars(cmd1);
        string cmd2ex = appInfo->replaceVars(cmd2);
        // parent process
        string forkInf = "IRQ2:";
        forkInf += " RisingCommand:'" + cmd1ex + "' FallingCommand:'" + cmd2ex + "' DebounceMS:" + to_string(debounceMS);
        
        ForkAccess::noteInfo(pinNumber, pid, forkInf);
    }
    
    return true;
}

string Irq2Operation::help() {
    stringstream hStr;
    hStr << "irq2 <pin-expr> <rising-command> <falling-command> <debounce-expr>";
    hStr << "\n\tStarts a separate process to respond to interrupts";
    hStr << "\n\ton given pin and perform given commands on interrupt";
    hStr << "\n\tboth for rising and falling edges";
    hStr << "\n\t<pin-expr> is an expression that must evaluate to one of:";
    hStr << "\n\t\t0,1,6,7,8,12,13,14,15,16,17,18,19,23,26";
    hStr << "\n\t<rising-command> is the command to be performed on interrupt";
    hStr << "\n\t\trising edge.";
    hStr << "\n\t<falling-command> is the command to be performed on interrupt";
    hStr << "\n\t\tfalling edge.";
    hStr << "\n\tEach must be enclosed in \" characters if it contains spaces";
    hStr << "\n\tor other special character";
    
    hStr << "\n\tEach command may contain any number of <varsub>s which will be";
    hStr << "\n\treplaced by actual values at the time the irq2 operation is";
    hStr << "\n\tactually invoked.";
    hStr << "\n\tA <varsub> is any sequence like {<varref>}";
    hStr << "\n\twhere '<varref>' is one of:";
    hStr << "\n\t  - <variable-name> = current value of variable";
    hStr << "\n\t    as assigned in an 'assign' operation ";
    hStr << "\n\t    or not substituted if variable has never been assigned";
    hStr << "\n\t  - $n or $nn = current value of pin n or nn";
    hStr << "\n\t  - $! = value of latest result set by earlier commands";
    hStr << "\n\t  - $? = status of last executed command: 0=error, 1=ok";
    hStr << "\n\t  - $[<file-name>] = file <file-name> exists; 0=no, 1=yes";
    
    hStr << "\n\t<debounce-expr> is an expression that evaluates to a";
    hStr << "\n\tdebounce time in milliseconds.  Any interrupts that occur within";
    hStr << "\n\tthis time of a previous interrupt will be ignored.";
    hStr << "\n\tUsed to cater for noisy mechanical signals";
    hStr << "\n\tMust be greater than or equal to 0";
    hStr << "\n\tWhen 0, no debounce testing is applied";
    hStr << "\n\tThe separate process runs until the 'irqstop' operation";
    hStr << "\n\tis performed on the same <pin-number>";
    
    return hStr.str();
}
