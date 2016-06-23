#ifndef OmegaPort_H
#define	OmegaPort_H

#include "OmegaAccessTypes.h"

class OmegaAccess;
class OmegaCommandPort;

class OmegaPort {
friend OmegaAccess;
friend OmegaCommandPort;
public:
    OmegaPort(unsigned char portN);
    
    int getPortNumber();
    
    virtual ResponseData processCommand(unsigned char cmd, LinkData linkData) = 0;

    virtual Arduino_Result signalOmega(LinkData sigData);
    
private:
    ResponseData handleCommand(unsigned char cmd, LinkData linkData);
    
    void setResponseData(ResponseData responseData);
    ResponseData getResponseData();
    void setSigResponseData(LinkData responseData);
    ResponseData getSigResponseData();

    int portNumber;
    
    ResponseData lastCommandResponse;
    ResponseData signalData;
    bool haveSigData;
};

#endif
