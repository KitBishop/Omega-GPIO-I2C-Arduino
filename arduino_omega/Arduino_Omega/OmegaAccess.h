#ifndef OmegaAccess_H
#define OmegaAccess_H

#include "OmegaPort.h"

#define MAX_PORT    15
#define COMMAND_PORT                255

class OmegaCommandPort;

class OmegaAccess {
friend OmegaCommandPort;
friend OmegaPort;
    
public:
    static void begin();
    static void begin(unsigned char i2cAddr);
    static void begin(unsigned char i2cAddr, int sigPin);
    
    static Arduino_Result registerPort(OmegaPort * omegaPort);
    
private: 
    static OmegaCommandPort * commandPort;
    
    static unsigned char arduinoI2CAddr;
    static int omegaSigPin;
    
    static unsigned int sigPorts;
    static unsigned int sigPortsPending;
    static bool haveSignalled;
    
    static unsigned char readPortN;
    
    static OmegaPort * ports[MAX_PORT + 1];

    static Arduino_Result signalOmega(unsigned char portN, LinkData linkData);
    static void clearSignals();

    static void handleReceived(LinkData linkDataIn);
    static void getResponseData(LinkData & linkData);

    static void setup(unsigned char i2cAddr, int sigPin);
    
    static void onReceive(int size);
    static void onRequest();

    static void resetArduino();
};

#endif
