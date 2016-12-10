#include "GPIOAccess.h"

#define REGISTER_BLOCK_ADDR     0x18040000
#define REGISTER_BLOCK_SIZE     0x30

#define REGISTER_OE_OFFSET      0
#define REGISTER_IN_OFFSET      1
#define REGISTER_OUT_OFFSET     2
#define REGISTER_SET_OFFSET     3
#define REGISTER_CLEAR_OFFSET   4
#define REGISTER_IRQ_ENABLE     5
#define REGISTER_IRQ_TYPE       6
#define REGISTER_IRQ_POLARITY   7
#define REGISTER_IRQ_PENDING    8
#define REGISTER_IRQ_MASK       9

long int GPIOAccess::pinUsable =
    0 |
    1 << 0 |    // 0
    1 << 1 |    // 1
    0 << 2 |    // ~2
    0 << 3 |    // ~3
    0 << 4 |    // ~4
    0 << 5 |    // ~5
    1 << 6 |    // 6
    1 << 7 |    // 7
    1 << 8 |    // 8
    0 << 9 |    // ~9
    0 << 10 |   // ~10
    0 << 11 |   // ~``
    1 << 12 |   // 12
    1 << 13 |   // 13
    1 << 14 |   // 14
    1 << 15 |   // 15
    1 << 16 |   // 16
    1 << 17 |   // 17
    1 << 18 |   // 18
    1 << 19 |   // 19
    1 << 20 |   // 20
    1 << 21 |   // 21
    0 << 22 |   // ~22
    1 << 23 |   // 23
    0 << 24 |   // ~24
    0 << 25 |   // ~25
    1 << 26;    // 26


GPIOPwmPin * GPIOAccess::pwmPin[numPins];
bool GPIOAccess::pwmSetup = false;

GPIOPulseInPin * GPIOAccess::pulseInPin[numPins];
bool GPIOAccess::pulseInSetup = false;

GPIOIrqInf * GPIOAccess::irqInf[numPins];
bool GPIOAccess::irqSetup = false;
pthread_t GPIOAccess::irqThread;
bool GPIOAccess::irqRunning = false;
sigset_t GPIOAccess::irqSigset;
siginfo_t GPIOAccess::irqSiginfo;

#define SIG_GPIO_IRQ 42    // So we have to hardcode this value
bool GPIOAccess::irqIsEnabled = true;

volatile unsigned long int *GPIOAccess::regAddress = NULL;

#define WRITEREG(regOff, regVal) (*(regAddress + regOff) = regVal)
#define READREG(regVal, regOff) (regVal = *(regAddress + regOff))
#define SETBIT(regVal, bit, val) (regVal ^= (-val ^ regVal) & (1 << bit))
#define GETBIT(regval, bit) ((regval >> bit) & 0x1)

GPIO_Result GPIOAccess::checkPinAndAccessDir(int pin, GPIO_Direction dir) {
    GPIO_Result res = checkPinAndAccess(pin);
    if (res == GPIO_OK) {
        if (getDirectionImpl(pin) != dir) {
            res = GPIO_INVALID_OP;
        }
    }
    
    return res;
}


GPIO_Result GPIOAccess::checkPinAndAccess(int pin) {
    if (!isPinUsable(pin)) {
        return GPIO_INVALID_PIN;
    }

    return checkAndSetupAddress(REGISTER_BLOCK_ADDR, REGISTER_BLOCK_SIZE);
}

GPIO_Result GPIOAccess::checkAndSetupAddress(unsigned long int blockBaseAddr, unsigned long int blockSize) {
    if (regAddress != NULL) {
        if (regAddress == MAP_FAILED) {
            return GPIO_BAD_ACCESS; // maybe return -2
        }

        return GPIO_OK; // regAddress is now populated
    }

    int m_mfd;

    if ((m_mfd = open("/dev/mem", O_RDWR)) < 0) {
        return GPIO_BAD_ACCESS; // maybe return -1
    }

    regAddress = (unsigned long*) mmap(NULL,
            blockSize,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            m_mfd,
            blockBaseAddr
            );
    close(m_mfd);

    if (regAddress == MAP_FAILED) {
        return GPIO_BAD_ACCESS; // maybe return -2
    }

    if (!pwmSetup) {
        int i;
        for (i = 0; i < numPins; i++) {
                pwmPin[i] = NULL;
        }
        pwmSetup = true;
    }

    if (!pulseInSetup) {
        int i;
        for (i = 0; i < numPins; i++) {
                pulseInPin[i] = NULL;
        }
        pulseInSetup = true;
    }

    if (!irqSetup) {
        int i;
        for (i = 0; i < numPins; i++) {
            irqInf[i] = NULL;
        }
        setupIrqHandler();
        irqSetup = true;
    }

    return GPIO_OK; // regAddress is now populated
}

void GPIOAccess::createPwmPinIfNeeded(int pin) {
    if (pwmPin[pin] == NULL) {
        pwmPin[pin] = new GPIOPwmPin(pin);
    }
}

void GPIOAccess::createPulseInPinIfNeeded(int pin) {
    if (pulseInPin[pin] == NULL) {
        pulseInPin[pin] = new GPIOPulseInPin(pin);
    }
}

void GPIOAccess::createIrqInfIfNeeded(int pin) {
    if (irqInf[pin] == NULL) {
        irqInf[pin] = new GPIOIrqInf;

        irqInf[pin]->type = GPIO_IRQ_NONE;
        irqInf[pin]->handlerFunc = NULL;
        irqInf[pin]->handlerObj = NULL;
        irqInf[pin]->enabled = false;
        irqInf[pin]->debounceMs = 0;
        irqInf[pin]->lastTimeMs = 0;
        irqInf[pin]->lastVal = 0;
    }
}

void * GPIOAccess::irqThreadRunner(void * unused) {
    return runIRQ();
}

void * GPIOAccess::runIRQ() {
    irqRunning = true;
    while (irqRunning) {
        sigwaitinfo(&irqSigset, &irqSiginfo);
    }
    return NULL;
}

void GPIOAccess::irqHandler(int n, siginfo_t *info, void *unused) {
    // Ignore if irq not enabled
    if (!irqIsEnabled) {
        return;
    }

    // Get type of change and pin it applies to
    int pin = -1;
    int val = 0;
    GPIO_Irq_Type type = GPIO_IRQ_NONE;
    pin = (info->si_int >> 24) & 0xff;
    val = info->si_int & 0xff;
    if (val == 0) {
        type = GPIO_IRQ_FALLING;
    } else {
        type = GPIO_IRQ_RISING;
        val = 1;
    }

    createIrqInfIfNeeded(pin);
    
    // Only process if type matches that for pin and irq is enabled for pin
    if (((irqInf[pin]->type == type) || (irqInf[pin]->type == GPIO_IRQ_BOTH)) && irqInf[pin]->enabled) {
        bool wantIrq = true;

        // Check for change in pin value in GPIO_IRQ_BOTH case
        if (irqInf[pin]->type == GPIO_IRQ_BOTH) {
            wantIrq = (val != irqInf[pin]->lastVal);
        }

        // Check for any debounce to be applied
        if (wantIrq && (irqInf[pin]->debounceMs > 0)) {
            timeval timeNow;
            gettimeofday(&timeNow, NULL);
            long int nowMs = (timeNow.tv_sec * 1000L) + (timeNow.tv_usec / 1000L);

            long int timeDiff = nowMs - irqInf[pin]->lastTimeMs;

            // Don't want it if insufficient time has elapsed for debounce
            if (timeDiff < irqInf[pin]->debounceMs) {
                wantIrq = false;
            }

            // Preserve last value change time
            irqInf[pin]->lastTimeMs = nowMs;
        }

        if (wantIrq) {
            // Need to handle irq
            // Call whichever function or object is registered
            if (irqInf[pin]->handlerFunc != NULL) {
                irqInf[pin]->handlerFunc(pin, type);
            } else if (irqInf[pin]->handlerObj != NULL) {
                irqInf[pin]->handlerObj->handleIrq(pin, type);
            }

            // Preserve last applied value for GPIO_IRQ_BOTH case
            if (irqInf[pin]->type == GPIO_IRQ_BOTH) {
                irqInf[pin]->lastVal = val;
            }
        }
    }
}

void GPIOAccess::setupIrqHandler() {
    if (!irqRunning) {
        struct sigaction sig;
        sig.sa_sigaction = GPIOAccess::irqHandler;

        sig.sa_flags = SA_SIGINFO | SA_NODEFER;
        sigaction(SIG_GPIO_IRQ, &sig, NULL);

        sigemptyset(&irqSigset);
//        sigaddset(&irqSigset, SIGINT); //	Ctrl+C

        sigprocmask(SIG_BLOCK, &irqSigset, NULL);

        int pres = pthread_create(&irqThread, NULL, &irqThreadRunner, NULL);
        irqRunning = true;
    }
}

void GPIOAccess::enableIrq() {
    enableIrq(true);
}

void GPIOAccess::disableIrq() {
    enableIrq(false);
}

void GPIOAccess::enableIrq(bool enable) {
    irqIsEnabled = enable;
}

bool GPIOAccess::irqEnabled() {
    return irqIsEnabled;
}

GPIO_Result GPIOAccess::enableIrq(int pinNum) {
    return enableIrq(pinNum, true);
}

GPIO_Result GPIOAccess::disableIrq(int pinNum) {
    return enableIrq(pinNum, false);
}

GPIO_Result GPIOAccess::enableIrq(int pinNum, bool enable) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        createIrqInfIfNeeded(pinNum);
        irqInf[pinNum]->enabled = enable;
    }
    
    return res;
}

GPIO_Result GPIOAccess::irqEnabled(int pinNum, bool & ret) {
    ret = false;
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        createIrqInfIfNeeded(pinNum);
        ret = irqInf[pinNum]->enabled;
    }

    return res;
}

GPIO_Irq_Type GPIOAccess::getIrqType(int pinNum) {
    GPIO_Irq_Type type = GPIO_IRQ_NONE;
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        createIrqInfIfNeeded(pinNum);
        type = irqInf[pinNum]->type;
    }

    return type;
}

GPIO_Irq_Handler_Func GPIOAccess::getIrqHandler(int pinNum) {
    GPIO_Irq_Handler_Func handler = NULL;
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        createIrqInfIfNeeded(pinNum);
        handler = irqInf[pinNum]->handlerFunc;
    }

    return handler;
}

GPIO_Irq_Handler_Object * GPIOAccess::getIrqHandlerObj(int pinNum) {
    GPIO_Irq_Handler_Object * handlerObj = NULL;
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        createIrqInfIfNeeded(pinNum);
        handlerObj = irqInf[pinNum]->handlerObj;
    }

    return handlerObj;
}

GPIO_Result GPIOAccess::setDirection(int pinNum, GPIO_Direction dir) {
    GPIO_Result res = checkPinAndAccess(pinNum);
    if (res == GPIO_OK) {
        unsigned long int regVal;

        // read the current input and output settings
        READREG(regVal, REGISTER_OE_OFFSET);

        // set the OE for this pin
        SETBIT(regVal, pinNum, dir);

        // write the new register value
        WRITEREG(REGISTER_OE_OFFSET, regVal);
    }

    return res;
}

GPIO_Result GPIOAccess::getDirection(int pinNum, GPIO_Direction &dir) {
    dir = GPIO_INPUT;
    GPIO_Result res = checkPinAndAccess(pinNum);
    if (res == GPIO_OK) {
        unsigned long int regVal;

        // find the OE for this pin
        dir = getDirectionImpl(pinNum);
    }

    return res;
}

GPIO_Direction GPIOAccess::getDirectionImpl(int pinNum) {
    unsigned long int regVal;

    // read the current input and output settings
    READREG(regVal, REGISTER_OE_OFFSET);

    // find the OE for this pin
    return GETBIT(regVal, pinNum) ? GPIO_OUTPUT : GPIO_INPUT;
}

void GPIOAccess::rawSet(int pinNum, int value) {
    unsigned long int regAddr;
    unsigned long int regVal;

    if (value == 0) {
        // write to the clear register
        regAddr = REGISTER_CLEAR_OFFSET;
    } else {
        // write to the set register
        regAddr = REGISTER_SET_OFFSET;
    }

    // put the desired pin value into the register 
    regVal = (0x1 << pinNum);

    // write to the register
    WRITEREG(regAddr, regVal);
}

GPIO_Result GPIOAccess::set(int pinNum, int value) {
    GPIO_Result res = checkPinAndAccess(pinNum);
    if (res == GPIO_OK) {
        rawSet(pinNum, value);
    }

    return res;
}

int GPIOAccess::rawGet(int pinNum) {
    unsigned long int regVal;

    // read the current value of all pins
    READREG(regVal, REGISTER_IN_OFFSET);

    // find the value of the specified pin
    return GETBIT(regVal, pinNum);
}

GPIO_Result GPIOAccess::get(int pinNum, int &value) {
    value = 0;
    GPIO_Result res = checkPinAndAccess(pinNum);
    if (res == GPIO_OK) {
        value = rawGet(pinNum);
    }

    return res;
}

GPIO_Result GPIOAccess::setIrq(int pinNum, GPIO_Irq_Type type, GPIO_Irq_Handler_Func handler, long int debounceMs) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        resetIrq(pinNum);
        if (type != GPIO_IRQ_NONE) {
            int fd;
            char buf[100];

            createIrqInfIfNeeded(pinNum);

            fd = open("/sys/kernel/debug/gpio-irq", O_WRONLY);
            if (fd < 0) {
                res = GPIO_INVALID_OP;
            } else {
                sprintf(buf, "+ %d %i", pinNum, getpid());

                if (write(fd, buf, strlen(buf) + 1) < 0) {
                    res = GPIO_INVALID_OP;
                } else {
                    irqInf[pinNum]->type = type;
                    irqInf[pinNum]->handlerFunc = handler;
                    irqInf[pinNum]->handlerObj = NULL;
                    irqInf[pinNum]->enabled = true;
                    irqInf[pinNum]->debounceMs = debounceMs;
                    irqInf[pinNum]->lastTimeMs = 0;
                    irqInf[pinNum]->lastVal = 0;
                    if (!irqRunning) {
                        setupIrqHandler();
                    }
                }

                close(fd);
            }
        }
    }

    return res;
}

GPIO_Result GPIOAccess::setIrq(int pinNum, GPIO_Irq_Type type, GPIO_Irq_Handler_Object * handlerObj, long int debounceMs) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        resetIrq(pinNum);
        if (type != GPIO_IRQ_NONE) {
            int fd;
            char buf[100];

            createIrqInfIfNeeded(pinNum);

            fd = open("/sys/kernel/debug/gpio-irq", O_WRONLY);
            if (fd < 0) {
                res = GPIO_INVALID_OP;
            } else {
                sprintf(buf, "+ %d %i", pinNum, getpid());

                if (write(fd, buf, strlen(buf) + 1) < 0) {
                    res = GPIO_INVALID_OP;
                } else {
                    irqInf[pinNum]->type = type;
                    irqInf[pinNum]->handlerFunc = NULL;
                    irqInf[pinNum]->handlerObj = handlerObj;
                    irqInf[pinNum]->enabled = true;
                    irqInf[pinNum]->debounceMs = debounceMs;
                    irqInf[pinNum]->lastTimeMs = 0;
                    irqInf[pinNum]->lastVal = 0;
                    if (!irqRunning) {
                        setupIrqHandler();
                    }
                }
                close(fd);
            }
        }
    }
    
    return res;
}

GPIO_Result GPIOAccess::resetIrq(int pinNum) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        int fd;
        char buf[100];

        createIrqInfIfNeeded(pinNum);

        fd = open("/sys/kernel/debug/gpio-irq", O_WRONLY);
        if (fd < 0) {
            res = GPIO_INVALID_OP;
        } else {
            sprintf(buf, "- %d %i", pinNum, getpid());

            if (write(fd, buf, strlen(buf) + 1) < 0) {
                res = GPIO_INVALID_OP;
            } else {
                irqInf[pinNum]->type = GPIO_IRQ_NONE;
                irqInf[pinNum]->handlerFunc = NULL;
                irqInf[pinNum]->handlerObj = NULL;
                irqInf[pinNum]->enabled = false;
                irqInf[pinNum]->debounceMs = 0;
                irqInf[pinNum]->lastTimeMs = 0;
                irqInf[pinNum]->lastVal = 0;
            }
            close(fd);
        }
    }

    return res;
}

bool GPIOAccess::isPinUsable(int pin) {
    if ((pin < 0) || (pin >= numPins)) {
        return false;
    }

    return (pinUsable & (1 << pin)) != 0;
}

bool GPIOAccess::isAccessOk() {
    return checkAndSetupAddress(REGISTER_BLOCK_ADDR, REGISTER_BLOCK_SIZE) == GPIO_OK;
}

GPIO_Result GPIOAccess::setPWM(int pinNum, long int freq, int duty, int durationMs) {
    return setPWMInternal(pinNum, freq, duty, false, durationMs);
}

GPIO_Result GPIOAccess::setPWMInternal(int pinNum, long int freq, int duty, bool isTone, int durationMs) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_OUTPUT);
    if (res == GPIO_OK) {
        createPwmPinIfNeeded(pinNum);
        res = pwmPin[pinNum]->setPWM(freq, duty, isTone, durationMs);

        if (res == GPIO_OK) {
            // Wait till thread has actually started PWM
            while (!pwmPin[pinNum]->isPWMRunning()) {
            }
        }
    }

    return res;
}

GPIO_Result GPIOAccess::startPWM(int pinNum, int durationMs) {
    return startPWMInternal(pinNum, false, durationMs);
}

GPIO_Result GPIOAccess::startPWMInternal(int pinNum, bool isTone, int durationMs) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_OUTPUT);
    if (res == GPIO_OK) {
        createPwmPinIfNeeded(pinNum);
        if (pwmPin[pinNum]->isTone() == isTone) {
            res = pwmPin[pinNum]->startPWM(isTone, durationMs);
        } else {
            res = GPIO_INVALID_OP;
        }
    }

    return res;
}

GPIO_Result GPIOAccess::stopPWM(int pinNum) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_OUTPUT);
    if (res == GPIO_OK) {
        createPwmPinIfNeeded(pinNum);
        res = pwmPin[pinNum]->stopPWM();
    }

    return res;
}

GPIO_Result GPIOAccess::getPWMFreq(int pinNum, long int &freq) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_OUTPUT);
    freq = 0;
    if (res == GPIO_OK) {
        createPwmPinIfNeeded(pinNum);
        if (!pwmPin[pinNum]->isTone()) {
            freq = pwmPin[pinNum]->getPWMFreq();
        } else {
            res = GPIO_INVALID_OP;
        }
    }

    return res;
}

GPIO_Result GPIOAccess::getPWMDuty(int pinNum, int &duty) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_OUTPUT);
    duty = 0;
    if (res == GPIO_OK) {
        createPwmPinIfNeeded(pinNum);
        if (!pwmPin[pinNum]->isTone()) {
            duty = pwmPin[pinNum]->getPWMDuty();
        } else {
            res = GPIO_INVALID_OP;
        }
    }

    return res;
}

GPIO_Result GPIOAccess::isPWMRunning(int pinNum, bool &running) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_OUTPUT);
    running = false;
    if (res == GPIO_OK) {
        createPwmPinIfNeeded(pinNum);
        running = pwmPin[pinNum]->isPWMRunning();
    }

    return res;
}

GPIO_Result GPIOAccess::getPWMDuration(int pinNum, int &duration) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_OUTPUT);
    duration = 0;
    if (res == GPIO_OK) {
        createPwmPinIfNeeded(pinNum);
        if (!pwmPin[pinNum]->isTone()) {
            duration = pwmPin[pinNum]->getPWMDuration();
        } else {
            res = GPIO_INVALID_OP;
        }
    }

    return res;
}

GPIO_Result GPIOAccess::setTone(int pinNum, long int freq, int durationMs) {
    return setPWMInternal(pinNum, freq, 50, true, durationMs);
}

GPIO_Result GPIOAccess::startTone(int pinNum, int durationMs) {
    return startPWMInternal(pinNum, true, durationMs);
}

GPIO_Result GPIOAccess::stopTone(int pinNum) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_OUTPUT);
    if (res == GPIO_OK) {
        createPwmPinIfNeeded(pinNum);
        res = pwmPin[pinNum]->stopPWM();
    }

    return res;
}

GPIO_Result GPIOAccess::getToneFreq(int pinNum, long int &freq) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_OUTPUT);
    freq = 0;
    if (res == GPIO_OK) {
        createPwmPinIfNeeded(pinNum);
        if (pwmPin[pinNum]->isTone()) {
            freq = pwmPin[pinNum]->getPWMFreq();
        } else {
            res = GPIO_INVALID_OP;
        }
    }

    return res;
}

GPIO_Result GPIOAccess::getToneDuration(int pinNum, int &duration) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_OUTPUT);
    duration = 0;
    if (res == GPIO_OK) {
        createPwmPinIfNeeded(pinNum);
        if (pwmPin[pinNum]->isTone()) {
            duration = pwmPin[pinNum]->getPWMDuration();
        } else {
            res = GPIO_INVALID_OP;
        }
    }

    return res;
}

GPIO_Result GPIOAccess::isToneRunning(int pinNum, bool &running) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_OUTPUT);
    running = false;
    if (res == GPIO_OK) {
        createPwmPinIfNeeded(pinNum);
        running = pwmPin[pinNum]->isPWMRunning();
    }

    return res;
}

GPIO_Result GPIOAccess::shiftOut(int dataPinNum, int clockPinNum, int val, long int clockPeriodNS, GPIO_Bit_Order bitOrder) {
    GPIO_Result res = checkPinAndAccessDir(dataPinNum, GPIO_OUTPUT);
    if (res == GPIO_OK) {
        res = checkPinAndAccessDir(clockPinNum, GPIO_OUTPUT);
        if (res == GPIO_OK) {
            if (clockPeriodNS < 8) {
                res = GPIO_INVALID_OP;
            } else {
                long int clockLenNS = clockPeriodNS / 2;
                long int dataToClockNS = clockLenNS / 4;
                int curBit = 0;
                int bitCount;

                rawSet(dataPinNum, 0);
                rawSet(clockPinNum, 0);

                for (bitCount = 0; bitCount < 8; bitCount++) {
                    // get the bit
                    if (bitOrder == GPIO_MSB_FIRST) {
                        curBit = (val >> (7 - bitCount)) & 1;
                    } else {
                        curBit = (val >> bitCount) & 1;
                    }
                    rawSet(dataPinNum, curBit);
                    sleepNano(dataToClockNS);
                    rawSet(clockPinNum, 1);
                    sleepNano(clockLenNS);
                    rawSet(clockPinNum, 0);
                    sleepNano(clockLenNS - dataToClockNS);
                }
            }
        }
    }

    return res;
}

GPIO_Result GPIOAccess::shiftIn(int dataPinNum, int clockPinNum, long int clockPeriodNS, GPIO_Bit_Order bitOrder, int &val) {
    val = 0;
    if (dataPinNum == clockPinNum) {
        return GPIO_INVALID_OP;
    }
    GPIO_Result res = checkPinAndAccessDir(dataPinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        res = checkPinAndAccessDir(clockPinNum, GPIO_OUTPUT);
        if (res == GPIO_OK) {
            if (clockPeriodNS < 8) {
                res = GPIO_INVALID_OP;
            } else {
                // do it here
                long int clockLenNS = clockPeriodNS / 2;
                long int dataToClockNS = clockLenNS / 4;
                int curBit = 0;
                int bitCount;

                curBit = rawGet(dataPinNum);
                rawSet(clockPinNum, 0);

                for (bitCount = 0; bitCount < 8; bitCount++) {
                    if (bitOrder == GPIO_MSB_FIRST) {
                        val = val + (curBit << (7 - bitCount));
                    } else {
                        val = val + (curBit << bitCount);
                    }

                    sleepNano(dataToClockNS);
                    rawSet(clockPinNum, 1);
                    sleepNano(clockLenNS);
                    rawSet(clockPinNum, 0);
                    sleepNano(clockLenNS - dataToClockNS);

                    curBit = rawGet(dataPinNum);
                }
            }
        }
    }

    return res;
}

GPIO_Result GPIOAccess::pulseOut(int pinNum, long int pulseLenUS, int pulseLevel){
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_OUTPUT);
    if (res == GPIO_OK) {
        int offV;
        int onV;
        if (pulseLevel == 0) {
            offV = 1;
            onV = 0;
        } else {
            offV = 0;
            onV = 1;
        }

        rawSet(pinNum, offV);
        sleepNano(10);
        rawSet(pinNum, onV);
        sleepMicro(pulseLenUS);
        rawSet(pinNum, offV);
    }
    
    return res;
}

GPIO_Result GPIOAccess::pulseIn(int pinNum, int pulseLevel, long int timeoutUS, long int &val) {
    val = 0;
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        createPulseInPinIfNeeded(pinNum);
        res = pulseInPin[pinNum]->pulseIn(pulseLevel, timeoutUS, val);
    }

    return res;
}

GPIO_Result GPIOAccess::pulseIn(int pinNum, GPIO_PulseIn_Handler_Func handler, int pulseLevel, long int timeoutUS) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        createPulseInPinIfNeeded(pinNum);
        res = pulseInPin[pinNum]->pulseIn(handler, pulseLevel, timeoutUS);
    }

    return res;
}

GPIO_Result GPIOAccess::pulseIn(int pinNum, GPIO_PulseIn_Handler_Object * handlerObj, int pulseLevel, long int timeoutUS) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        createPulseInPinIfNeeded(pinNum);
        res = pulseInPin[pinNum]->pulseIn(handlerObj, pulseLevel, timeoutUS);
    }

    return res;
}

GPIO_Result GPIOAccess::stopPulseIn(int pinNum) {
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        createPulseInPinIfNeeded(pinNum);
        res = pulseInPin[pinNum]->stopPulseIn();
    }

    return res;
}

GPIO_Result GPIOAccess::isPulseInRunning(int pinNum, bool &running) {
    running = false;
    GPIO_Result res = checkPinAndAccessDir(pinNum, GPIO_INPUT);
    if (res == GPIO_OK) {
        createPulseInPinIfNeeded(pinNum);
        running = pulseInPin[pinNum]->isPulseInRunning();
    }

    return res;
}

GPIO_Result GPIOAccess::getFrequency(int pinNum, long int sampleTimeMS, long int &freq) {
    freq = -1L;
/**/
    class FreqHandler : public GPIO_Irq_Handler_Object {
    public:
        FreqHandler(int pinNumber, int cntLim) {
            pin = pinNumber;
            countLimit = cntLim;
            count = 0;
            done =false;
        }
        virtual void handleIrq(int pinNum, GPIO_Irq_Type type) {
            if ((pinNum == pin) && (type == GPIO_IRQ_RISING)) {
                if (!done) {
                    if (count == 0) {
                        startUS = timeNowUS();
                        count++;
                    } else if (count < countLimit) {
                        count++;
                    } else {
                        durationNS = ((timeNowUS() - startUS) * 1000LL) / (long long)countLimit;
                        done = true;
                    }
                }
            }
        };
        
        bool done;
        long long durationNS;
    private:
        int pin;
        int count;
        int countLimit;
        long long startUS;
        
    };
/*
    class FreqHandler : public GPIO_Irq_Handler_Object {
    public:
        FreqHandler(int pinNumber) {
            pin = pinNumber;
            intCount = 0;
        }
        virtual void handleIrq(int pinNum, GPIO_Irq_Type type) {
            if ((pinNum == pin) && (type == GPIO_IRQ_RISING)) {
                intCount++;
            }
        };
        
        long int intCount;
    private:
        int pin;
    };
*/
    FreqHandler * freqHandler = new FreqHandler(pinNum, sampleTimeMS);
    long long sampleNS = sampleTimeMS * 1000LL * 1000LL;

    GPIO_Result res = setIrq(pinNum, GPIO_IRQ_RISING, freqHandler, 0);
    if (res == GPIO_OK) {
        while (!freqHandler->done) {
            sleepMilli(1LL);
        }
        freq = 1000000000LL / freqHandler->durationNS;
//        sleepNano(sampleNS);
//        long int iCnt = freqHandler->intCount;
//        freq = (iCnt * 1000L) / sampleTimeMS;
        resetIrq(pinNum);
    }
    delete freqHandler;

    return res;
}

char * GPIOAccess::getLibVersion() {
    return (char *)gpioLibraryVersion;
}
