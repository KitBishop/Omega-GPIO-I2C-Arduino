#ifndef OmegaIOTypes_H
#define OmegaIOTypes_H

enum NodeType {
    nodeTypeOption,
    nodeTypeOperation
};

enum OperationType {
// GPIO operations    
    opInfo,
    opSet,
    opRead,
    opSetInput,
    opSetOutput,
    opGetDirection,
    opPwm,
    opPwmStop,
    opIrq,
    opIrq2,
    opIrqStop,
    opTone,
    opToneStop,
    opShiftOut,
    opShiftIn,
    opPulseOut,
    opPulseIn,
    opFreq,
    opExpled,
    opExpledStop,
    
// I2C operations
    opI2CProbe,
    opI2CRead8,
    opI2CRead16,
    opI2CRead32,
    opI2CReadBuf,
    opI2CWrite8,
    opI2CWrite16,
    opI2CWrite32,
    opI2CWriteBuf,

// Arduino operations
    opArduinoSystem,
    opASReboot,    
    opASRetries,    
    opASPinMode,
    opASDigitalRead,
    opASDigitalWrite,
    opASAnalogRef,
    opASAnalogRead,
    opASAnalogWrite,
    opASTone,
    opASNoTone,
    opASShiftIn,
    opASShiftOut,
    opASPulseIn,
    opArduinoPort,
    opAPReboot,    
    opAPRetries, 
    opAPSendCommand,
    opAPSend8,
    opAPSend16,
    opAPSend32,
    opAPSendBuf,
    opAPGetStatus,
    opAPGet8,
    opAPGet16,
    opAPGet32,
    opAPGetBuf,
            
// Flow control operations
    opWhile,
    opEndWhile,
    opIf, 
    opElse,
    opEndIf,
    opContinue,
    opBreak,
    opExit,

// File operations
    opFileIn,
    opFileOut,
    opFileDelete,

// Miscellaneous operations    
    opDelay,
    opExec,
    opAssign,
    
// Invalid operation type
    opInvalid
};

#define opGPIOFirst opInfo
#define opGPIOLast  opExpledStop
#define opI2CFirst  opI2CProbe
#define opI2CLast   opI2CWriteBuf
#define opArduinoFirst  opArduinoSystem
#define opArduinoLast   opAPGetBuf
#define opFlowFirst opWhile
#define opFlowLast  opExit
#define opFileFirst opFileIn
#define opFileLast  opFileDelete
#define opMiscFirst opDelay
#define opMiscLast  opAssign

#endif
