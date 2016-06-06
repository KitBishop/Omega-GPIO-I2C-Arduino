#include "GPIOPin.h"
#include "I2CDevice.h"
#include "ArduinoSystem.h"

int main(int argc, char** argv) {
    // The code for your main program
    
    // To access an Omega GPIO pin, create an instance of GPIOPin for the pin number 
    // and use methods on that instance
    // e.g.:
    //      GPIOPin * aPin = new GPIOPin(<pin-number>);
    
    GPIOPin * aPin = new GPIOPin(0);
    
    // To access an I2C device connected to the Omega, create an instance of I2CDevice for the i2c address
    // and use methods on that instance
    // e.g.:
    //      I2CDevice * i2cDev = new I2CDevice(<i2c-addr>);
    
    I2CDevice * i2cDev = new I2CDevice(0x10);
    
    // To access an Arduino connected to the Omega (via I2C), create an ArduinoSystem(or ArduinoPort) 
    // instance and use methods on that instance
    // Note: will require Arduino library from arduino-omega to be used in a sketch running
    // on the Arduino
    // e.g. for access to Arduino IO functions on an Arduino using default I2C address (0x08) and
    // default port number (0) from the Omega, use:
    
    ArduinoSystem * arduinoSys = new ArduinoSystem();
}

