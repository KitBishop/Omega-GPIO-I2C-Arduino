#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <chrono>
using namespace std;

#include "TimeHelper.h"


void sleepNano(long long nano) {
    struct timespec req = {0};
    time_t sec = (int)(nano / 1000000000LL);
    unsigned long nsec = nano - (((long long)sec) * 1000000000LL);
    req.tv_sec = sec;
    req.tv_nsec = nsec;
    while (nanosleep(&req,&req) == -1) {
         continue;
    }
}

void sleepMicro(long long micro) {
    sleepNano(MICRO_TO_NANO(micro));
}

void sleepMilli(long long milli) {
    sleepNano(MILLI_TO_NANO(milli));
}

void sleepSec(long long sec) {
    sleepNano(SEC_TO_NANO(sec));
}

long long timeNowUS() {
    timeval timeNow;
    gettimeofday(&timeNow, NULL);
    long long nowUS = (timeNow.tv_sec * 1000000LL) + timeNow.tv_usec;

    return nowUS;
}

long long timeNowNS() {
    auto now = chrono::high_resolution_clock::now();
    long long nowNS = chrono::time_point_cast<chrono::nanoseconds>(now).time_since_epoch().count();
    return nowNS;
}

