#include <Arduino.h>
#include <OmegaArduinoSystemPort.h>
#include <OmegaAccess.h>
#include <OmegaPort.h>
#include <OmegaAccessTypes.h>

void setup() {
// Set up Omega access for default access with I2C address 0x08
// using access to Arduino System pin functionality on default port 0
//
// See documentation at:
//    https://github.com/KitBishop/Omega-GPIO-I2C-Arduino/tree/master/arduino-omega/arduino-omega.pdf
// for full details
    OmegaAccess::begin();
    OmegaAccess::registerPort(new OmegaArduinoSystemPort());
}

void loop() {
}
