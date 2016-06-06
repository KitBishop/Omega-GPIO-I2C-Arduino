#include "GPIOPin.h"
#include "GPIOAccess.h"

GPIOPin::GPIOPin(int pinNum)
{
    pinNumber = pinNum;
}

GPIOPin::~GPIOPin(void)
{
    stopPWM();
}

GPIO_Result GPIOPin::setDirection(GPIO_Direction dir)
{
    return GPIOAccess::setDirection(pinNumber, dir);
}

GPIO_Result GPIOPin::getDirection(GPIO_Direction &dir)
{
    return GPIOAccess::getDirection(pinNumber, dir);
}

GPIO_Result GPIOPin::set(int value)
{
    bool running;
    GPIO_Result res = GPIOAccess::isPWMRunning(pinNumber, running);
    if (res == GPIO_OK) {
        if (running) {
            return GPIO_INVALID_OP;
        } else {
            return GPIOAccess::set(pinNumber, value);
        }
    }
    
    return res;
}

GPIO_Result GPIOPin::get(int &value)
{
    return GPIOAccess::get(pinNumber, value);
}

GPIO_Result GPIOPin::setIrq(GPIO_Irq_Type type, GPIO_Irq_Handler_Func handler, long int debounceMs) {
    return GPIOAccess::setIrq(pinNumber, type, handler, debounceMs);
}

GPIO_Result GPIOPin::setIrq(GPIO_Irq_Type type, GPIO_Irq_Handler_Object * handlerObj, long int debounceMs) {
    return GPIOAccess::setIrq(pinNumber, type, handlerObj, debounceMs);
}

GPIO_Result GPIOPin::resetIrq() {
    return GPIOAccess::resetIrq(pinNumber);
}

GPIO_Result GPIOPin::enableIrq() {
    return enableIrq(true);
}

GPIO_Result GPIOPin::disableIrq() {
    return enableIrq(false);
}

GPIO_Result GPIOPin::enableIrq(bool enable) {
    return GPIOAccess::enableIrq(pinNumber, enable);
}

GPIO_Result GPIOPin::irqEnabled(bool &enabled) {
    return GPIOAccess::irqEnabled(pinNumber, enabled);
}

GPIO_Irq_Type GPIOPin::getIrqType(){
    return GPIOAccess::getIrqType(pinNumber);
}

GPIO_Irq_Handler_Func GPIOPin::getIrqHandler() {
    return GPIOAccess::getIrqHandler(pinNumber);
}

GPIO_Irq_Handler_Object * GPIOPin::getIrqHandlerObj() {
    return GPIOAccess::getIrqHandlerObj(pinNumber);
}

GPIO_Result GPIOPin::setPWM(long int freq, int duty, int durationMs) {
    return GPIOAccess::setPWM(pinNumber, freq, duty, durationMs);
}

GPIO_Result GPIOPin::startPWM(int durationMs) {
    return GPIOAccess::startPWM(pinNumber, durationMs);
}

GPIO_Result GPIOPin::stopPWM() {
    return GPIOAccess::stopPWM(pinNumber);
}

GPIO_Result GPIOPin::getPWMFreq(long int &freq) {
    return GPIOAccess::getPWMFreq(pinNumber, freq);
}

GPIO_Result GPIOPin::getPWMDuty(int &duty) {
    return GPIOAccess::getPWMDuty(pinNumber, duty);
}

GPIO_Result GPIOPin::getPWMDuration(int &duration) {
    return GPIOAccess::getPWMDuration(pinNumber, duration);
}

GPIO_Result GPIOPin::isPWMRunning(bool &running) {
    return GPIOAccess::isPWMRunning(pinNumber, running);
}

int GPIOPin::getPinNumber() {
    return pinNumber;
}

GPIO_Result GPIOPin::setTone(long int freq, int durationMs) {
    return GPIOAccess::setTone(pinNumber, freq, durationMs);
}

GPIO_Result GPIOPin::startTone(int durationMs) {
    return GPIOAccess::startTone(pinNumber, durationMs);
}

GPIO_Result GPIOPin::stopTone() {
    return GPIOAccess::stopTone(pinNumber);
}

GPIO_Result GPIOPin::getToneFreq(long int &freq) {
    return GPIOAccess::getToneFreq(pinNumber, freq);
}

GPIO_Result GPIOPin::getToneDuration(int &duration) {
    return GPIOAccess::getToneDuration(pinNumber, duration);
}

GPIO_Result GPIOPin::isToneRunning(bool &running) {
    return GPIOAccess::isToneRunning(pinNumber, running);
}

GPIO_Result GPIOPin::pulseOut(long int pulseLenUS, int pulseLevel) {
    return GPIOAccess::pulseOut(pinNumber, pulseLenUS, pulseLevel);
}

GPIO_Result GPIOPin::pulseIn(int pulseLevel, long int timeoutUS, long int &val) {
    return GPIOAccess::pulseIn(pinNumber, pulseLevel, timeoutUS, val);
}

GPIO_Result GPIOPin::pulseIn(GPIO_PulseIn_Handler_Func handler, int pulseLevel, long int timeoutUS) {
    return GPIOAccess::pulseIn(pinNumber, handler, pulseLevel, timeoutUS);
}

GPIO_Result GPIOPin::pulseIn(GPIO_PulseIn_Handler_Object * handlerObj, int pulseLevel, long int timeoutUS) {
    return GPIOAccess::pulseIn(pinNumber, handlerObj, pulseLevel, timeoutUS);
}

GPIO_Result GPIOPin::stopPulseIn() {
    return GPIOAccess::stopPulseIn(pinNumber);
}

GPIO_Result GPIOPin::isPulseInRunning(bool &running) {
    return GPIOAccess::isPulseInRunning(pinNumber, running);
}

GPIO_Result GPIOPin::getFrequency(long int sampleTimeMS, long int &freq) {
    return GPIOAccess::getFrequency(pinNumber, sampleTimeMS, freq);
}
