# **Omega-GPIO-I2C-Arduino**
A suite of libraries and programs that provide access on the Omega to GPIO pins, I2C devices and connected Arduino systems.


The components that make up the suite are:
* libnewgpio – a C++ library for access to Omega GPIO pins
* libnewi2c – a C++ library that provides Omega access to I2C devices
* libarduino – a C++ library that provides Omega access to Arduino systems connected via I2C; makes use of the libnewi2c library on the Omega and requires use of arduino-omega based code on the connected Arduino
* arduino_omega – source for an Arduino C++ library to run on an Arduino to allow access from the Omega libarduino code
* iotemplate – a template for a C++ program that can be used as the basis for user written code that makes use of libnewgpio and/or libnewi2c and/or libarduino libraries on the Omega
* i2cscan – a simple standalone program that scans for the existence of I2C devices connected to the Omega.  Makes use of the libnewi2c library
* gpioirq – a simple standalone program that triggers execution of user supplied commands on interrupt(s) from Omega GPIO pins.   Makes use of the libnewgpio library
* gpioexpled – a simple  standalone program for control of the Omega Expansion Dock LED.    Makes use of the libnewgpio library
* omegaio – a sophisticated standalone program that provides full command line access to the functionality in libnewgpio, libnewi2c, libarduino libraries on the Omega.  Provides some basic scripting facilities

See file **`Omega-GPIO-I2C-Arduino.pdf`** for overview documentation
