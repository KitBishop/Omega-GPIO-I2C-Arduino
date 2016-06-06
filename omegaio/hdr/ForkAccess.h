#ifndef ForkAccess_H
#define ForkAccess_H

#include <string>

using namespace std;

class ForkAccess {
public:
    static void noteInfo(int pinnum, pid_t pid, string inf);
    static void stop(int pinnum);
    static bool getInfo(int pinnum, string & inf);

private:
    static void removeMatchingPid(int pid);
};

#endif
