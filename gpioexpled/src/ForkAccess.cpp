#include <fstream>
#include <cstdlib>
#include <signal.h>

#include "ForkAccess.h"
#include "Utilities.h"
#include "GPIOAccess.h"

#define PID_FILE (char *)"/tmp/gpio_pin_%d_pid"

void ForkAccess::noteInfo(int pinnum, pid_t pid, string inf) {
    char pathname[255];
    ofstream myfile;

    // determine the file name and open the file
    snprintf(pathname, sizeof (pathname), PID_FILE, pinnum);
    myfile.open(pathname);

    // write the pid to the file
    myfile << pid;
    myfile << "\n";

    char infa[255];
    sprintf(infa, "%s\n", inf.c_str());
    myfile << infa;

    myfile.close();
}

void ForkAccess::stop(int pinnum) {
    char pathname[255];
    char line[255];
    char cmd[255];

    int pid;
    std::ifstream myfile;

    // determine the file name and open the file
    snprintf(pathname, sizeof (pathname), PID_FILE, pinnum);
    myfile.open(pathname);

    // read the file
    if (myfile.is_open()) {
        // file exists, check for pid
        myfile.getline(line, 255);
        pid = atoi(line);

        // kill the process
        if (pid > 0) {
            sprintf(cmd, "kill %d >& /dev/null", pid);
            system(cmd);
        }

        myfile.close();
        
        removeMatchingPid(pid);
    }
}

void ForkAccess::removeMatchingPid(int pid) {
    int pin;
    
    for (pin = 0; pin <=30; pin++) {
        if (GPIOAccess::isPinUsable(pin)) {
            char pathname[255];
            char line[255];
            char cmd[255];
            std::ifstream myfile;
            int aPid;
            
            snprintf(pathname, sizeof (pathname), PID_FILE, pin);
            myfile.open(pathname);

            // read the file
            if (myfile.is_open()) {
                // file exists, check for pid
                myfile.getline(line, 255);
                aPid = atoi(line);
            
                myfile.close();
            }
            
            if (aPid == pid) {
                sprintf(cmd, "rm %s >& /dev/null", pathname);
                system(cmd);
            }
        }
    }
}

bool ForkAccess::getInfo(int pinnum, string & inf) {
    char pathname[255];
    char line[255];

    int pid;
    ifstream myfile;

    // determine the file name and open the file
    snprintf(pathname, sizeof (pathname), PID_FILE, pinnum);
    myfile.open(pathname);

    // read the file
    if (myfile.is_open()) {
        bool ret = false;
        // file exists, check for pid
        myfile.getline(line, 255);
        pid = atoi(line);
        if (kill(pid, 0) == 0) {
            // Process is running - get the info
            inf.assign("Process Id:");
            inf.append(to_string(pid));
            inf.append("\n\t");
            myfile.getline(line, 255);
            inf.append(line);
            ret = true;
        }

        myfile.close();

        return ret;
    }

    return false;
}
