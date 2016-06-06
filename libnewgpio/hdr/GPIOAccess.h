#ifndef GPIOAccess_H
#define GPIOAccess_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>

#include "GPIOTypes.h"
#include "GPIOPwmPin.h"
#include "GPIOIrqInfo.h"
#include "GPIOPulseInPin.h"
#include "TimeHelper.h"

#define numPins 27
#define gpioLibraryVersion  "1.4.1"

class GPIOAccess {
    friend GPIOPulseInPin;
public:
    static GPIO_Result setDirection(int pinNum, GPIO_Direction dir);
    static GPIO_Result getDirection(int pinNum, GPIO_Direction &dir);
    static GPIO_Result set(int pinNum, int value);
    static GPIO_Result get(int pinNum, int &value);
    
    static GPIO_Result setIrq(int pinNum, GPIO_Irq_Type type, GPIO_Irq_Handler_Func handler, long int debounceMs = 0);
    static GPIO_Result setIrq(int pinNum, GPIO_Irq_Type type, GPIO_Irq_Handler_Object * handlerObj, long int debounceMs = 0);
    static GPIO_Result resetIrq(int pinNum);
    static GPIO_Result enableIrq(int pinNum, bool enable);
    static GPIO_Result enableIrq(int pinNum);
    static GPIO_Result disableIrq(int pinNum);
    static GPIO_Result irqEnabled(int pinNum, bool & ret);
    static GPIO_Irq_Type getIrqType(int pinNum);
    static GPIO_Irq_Handler_Func getIrqHandler(int pinNum);
    static GPIO_Irq_Handler_Object * getIrqHandlerObj(int pinNum);
    
    static void enableIrq();
    static void disableIrq();
    static void enableIrq(bool enable);
    static bool irqEnabled();
    
    static GPIO_Result setPWM(int pinNum, long int freq, int duty, int durationMs = 0);
    static GPIO_Result startPWM(int pinNum, int durationMs = 0);
    static GPIO_Result stopPWM(int pinNum);
    
    static GPIO_Result getPWMFreq(int pinNum, long int &freq);
    static GPIO_Result getPWMDuty(int pinNum, int &duty);
    static GPIO_Result getPWMDuration(int pinNum, int&duration);
    static GPIO_Result isPWMRunning(int pinNum, bool &irunning);

    static GPIO_Result setTone(int pinNum, long int freq, int durationMs = 0);
    static GPIO_Result startTone(int pinNum, int durationMs = 0);
    static GPIO_Result stopTone(int pinNum);
    static GPIO_Result getToneFreq(int pinNum, long int &freq);
    static GPIO_Result getToneDuration(int pinNum, int & duration);
    static GPIO_Result isToneRunning(int pinNum, bool &running);
    
    static GPIO_Result shiftOut(int dataPinNum, int clockPinNum, int val, long int clockPeriodNS, GPIO_Bit_Order bitOrder = GPIO_MSB_FIRST);
    static GPIO_Result shiftIn(int dataPinNum, int clockPinNum, long int clockPeriodNS, GPIO_Bit_Order bitOrder, int &value);
    
    static GPIO_Result pulseOut(int pinNum, long int pulseLenUS, int pulseLevel = 1);

    static GPIO_Result pulseIn(int pinNum, int pulseLevel, long int timeoutUS, long int &value);
    static GPIO_Result pulseIn(int pinNum, GPIO_PulseIn_Handler_Func handler, int pulseLevel, long int timeoutUS);
    static GPIO_Result pulseIn(int pinNum, GPIO_PulseIn_Handler_Object * handlerObj, int pulseLevel, long int timeoutUS);
    static GPIO_Result stopPulseIn(int pinNum);
    static GPIO_Result isPulseInRunning(int pinNum, bool &running);
    
    static GPIO_Result getFrequency(int pinNum, long int sampleTimeMS, long int &freq);
    
    static bool isPinUsable(int pinNum);
    static bool isAccessOk();
    
    static void rawSet(int pinNum, int value);
    static int rawGet(int pinNum);
    
    static char * getLibVersion();
private:
    static GPIO_Result checkAndSetupAddress(unsigned long int blockBaseAddr, unsigned long int blockSize);
    static GPIO_Result checkPinAndAccessDir(int pin, GPIO_Direction dir);
    static GPIO_Result checkPinAndAccess(int pin);
    
    static void createPwmPinIfNeeded(int pin);
    static void createIrqInfIfNeeded(int pin);
    static void createPulseInPinIfNeeded(int pin);
    
    static void setupIrqHandler();
    static void irqHandler(int n, siginfo_t *info, void *unused);

    static GPIO_Result setPWMInternal(int pinNum, long int freq, int duty, bool isTone, int durationMs);
    static GPIO_Result startPWMInternal(int pinNum, bool isTone, int durationMs);
    
    static GPIO_Direction getDirectionImpl(int pinNum);
    
    static void setBit(unsigned long int &regVal, int bitNum, int value);
    static int getBit(unsigned long int regVal, int bitNum);

    static volatile unsigned long int *regAddress;
    
    static long int pinUsable;
    
    static GPIOPwmPin * pwmPin[numPins];
    static bool pwmSetup;
    
    static GPIOPulseInPin * pulseInPin[numPins];
    static bool pulseInSetup;
    
    static GPIOIrqInf * irqInf[numPins];
    static bool irqSetup;
    
    static void * irqThreadRunner(void * unused);
    static void * runIRQ();
    static pthread_t irqThread;
    static bool irqRunning;
    static sigset_t irqSigset;
    static siginfo_t irqSiginfo;
    static bool irqIsEnabled;
};

#endif
