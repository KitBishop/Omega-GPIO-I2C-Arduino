#ifndef OmegaAccess_H
#define OmegaAccess_H

#include "OmegaPort.h"

class OmegaAccess {
public:
    static void begin();
    static void begin(unsigned char i2cAddr);
    static void begin(unsigned char i2cAddr, int sigPin);
    
    static Arduino_Result registerPort(OmegaPort * omegaPort);
    
    static Arduino_Result signalOmega(unsigned char portN, LinkData sigData);
    
private: 
    static unsigned char arduinoI2CAddr;
    static int omegaSigPin;
    static unsigned char sigPort;
    static bool haveSigData;
    static LinkData signalData;
    static ResponseData signalResponse;
    static unsigned char lastPortN;
    static OmegaPort * ports[10];

    static ResponseData handleSignalCommand(unsigned char cmd, LinkData linkData);
    
    static void handleReceived(LinkData linkDataIn);
    static void getResponseData(LinkData & linkData);

    static void setup(unsigned char i2cAddr, int sigPin);
    
    static void resetArduino();
    static void restartArduino();

    static void onReceive(int size);
    static void onRequest();
};

#endif
