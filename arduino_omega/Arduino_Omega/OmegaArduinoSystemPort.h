#ifndef OmegaArduinoSystemPort_H
#define	OmegaArduinoSystemPort_H

#include "OmegaPort.h"
#include "OmegaAccessTypes.h"

class OmegaArduinoSystemPort : public OmegaPort {
public:
    OmegaArduinoSystemPort();
    OmegaArduinoSystemPort(unsigned char portN);
    
    virtual ResponseData processCommand(unsigned char cmd, LinkData linkData);
};

#endif

