#include <Arduino.h>
#include "OmegaPort.h"
#include "OmegaAccess.h"

OmegaPort::OmegaPort(unsigned char portN) {
    portNumber = portN;
    lastCommandResponse.size = 0;
    lastCommandResponse.status = ARDUINO_OK;
    signalData.size = 0;
    haveSigData = false;
}

int OmegaPort::getPortNumber() {
    return portNumber;
}

ResponseData OmegaPort::getResponseData() {
    return lastCommandResponse;
}

void OmegaPort::setResponseData(ResponseData responseData) {
    lastCommandResponse = responseData;
}

ResponseData OmegaPort::handleCommand(unsigned char cmd, LinkData linkData) {
    return processCommand(cmd, linkData);
}

Arduino_Result OmegaPort::signalOmega(LinkData sigData) {
    if (haveSigData) {
        return ARDUINO_SIG_PENDING;
    }
    
    return OmegaAccess::signalOmega(portNumber, sigData);
}

void OmegaPort::setSigResponseData(LinkData responseLinkData) {
    signalData.status = ARDUINO_OK;
    int i;
    for (i = 0; i < responseLinkData.size; i++) {
        signalData.data[i] = responseLinkData.data[i];
    }
    signalData.size = responseLinkData.size;
    haveSigData = true;
}

ResponseData OmegaPort::getSigResponseData(){
    return signalData;
}
