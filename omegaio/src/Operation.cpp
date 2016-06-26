#include "Operation.h"
#include "InfoOperation.h"
#include "SetOperation.h"
#include "ReadOperation.h"
#include "SetInputOperation.h"
#include "SetOutputOperation.h"
#include "GetDirectionOperation.h"
#include "PwmOperation.h"
#include "PwmStopOperation.h"
#include "ArduinoSystemOperation.h"
#include "ASRebootOperation.h"
#include "ASRetriesOperation.h"
#include "ASPinModeOperation.h"
#include "ASDigitalReadOperation.h"
#include "ASDigitalWriteOperation.h"
#include "ASAnalogRefOperation.h"
#include "ASAnalogReadOperation.h"
#include "ASAnalogWriteOperation.h"
#include "ASToneOperation.h"
#include "ASNoToneOperation.h"
#include "ASShiftInOperation.h"
#include "ASShiftOutOperation.h"
#include "ASPulseInOperation.h"
#include "ArduinoPortOperation.h"
#include "APRebootOperation.h"
#include "APRetriesOperation.h"
#include "APSendOperation.h"
#include "APSendBufOperation.h"
#include "APGetOperation.h"
#include "APGetBufOperation.h"
#include "IrqOperation.h"
#include "Irq2Operation.h"
#include "IrqStopOperation.h"
#include "ExpledOperation.h"
#include "ExpledStopOperation.h"
#include "ToneOperation.h"
#include "ToneStopOperation.h"
#include "ShiftOutOperation.h"
#include "ShiftInOperation.h"
#include "PulseOutOperation.h"
#include "PulseInOperation.h"
#include "FreqOperation.h"
#include "DelayOperation.h"
#include "AssignOperation.h"
#include "FileInOperation.h"
#include "FileOutOperation.h"
#include "FileDeleteOperation.h"
#include "ExecOperation.h"
#include "WhileOperation.h"
#include "IfOperation.h"
#include "ContinueOperation.h"
#include "BreakOperation.h"
#include "ExitOperation.h"
#include "I2CProbeOperation.h"
#include "I2CReadOperation.h"
#include "I2CReadBufOperation.h"
#include "I2CWriteOperation.h"
#include "I2CWriteBufOperation.h"

Operation::Operation(OperationType ot)
    : Node(nodeTypeOperation) {
    opType = ot;
}

string Operation::toString() {
    return "Operation: Type:" + mapFromOpType(opType);
}

map<string, OperationType> Operation::opTypeMap = {
// GPIO operations    
    {"info", opInfo},
    {"set", opSet},
    {"read", opRead},
    {"set-input", opSetInput},
    {"set-output", opSetOutput},
    {"get-direction", opGetDirection},
    {"pwm", opPwm},
    {"pwmstop", opPwmStop},
    {"irq", opIrq},
    {"irq2", opIrq2},
    {"irqstop", opIrqStop},
    {"tone", opTone},
    {"tonestop", opToneStop},
    {"shiftout", opShiftOut},
    {"shiftin", opShiftIn},
    {"pulseout", opPulseOut},
    {"pulsein", opPulseIn},
    {"frequency", opFreq},
    {"expled", opExpled},
    {"expledstop", opExpledStop},
    
// I2C operations
    {"i2cprobe", opI2CProbe},
    {"i2cread8", opI2CRead8},
    {"i2cread16", opI2CRead16},
    {"i2cread32", opI2CRead32},
    {"i2creadbuf", opI2CReadBuf},
    {"i2cwrite8", opI2CWrite8},
    {"i2cwrite16", opI2CWrite16},
    {"i2cwrite32", opI2CWrite32},
    {"i2cwritebuf", opI2CWriteBuf},

// Arduino operations
    {"arduinosys", opArduinoSystem},
    {"asreboot", opASReboot},    
    {"asretries", opASRetries},    
    {"aspinmode", opASPinMode},
    {"asdigitalread", opASDigitalRead},
    {"asdigitalwrite", opASDigitalWrite},
    {"asanalogref", opASAnalogRef},
    {"asanalogread", opASAnalogRead},
    {"asanalogwrite", opASAnalogWrite},
    {"astone", opASTone},
    {"asnotone", opASNoTone},
    {"asshiftout", opASShiftOut},
    {"asshiftin", opASShiftIn},
    {"aspulsein", opASPulseIn},
    {"arduinoport", opArduinoPort},
    {"apreboot", opAPReboot},
    {"apretries", opAPRetries},
    {"apsendcmd", opAPSendCommand},
    {"apsend8", opAPSend8},
    {"apsend16", opAPSend16},
    {"apsend32", opAPSend32},
    {"apsendbuf", opAPSendBuf},
    {"apgetstatus", opAPGetStatus},
    {"apget8", opAPGet8},
    {"apget16", opAPGet16},
    {"apget32", opAPGet32},
    {"apgetbuf", opAPGetBuf},
    
// Flow control operations
    {"while", opWhile},
    {"endwhile", opEndWhile},
    {"if", opIf},
    {"else", opElse},
    {"endif", opEndIf},
    {"continue", opContinue},
    {"break", opBreak},
    {"exit", opExit},

// File operations
    {"filein", opFileIn},
    {"fileout", opFileOut},
    {"filedelete", opFileDelete},

// Miscellaneous operations    
    {"delay", opDelay},
    {"exec", opExec},
    {"assign", opAssign},
};

OperationType Operation::mapOpType(string ot) {
    
    auto it = opTypeMap.find(ot);
    if (it == opTypeMap.end()) {
        return opInvalid;
    }
    
    return it->second;
}

string Operation::mapFromOpType(OperationType ot) {
    for (auto it : opTypeMap) {
        if (it.second == ot) {
            return it.first;
        }
    }
    
    return "";
}

Operation * Operation::create(AppInfo * appInfo, OperationType ot, list<string> * &paramList, list<string>::iterator * &paramIter) {
    Operation * theOp = NULL;
    
    switch (ot) {
        case opInfo:
            theOp = new InfoOperation();
            break;

        case opSet:
            theOp = new SetOperation();
            break;
            
        case opRead:
            theOp = new ReadOperation();
            break;

        case opSetInput:
            theOp = new SetInputOperation();
            break;

        case opSetOutput:
            theOp = new SetOutputOperation();
            break;

        case opGetDirection:
            theOp = new GetDirectionOperation();
            break;

        case opPwm:
            theOp = new PwmOperation();
            break;

        case opPwmStop:
            theOp = new PwmStopOperation();
            break;

        case opIrq:
            theOp = new IrqOperation();
            break;

        case opIrq2:
            theOp = new Irq2Operation();
            break;

        case opIrqStop:
            theOp = new IrqStopOperation();
            break;

        case opExpled:
            theOp = new ExpledOperation();
            break;

        case opExpledStop:
            theOp = new ExpledStopOperation();
            break;

        case opTone:
            theOp = new ToneOperation();
            break;

        case opToneStop:
            theOp = new ToneStopOperation();
            break;

        case opShiftOut:
            theOp = new ShiftOutOperation();
            break;

        case opShiftIn:
            theOp = new ShiftInOperation();
            break;

        case opPulseOut:
            theOp = new PulseOutOperation();
            break;

        case opPulseIn:
            theOp = new PulseInOperation();
            break;

        case opFreq:
            theOp = new FreqOperation();
            break;

        case opDelay:
            theOp = new DelayOperation();
            break;

        case opAssign:
            theOp = new AssignOperation();
            break;

        case opFileIn:
            theOp = new FileInOperation();
            break;

        case opFileOut:
            theOp = new FileOutOperation();
            break;

        case opFileDelete:
            theOp = new FileDeleteOperation();
            break;

        case opExec:
            theOp = new ExecOperation();
            break;

        case opArduinoSystem:
            theOp = new ArduinoSystemOperation();
            break;

        case opASReboot:
            theOp = new ASRebootOperation();
            break;

        case opASRetries:
            theOp = new ASRetriesOperation();
            break;

        case opASPinMode:
            theOp = new ASPinModeOperation();
            break;

        case opASDigitalRead:
            theOp = new ASDigitalReadOperation();
            break;

        case opASDigitalWrite:
            theOp = new ASDigitalWriteOperation();
            break;

        case opASAnalogRef:
            theOp = new ASAnalogRefOperation();
            break;

        case opASAnalogRead:
            theOp = new ASAnalogReadOperation();
            break;

        case opASAnalogWrite:
            theOp = new ASAnalogWriteOperation();
            break;

        case opASTone:
            theOp = new ASToneOperation();
            break;

        case opASNoTone:
            theOp = new ASNoToneOperation();
            break;

        case opASShiftIn:
            theOp = new ASShiftInOperation();
            break;

        case opASShiftOut:
            theOp = new ASShiftOutOperation();
            break;

        case opASPulseIn:
            theOp = new ASPulseInOperation();
            break;

        case opArduinoPort:
            theOp = new ArduinoPortOperation();
            break;

        case opAPReboot:
            theOp = new APRebootOperation();
            break;

        case opAPRetries:
            theOp = new APRetriesOperation();
            break;

        case opAPSendCommand:
            theOp = new APSendOperation(0);
            break;

        case opAPSend8:
            theOp = new APSendOperation(8);
            break;

        case opAPSend16:
            theOp = new APSendOperation(16);
            break;

        case opAPSend32:
            theOp = new APSendOperation(32);
            break;

        case opAPSendBuf:
            theOp = new APSendBufOperation();
            break;

        case opAPGetStatus:
            theOp = new APGetOperation(0);
            break;

        case opAPGet8:
            theOp = new APGetOperation(8);
            break;

        case opAPGet16:
            theOp = new APGetOperation(16);
            break;

        case opAPGet32:
            theOp = new APGetOperation(32);
            break;

        case opAPGetBuf:
            theOp = new APGetBufOperation();
            break;

        case opWhile:
            theOp = new WhileOperation();
            break;

        case opIf:
            theOp = new IfOperation();
            break;

        case opContinue:
            theOp = new ContinueOperation();
            break;

        case opBreak:
            theOp = new BreakOperation();
            break;

        case opExit:
            theOp = new ExitOperation();
            break;

        case opI2CProbe:
            theOp = new I2CProbeOperation();
            break;

        case opI2CRead8:
            theOp = new I2CReadOperation(8);
            break;

        case opI2CRead16:
            theOp = new I2CReadOperation(16);
            break;

        case opI2CRead32:
            theOp = new I2CReadOperation(32);
            break;

        case opI2CReadBuf:
            theOp = new I2CReadBufOperation();
            break;

        case opI2CWrite8:
            theOp = new I2CWriteOperation(8);
            break;

        case opI2CWrite16:
            theOp = new I2CWriteOperation(16);
            break;

        case opI2CWrite32:
            theOp = new I2CWriteOperation(32);
            break;

        case opI2CWriteBuf:
            theOp = new I2CWriteBufOperation();
            break;
    }
    
    if (theOp != NULL) {
        if (!theOp->build(appInfo, paramList, paramIter)) {
            delete theOp;
            theOp = NULL;
        }
    }
    
    return theOp;
}
