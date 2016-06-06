#ifndef TimeHelper_H
#define TimeHelper_H

void sleepNano(long long nano);
void sleepMicro(long long micro);
void sleepMilli(long long milli);
void sleepSec(long long sec);

#define SEC_TO_MILLI(s) (((long long)s) * 1000LL)
#define SEC_TO_MICRO(s) (((long long)s) * 1000000LL)
#define SEC_TO_NANO(s) (((long long)s) * 1000000000LL)

#define MILLI_TO_SEC(m) (((long long)m) / 1000LL)
#define MILLI_TO_MICRO(m) (((long long)m) * 1000LL)
#define MILLI_TO_NANO(m) (((long long)m) * 1000000LL)

#define MICRO_TO_SEC(u) (((long long)u) / 1000000LL)
#define MICRO_TO_MILLI(u) (((long long)u) / 1000LL)
#define MICRO_TO_NANO(u) (((long long)u) * 1000LL)

#define NANO_TO_SEC(n) (((long long)n) / 1000000000LL)
#define NANO_TO_MILLLLI(n) (((long long)n) / 1000000LL)
#define NANO_TO_MICRO(n) (((long long)n) / 1000LL)

long long timeNowUS();
long long timeNowNS();

#endif
