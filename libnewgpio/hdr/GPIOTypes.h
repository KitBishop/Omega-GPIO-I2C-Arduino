#ifndef GPIOTypes_h
#define GPIOTypes_h

#ifdef __cplusplus
extern "C" {
#endif

enum GPIO_Result {
    GPIO_OK = 0,
    GPIO_BAD_ACCESS = 1,
    GPIO_INVALID_PIN = 2,
    GPIO_INVALID_OP = 3,
    GPIO_TIME_OUT = 4
};

enum GPIO_Direction {
    GPIO_INPUT = false,
    GPIO_OUTPUT = true
};

enum GPIO_Irq_Type {
    GPIO_IRQ_NONE = 0,
    GPIO_IRQ_RISING = 1,
    GPIO_IRQ_FALLING = 2,
    GPIO_IRQ_BOTH = 3
};

enum GPIO_Bit_Order {
    GPIO_LSB_FIRST = 0,
    GPIO_MSB_FIRST = 1
};

typedef void (*GPIO_Irq_Handler_Func) (int pinNum, GPIO_Irq_Type type);

class GPIO_Irq_Handler_Object {
public:
    virtual void handleIrq(int pinNum, GPIO_Irq_Type type) = 0;
};

typedef void (*GPIO_PulseIn_Handler_Func) (int pinNum, long int len);

class GPIO_PulseIn_Handler_Object {
public:
    virtual void handlePulseIn(int pinNum, long int len) = 0;
};

#ifdef __cplusplus
}
#endif

#endif /* GPIOTYPES_H */

