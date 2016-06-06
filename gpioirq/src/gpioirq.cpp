#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include "GPIOPin.h"
#include "GPIOAccess.h"
#include "ForkAccess.h"
#include "Utilities.h"

using namespace std;

void usage(char * nm) {
    printf("Catches interrupts on given pin to run given command(s)\n");
    printf("Usage\n");
    printf("Commands:\n");
    printf("- one of:\n");

    printf("    %s <pin> rising <command> <optional-debounce>\n",nm);
    printf("        Starts background process to run <command> on rising edge of pin\n");
    printf("    %s <pin> falling <command> <optional-debounce>\n",nm);
    printf("        Starts background process to run <command> on falling edge of pin\n");
    printf("    %s <pin> both <command1> <command2> <optional-debounce>\n",nm);
    printf("        Starts background process to run <command1> on rising edge of pin\n");
    printf("                                     and <command2> on falling edge of pin\n");

    printf("    %s <pin> stop\n",nm);
    printf("        Terminates input handling on pin\n");

    printf("    %s help\n",nm);
    printf("        Displays this usage information\n\n");
    printf("Where:\n");
    printf("    <pin> is a valid GPIO pin number to trigger the interrupts\n");

    printf("    <command>, <command1>, <command2> are the commands to be executed\n");
    printf("        on the interrupt.  Must be enclosed in \" characters if they contain\n");
    printf("        spaces or other special characters\n");
    
    printf("    <optional-debounce> is an optional parameter that specifies a debounce time\n");
    printf("        in milliseconds to allow for potentially noisy mechanical switches.\n");
    printf("        Any signal change that occurs within this time of the previous change\n");
    printf("        will be ignored.  If absent or 0, no debounce processing will be applied.\n");
}

#define opirq 1
#define opirq2 2
#define opirqstop 3
int pin;
int operation;
GPIO_Irq_Type irqtype;
char irqCmd1[128];
char irqCmd2[128];
int debounceMS;

bool processArgs(int argc, char** argv) {
    operation = -1;
    if (argc > 1) {
        if (strcmp(argv[1], "help") == 0) {
            return false;
        } 
        
        if (strcmp("0", argv[1]) == 0) {
            pin = 0;
        } else {
            pin = strtol(argv[1], NULL, 10);
            if (pin == 0) {
                printf("**ERROR** Invalid pin number: %s\n", argv[1]);
                return false;
            }
        }
        if (!GPIOAccess::isPinUsable(pin)) {
            printf("**ERROR** Invalid pin number: %s\n", argv[1]);
            return false;
        }

        if (argc <= 2) {
            printf("**ERROR** No irq operation specified.\n");
            return false;
        }

        if (strcmp(argv[2], "stop") == 0) {
            operation = opirqstop;
            return true;
        } else if (strcmp(argv[2], "rising") == 0) {
            operation = opirq;
            irqtype = GPIO_IRQ_RISING;
            debounceMS = 0;
        } else if (strcmp(argv[2], "falling") == 0) {
            operation = opirq;
            irqtype = GPIO_IRQ_FALLING;
            debounceMS = 0;
        } else if (strcmp(argv[2], "both") == 0) {
            operation = opirq2;
            irqtype = GPIO_IRQ_BOTH;
            debounceMS = 0;
        } else {
            printf("**ERROR** Invalid irq operation: %s\n", argv[2]);
            return false;
        }
        
        if (argc <= 3) {
            printf("**ERROR** No command specified.\n");
            return false;
        }
        
        strcpy(irqCmd1, argv[3]);
        
        int dbParmN = 4;
        if (operation == opirq2) {
            if (argc <= 4) {
                printf("**ERROR** No second command specified for 'both'.\n");
                return false;
            }

            strcpy(irqCmd2, argv[4]);
            dbParmN = 5;
        }
        
        if (argc >= (dbParmN + 1)) {
            if (strcmp("0", argv[dbParmN]) == 0) {
                debounceMS = 0;
            } else {
                debounceMS = strtol(argv[dbParmN], NULL, 10);
                if (debounceMS <= 0) {
                    printf("**ERROR** Invalid debounce value: %s\n", argv[dbParmN]);
                    return false;
                }
            }
        }
    } else {
        printf("**ERROR** No parameter supplied\n");
        return false;
    }
    
    return true;
}

void stopIrq() {
    ForkAccess::stop(pin);
}

class GPIO_Irq_Command_Handler_Object : public GPIO_Irq_Handler_Object {
public:
    GPIO_Irq_Command_Handler_Object(GPIO_Irq_Type type, const char * com1, const char * com2) {
        irqType = type;
        
        strcpy(this->cmd1, com1);
        
        if (type == GPIO_IRQ_BOTH) {
            strcpy(this->cmd2, com2);
        }
    }

    void handleIrq(int pinNum, GPIO_Irq_Type type) {
        if (type == irqType) {
            system(cmd1);
        } else {
            if (type == GPIO_IRQ_RISING)  {
                system(cmd1);
            } else {
                system(cmd2);
            }
        }
    }
    
private:
    GPIO_Irq_Type irqType;
    char cmd1[200];
    char cmd2[200];
};

void startIrq() {
    stopIrq();

    // IRQ handling requires a separate process
    pid_t pid = fork();

    if (pid == 0) {
        // child process, run the IRQ

        GPIOPin * gpioPin = new GPIOPin(pin);

        gpioPin->setDirection(GPIO_INPUT);
        
        GPIO_Irq_Command_Handler_Object * handlerObj = new GPIO_Irq_Command_Handler_Object(irqtype, irqCmd1, irqCmd2);
        gpioPin->setIrq(irqtype, handlerObj, debounceMS);
        
        // Ensure child stays alive since IRQ is running
        while (true) {
            usleep(1000000L);
        }
    }
    else {

        string forkInf = "IRQ";
        
        if (irqtype == GPIO_IRQ_BOTH) {
            forkInf += "2: RisingCommand:'" + string(irqCmd1) + "' FallingCommand:'" + string(irqCmd2) + "' DebounceMS:" + to_string(debounceMS);
        } else {
            forkInf += ": Type:";
            if (irqtype == GPIO_IRQ_RISING) {
                forkInf += "rising";
            } else {
                forkInf += "falling";
            }
            forkInf += " Command:'" + string(irqCmd1) + "' DebounceMS:" + to_string(debounceMS);
        }
        
        ForkAccess::noteInfo(pin, pid, forkInf);
    }
}

int main(int argc, char** argv) {
    if (!processArgs(argc, argv)) {
        usage(argv[0]);
        return -1;
    }
    
    if ((operation == opirq) || (operation == opirq2)) {
        startIrq();
        return 0;
    } else if (operation == opirqstop) {
        stopIrq();
        return 0;
    }

    return -1;
}

