#include <avr/wdt.h>

#include "Arduino.h"

#include "OmegaAccess.h"
#include "Wire.h"
#include "OmegaAccessTypes.h"
#include "OmegaCommandPort.h"

unsigned char OmegaAccess::arduinoI2CAddr;
int OmegaAccess::omegaSigPin;
unsigned int OmegaAccess::sigPorts;
unsigned int OmegaAccess::sigPortsPending;
bool OmegaAccess::haveSignalled;

OmegaCommandPort * OmegaAccess::commandPort = NULL;

unsigned char OmegaAccess::readPortN;

OmegaPort * OmegaAccess::ports[MAX_PORT + 1];

void OmegaAccess::begin() {
    setup(DEFAULT_ARDUINO_DEV_ADDR, -1);
}

void OmegaAccess::begin(unsigned char i2cAddr) {
    setup(i2cAddr, -1);
}

void OmegaAccess::begin(unsigned char i2cAddr, int sigPin) {
    setup(i2cAddr, sigPin);
}

void OmegaAccess::setup(unsigned char i2cAddr, int sigPin) {
    arduinoI2CAddr = i2cAddr;
    omegaSigPin = sigPin;

    int i;
    for (i = 0; i <= MAX_PORT; i++) {
        ports[i] = NULL;
    }


    if (sigPin >= 0) {
        pinMode(sigPin, OUTPUT);
        digitalWrite(sigPin, LOW);
    }
    
    sigPorts = 0;
    sigPortsPending = 0;
    haveSignalled = false;
    
    commandPort = new OmegaCommandPort();
        
    // disable watchdog timer
    wdt_disable();

    // join i2c bus 
    Wire.begin(i2cAddr);

    // register service routine
    Wire.onReceive(&onReceive);
    Wire.onRequest(&onRequest);    
}

Arduino_Result OmegaAccess::signalOmega(unsigned char portN, LinkData linkData) {
    if (omegaSigPin < 0) {
        return ARDUINO_BAD_SIG;
    }

    OmegaPort * thePort;
    if ((portN >= 0) && (portN <= MAX_PORT) && (ports[portN] != NULL)) {
        return ARDUINO_BAD_SIG;
    } else {
        thePort = ports[portN];
    }
    
    if (haveSignalled) {
        if ((sigPortsPending & (1 << portN)) != 0) {
            return ARDUINO_SIG_PENDING;
        } else {
            sigPortsPending = sigPortsPending | (1 << portN);
            thePort->setSigResponseData(linkData);
        }
    } else {
        if ((sigPorts & (1 << portN)) != 0) {
            return ARDUINO_SIG_PENDING;
        }

        sigPorts = sigPorts | (1 << portN);
        thePort->setSigResponseData(linkData);
        
        haveSignalled = true;

        digitalWrite(omegaSigPin, HIGH);
    }
    
    return ARDUINO_OK;
}


Arduino_Result OmegaAccess::registerPort(OmegaPort * omegaPort) {
    if (omegaPort == NULL) {
        return ARDUINO_BAD_PORT;
    }
    
    int portN = omegaPort->getPortNumber();
    
    if ((portN < 0) || (portN > MAX_PORT)) {
        return ARDUINO_BAD_PORT;
    }
    
    ports[portN] = omegaPort;
    
    return ARDUINO_OK;
}

void OmegaAccess::onReceive(int size) {
    LinkData linkData;

    // receive bytes from i2c bus
    int i = 0;
    while ((i < size) && (Wire.available() > 0)) {
        linkData.data[i] = Wire.read();
        i++;
    }
    linkData.size = i;
    
    handleReceived(linkData);
}

void OmegaAccess::onRequest() {
    LinkData linkData;
    
    getResponseData(linkData);
    
    if (linkData.size > 0) {
        Wire.write(linkData.data, linkData.size);
    }
}

void OmegaAccess::handleReceived(LinkData linkDataIn) {
    if (linkDataIn.size < 1) {
        // No data - nothing to do
        return;
    }

    // Extract port and command
    unsigned char portN = linkDataIn.data[0];
    unsigned char cmd = linkDataIn.data[1];
    
    if ((portN == 0xde) && (cmd == 0xad)) {
        // This is request for reset
        resetArduino();
        return;
    }

    // Extract the data for the command
    LinkData linkData;
    int i = 2;
    for (i = 2; i < linkDataIn.size; i++) {
        linkData.data[i-2] = linkDataIn.data[i];
    }
    linkData.size = linkDataIn.size - 2;

    if ((portN >= 0) && (portN <= MAX_PORT) && (ports[portN] != NULL)) {
        // Standard port command - process it in the port recording response
        ports[portN]->setResponseData(ports[portN]->handleCommand(cmd, linkData));
    } else if (portN == COMMAND_PORT) {
        // System port command - process it here
        if (cmd == ARDUINO_CMD_SETREADPORT) {
            commandPort->setReadPort(linkData);
        } else {
            commandPort->setResponseData(commandPort->handleCommand(cmd, linkData));
        }
    } else {
        // Unknown port - ignore
    }
}

void OmegaAccess::getResponseData(LinkData & linkData) {
    ResponseData respData;
    respData.size = 0;
    OmegaPort * thePort = NULL;
    
    if ((readPortN >= 0) && (readPortN <= MAX_PORT) && (ports[readPortN] != NULL)) {
        thePort = ports[readPortN];
    } else if (readPortN == COMMAND_PORT) {
        thePort = commandPort;
    } 
    
    if (thePort != NULL) {
        respData = thePort->getResponseData();
    } else {
        // No response port available - respond with error
        respData.status = ARDUINO_BAD_PORT;
    }
    
    int i;
    linkData.data[0] = respData.status;
    for (i = 0; i < respData.size; i++) {
        linkData.data[i + 1] = respData.data[i];
    }
    linkData.size = respData.size + 1;
}

void OmegaAccess::resetArduino() {
    wdt_enable(WDTO_15MS);
    // Wait for timer signal
    while (true) {
    }
}

void OmegaAccess::clearSignals() {
    digitalWrite(omegaSigPin, LOW);
    sigPorts = 0;
    haveSignalled = false;
    if (sigPortsPending != 0) {
        sigPorts = sigPortsPending;
        haveSignalled = true;
        digitalWrite(omegaSigPin, HIGH);
    }
}
