#include <avr/wdt.h>

#include "Arduino.h"

#include "OmegaAccess.h"
#include "Wire.h"
#include "OmegaAccessTypes.h"

unsigned char OmegaAccess::arduinoI2CAddr;
int OmegaAccess::omegaSigPin;
unsigned char OmegaAccess::sigPort;
bool OmegaAccess::haveSigData;
LinkData OmegaAccess::signalData;
ResponseData OmegaAccess::signalResponse;
unsigned char OmegaAccess::lastPortN;
OmegaPort * OmegaAccess::ports[10];

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
    for (i = 0; i < 10; i++) {
        ports[i] = NULL;
    }

    haveSigData = false;
    if (sigPin >= 0) {
        pinMode(sigPin, OUTPUT);
        digitalWrite(sigPin, LOW);
    }
        
    // disable watchdog timer
    wdt_disable();

    // join i2c bus 
    Wire.begin(i2cAddr);

    // register service routine
    Wire.onReceive(&onReceive);
    Wire.onRequest(&onRequest);    
}

Arduino_Result OmegaAccess::signalOmega(unsigned char portN, LinkData sigData) {
    if (omegaSigPin < 0) {
        return ARDUINO_BAD_SIG;
    }
    
    if (haveSigData) {
        return ARDUINO_SIG_PENDING;
    }
    
    sigPort = portN;
    signalData = sigData;
    haveSigData = true;
    
    digitalWrite(omegaSigPin, HIGH);
    
    return ARDUINO_OK;
}


Arduino_Result OmegaAccess::registerPort(OmegaPort * omegaPort) {
    if (omegaPort == NULL) {
        return ARDUINO_BAD_PORT;
    }
    
    int portN = omegaPort->getPortNumber();
    
    if ((portN < 0) || (portN > 9)) {
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
    if (linkDataIn.size <= 1) {
        return;
    }
    unsigned char portN = linkDataIn.data[0];
    unsigned char cmd = linkDataIn.data[1];
    
    if ((portN == 0xde) && (cmd == 0xad)) {
        // This is request for reset
        resetArduino();
        return;
    }

    if ((portN == 0xc0) && (cmd == 0x1d)) {
        // This is request for restart
        restartArduino();
        return;
    }

    LinkData linkData;
    int i = 2;
    for (i = 2; i < linkDataIn.size; i++) {
        linkData.data[i-2] = linkDataIn.data[i];
    }
    linkData.size = linkDataIn.size - 2;

    if ((portN >= 0) && (portN <= 9) && (ports[portN] != NULL)) {
        lastPortN = portN;
        ports[portN]->setResponseData(ports[portN]->handleCommand(cmd, linkData));
    } else if (portN == SIGNAL_PORT) {
        lastPortN = SIGNAL_PORT;
        signalResponse = handleSignalCommand(cmd, linkData);
    }
}

void OmegaAccess::getResponseData(LinkData & linkData) {
    ResponseData respData;
    if ((lastPortN >= 0) && (lastPortN <= 9) && (ports[lastPortN] != NULL)) {
        ports[lastPortN]->getResponseData(respData);
    } else if (lastPortN == SIGNAL_PORT) {
        respData = signalResponse;
    } else {
        respData.status = ARDUINO_BAD_PORT;
        respData.data[0] = lastPortN;
        respData.size = 1;
    }
    int i;
    linkData.data[0] = respData.status;
    for (i = 0; i < respData.size; i++) {
        linkData.data[i + 1] = respData.data[i];
    }
    linkData.size = respData.size + 1;
}

ResponseData OmegaAccess::handleSignalCommand(unsigned char cmd, LinkData linkData) {
    ResponseData sigResp;
    sigResp.status = ARDUINO_OK;
    sigResp.size = 0;

    switch (cmd) {
    case ARDUINO_SIG_GETSIGPORT:
        if ((linkData.size != 0) || (omegaSigPin < 0) || !haveSigData) {
            sigResp.status = ARDUINO_DATA_ERR;
        } else {
            sigResp.data[0] = sigPort;
            sigResp.size = 1;
        }
        break;
        
    case ARDUINO_SIG_GETSIGSZ:
        if ((linkData.size != 0) || (omegaSigPin < 0) || !haveSigData) {
            sigResp.status = ARDUINO_DATA_ERR;
        } else {
            sigResp.data[0] = signalData.size;
            sigResp.size = 1;
        }
        break;
        
    case ARDUINO_SIG_GETSIGDAT:
        if ((linkData.size != 0) || (omegaSigPin < 0) || !haveSigData) {
            sigResp.status = ARDUINO_DATA_ERR;
        } else {
            int i;
            for (i = 0; i < signalData.size; i++) {
                sigResp.data[i] = signalData.data[i];
            }
            sigResp.size = signalData.size;
            digitalWrite(omegaSigPin, LOW);
            haveSigData = false;
        }
        break;
        
    default:
        sigResp.status = ARDUINO_UNKNOWN_COMMAND;
        break;
    }

    return sigResp;
}

void OmegaAccess::resetArduino() {
    wdt_enable(WDTO_15MS);
    // Wait for timer signal
    while (true) {
    }
}

void(* reStart) (void) = 0; //declare reset function @ address 0

void OmegaAccess::restartArduino() {
    reStart();
}
