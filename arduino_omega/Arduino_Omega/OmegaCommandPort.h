#ifndef OmegaCommandPort_H
#define	OmegaCommandPort_H

#include "OmegaPort.h"
#include "OmegaAccessTypes.h"

#define ARDUINO_CMD_GETSIGNALPORTS      250
#define ARDUINO_CMD_SETREADPORT         251
#define ARDUINO_CMD_GETSIGSIZE          252
#define ARDUINO_CMD_GETSIGDATA          253
#define ARDUINO_CMD_CLEARSIGNALS        254
#define ARDUINO_CMD_REBOOT              255

class OmegaAcess;

class OmegaCommandPort : public OmegaPort {
friend OmegaAccess;    

public:
    OmegaCommandPort();
    
    virtual ResponseData processCommand(unsigned char cmd, LinkData linkData);

    virtual Arduino_Result signalOmega(LinkData sigData);
    
private:
    void rebootArduino();
    
    void setReadPort(LinkData linkData);
};

#endif

