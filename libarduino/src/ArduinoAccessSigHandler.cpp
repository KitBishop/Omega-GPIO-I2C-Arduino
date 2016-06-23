#include "ArduinoAccessSigHandler.h"
#include "ArduinoPort.h"

ArduinoAccessSigHandler::ArduinoAccessSigHandler(unsigned char devAddr) {
    arduinoDevAddr = devAddr;
    auto infItem = ArduinoAccess::portInfo.find(devAddr);
    if (infItem != ArduinoAccess::portInfo.end()) {
        accessInfo = infItem->second;
    } else {
        accessInfo = NULL;
    }
}

void ArduinoAccessSigHandler::handleIrq(int pinNum, GPIO_Irq_Type type) {
    if (accessInfo == NULL) {
        return;
    }
    if (accessInfo->signalPin == NULL) {
        return;
    }

    if ((pinNum == accessInfo->signalPin->getPinNumber()) 
            && (type == GPIO_Irq_Type::GPIO_IRQ_RISING)) {
        Arduino_Result aRes;
        SignalData sigData;
        ArduinoPort * port;
        unsigned int sigPorts;
        aRes = ArduinoAccess::getSigPorts(arduinoDevAddr, sigPorts);
        if ((aRes == ARDUINO_OK) && (sigPorts != 0)) {
            int portN;
            for (portN = 0; portN <= MAX_PORT; portN++) {
                if ((sigPorts & (1 << portN)) != 0) {
                    auto portItem = accessInfo->ports.find(portN);
                    if (portItem != accessInfo->ports.end()) {
                        port = portItem->second;
                        
                        if (port != NULL) {
                            aRes = ArduinoAccess::getSignalDataInternal(arduinoDevAddr, portN, sigData);
                            if (aRes == ARDUINO_OK) {
                                if (port->sigHandler != NULL) {
                                    port->sigHandler(arduinoDevAddr, sigData.port, sigData.linkData);
                                } else if (port->sigHandlerObj != NULL) {
                                    port->sigHandlerObj->handleSignalData(arduinoDevAddr, sigData.port, sigData.linkData);
                                }
                            }
                        }
                    }                    
                }
            }
        }
        ArduinoAccess::clearSignal(arduinoDevAddr);
    }
}

