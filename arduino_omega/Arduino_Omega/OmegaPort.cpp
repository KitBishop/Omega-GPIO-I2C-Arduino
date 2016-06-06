#include <Arduino.h>
#include "OmegaPort.h"
#include "OmegaAccess.h"

OmegaPort::OmegaPort(unsigned char portN) {
    portNumber = portN;
    lastCommandResponse.size = 0;
    lastCommandResponse.status = ARDUINO_OK;
}

int OmegaPort::getPortNumber() {
    return portNumber;
}

void OmegaPort::getResponseData(ResponseData & respData) {
    respData = lastCommandResponse;
}

void OmegaPort::setResponseData(ResponseData responseData) {
    lastCommandResponse = responseData;
}

ResponseData OmegaPort::handleCommand(unsigned char cmd, LinkData linkData) {
    return processCommand(cmd, linkData);
}

Arduino_Result OmegaPort::signalOmega(LinkData sigData) {
    return OmegaAccess::signalOmega(portNumber, sigData);
}


