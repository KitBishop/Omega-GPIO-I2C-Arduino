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
        SignalData sigData;
        if (ArduinoAccess::getSignalData(arduinoDevAddr, sigData) == ARDUINO_OK) {
            ArduinoPort * port;
            auto portItem = accessInfo->ports.find(sigData.port);
            if (portItem != accessInfo->ports.end()) {
                port = portItem->second;
            } else {
                return;
            }
            
            if (port == NULL) {
                return;
            }
            
            if (port->sigHandler != NULL) {
                port->sigHandler(arduinoDevAddr, sigData.port, sigData.linkData);
            } else if (port->sigHandlerObj != NULL) {
                port->sigHandlerObj->handleSignalData(arduinoDevAddr, sigData.port, sigData.linkData);
            }
        }
    }
}

