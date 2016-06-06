#ifndef OmegaPort_H
#define	OmegaPort_H

#include "OmegaAccessTypes.h"

class OmegaAccess;

class OmegaPort {
friend OmegaAccess;
public:
    OmegaPort(unsigned char portN);
    
    int getPortNumber();
    
    virtual ResponseData processCommand(unsigned char cmd, LinkData linkData) = 0;

    Arduino_Result signalOmega(LinkData sigData);
    
private:
    ResponseData handleCommand(unsigned char cmd, LinkData linkData);
    
    void setResponseData(ResponseData responseData);
    void getResponseData(ResponseData & responseData);

    int portNumber;
    
    ResponseData lastCommandResponse;
};

#endif
