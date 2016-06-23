#include <Arduino.h>
#include <avr/wdt.h>

#include "OmegaAccess.h"
#include "OmegaCommandPort.h"

OmegaCommandPort::OmegaCommandPort() 
    : OmegaPort(COMMAND_PORT) {
}

Arduino_Result OmegaCommandPort::signalOmega(LinkData sigData) {
    return ARDUINO_BAD_SIG;
}

ResponseData OmegaCommandPort::processCommand(unsigned char cmd, LinkData linkData) {
    ResponseData resp;
    resp.status = ARDUINO_OK;
    resp.size = 0;

    switch (cmd) {
    case ARDUINO_CMD_GETSIGNALPORTS:
        if (linkData.size != 0) {
            resp.status = ARDUINO_DATA_ERR;
        } else {
            resp.data[0] = OmegaAccess::sigPorts & 0xff;
            resp.data[1] = (OmegaAccess::sigPorts >> 8) & 0xff;
        }
        break;
            
    case ARDUINO_CMD_GETSIGSIZE:
        if (linkData.size != 1) {
            resp.status = ARDUINO_DATA_ERR;
        } else {
            unsigned char portN = linkData.data[0];
            
            if ((portN >= 0) && (portN <= MAX_PORT) && (OmegaAccess::ports[portN] != NULL)) {
                OmegaPort * thePort = OmegaAccess::ports[portN];
                if (!thePort->haveSigData) {
                    resp.status = ARDUINO_BAD_SIG;
                } else {
                    resp.data[0] = thePort->signalData.size;
                    resp.size = 1;
                }
            } else {
                resp.status = ARDUINO_BAD_PORT;
            }
        }
        break;
        
    case ARDUINO_CMD_GETSIGDATA:
        if (linkData.size != 1) {
            resp.status = ARDUINO_DATA_ERR;
        } else {
            unsigned char portN = linkData.data[0];
            
            if ((portN >= 0) && (portN <= MAX_PORT) && (OmegaAccess::ports[portN] != NULL)) {
                OmegaPort * thePort = OmegaAccess::ports[portN];
                if (!thePort->haveSigData) {
                    resp.status = ARDUINO_BAD_SIG;
                } else {
                    int i;
                    for (i = 0; i < thePort->signalData.size; i++) {
                        resp.data[i] = thePort->signalData.data[i];
                    }
                    resp.size = thePort->signalData.size;
                    thePort->haveSigData = false;
                }
            } else {
                resp.status = ARDUINO_BAD_PORT;
            }
        }
        break;
        
    case ARDUINO_CMD_CLEARSIGNALS:
        if (linkData.size != 0) {
            resp.status = ARDUINO_DATA_ERR;
        } else {
            OmegaAccess::clearSignals();
        }
        break;
            
    case ARDUINO_CMD_REBOOT:
        if (linkData.size != 0) {
            resp.status = ARDUINO_DATA_ERR;
        } else {
            rebootArduino();
        }
        break;
            
    default:
        resp.status = ARDUINO_UNKNOWN_COMMAND;
        break;
    }

    return resp;
}

void OmegaCommandPort::setReadPort(LinkData linkData) {
    if (linkData.size == 1) {
        OmegaAccess::readPortN = linkData.data[0];
    }
}

void(* reBoot) (void) = 0; //declare reset function @ address 0

void OmegaCommandPort::rebootArduino() {
    reBoot();
}
