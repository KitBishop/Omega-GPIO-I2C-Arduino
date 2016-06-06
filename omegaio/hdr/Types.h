#ifndef Types_H
#define Types_H

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

// Flow control operations
    opWhile,
    opEndWhile,
    opIf, 
    opElse,
    opEndIf,
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
#define opFlowFirst opWhile
#define opFlowLast  opExit
#define opFileFirst opFileIn
#define opFileLast  opFileDelete
#define opMiscFirst opDelay
#define opMiscLast  opAssign

#endif
