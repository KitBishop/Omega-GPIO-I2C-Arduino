#include "Option.h"
#include "Utilities.h"
#include "InfoOperation.h"
#include "SetOperation.h"
#include "ReadOperation.h"
#include "SetInputOperation.h"
#include "SetOutputOperation.h"
#include "GetDirectionOperation.h"
#include "PwmOperation.h"
#include "PwmStopOperation.h"
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
#include "BreakOperation.h"
#include "ExitOperation.h"
#include "I2CProbeOperation.h"
#include "I2CReadOperation.h"
#include "I2CReadBufOperation.h"
#include "I2CWriteOperation.h"
#include "I2CWriteBufOperation.h"

#include "GPIOAccess.h"
#include "I2CAccess.h"
#include "ArduinoAccess.h"

Option::Option(char optC, OptOper op, string * param)
: Node(nodeTypeOption) {
    optChar = optC;
    optParam = param;
    oper = op;
}

string Option::toString() {
    stringstream s;
    s << "Option: OptionChar:";
    s << optChar << " OptionOperation:";
    if (oper == on) {
        s << "On";
    } else if (oper == off) {
        s << "Off";
    } else {
        s << "Param OptionParameter:";
        if (optParam == NULL) {
            s << "NULL";
        } else {
            s << *optParam;
        }
    }
    return s.str();
}

Option * Option::create(AppInfo * appInfo, string optStr) { 
    Option * opt = NULL;
    char optC;
    OptOper oper;
    string * optParam = NULL;
    bool optOk = true;

    if (optStr.length() >= 2) {
        optC = optStr.at(1);
        switch (optC) {
            case 'v':
            case 'q':
            case 'o':
            case 'r':
            case 'e':
            case 'i':
            case 'x':
            case 's':
                if (optStr.length() == 2) {
                    oper = on;
                } else if (optStr.length() == 3) {
                    if (optStr.at(2) == '+') {
                        oper = on;
                    } else if (optStr.at(2) == '-') {
                        oper = off;
                    } else {
                        optOk = false;
                    }
                } else {
                    optOk = false;
                }
                break;

            case '?':
                appInfo->haveOp = true;
                if (optStr.length() == 2) {
                    oper = on;
                } else {
                    optOk = false;
                }
                break;

            case 'h':
                appInfo->haveOp = true;
                if (optStr.length() == 2) {
                    oper = on;
                } else if (optStr.length() > 3) {
                    if (optStr.at(2) == ':') {
                        oper = param;
                        optParam = new string(optStr.substr(3, string::npos));

                        if (optParam->at(0) == '-') {
                            if (optParam->length() != 2) {
                                optOk = false;
                            } else {
                                char hC = optParam->at(1);
                                switch (hC) {
                                    case 'v':
                                    case 'q':
                                    case 'o':
                                    case 'r':
                                    case 'e':
                                    case 'i':
                                    case 's':
                                    case 'x':
                                    case 'd':
                                    case 'h':
                                    case '?':
                                        break;

                                    default:
                                        optOk = false;
                                }
                            }
                        }
                    } else {
                        optOk = false;
                    }
                } else {
                    optOk = false;
                }
                break;

            default:
                optOk = false;
                break;
        }
    } else {
        optOk = false;
    }

    if (optOk) {
        opt = new Option(optC, oper, optParam);
    } else {
        prterr("Invalid option:" + optStr);
    }

    return opt;
}

bool Option::execute(AppInfo * appInfo) {
    if (appInfo->breaking) {
        return true;
    }

    bool setting;
    switch (optChar) {
        case 'v':
            setting = oper == on;
            appInfo->output = setting;
            appInfo->report = setting;
            appInfo->errors = setting;
            break;

        case 'q':
            setting = oper == off;
            appInfo->output = setting;
            appInfo->report = setting;
            appInfo->errors = setting;
            break;

        case 'o':
            setting = oper == on;
            appInfo->output = setting;
            break;

        case 'r':
            setting = oper == on;
            appInfo->report = setting;
            break;

        case 'i':
            setting = oper == on;
            appInfo->ignore = setting;
            break;

        case 'e':
            setting = oper == on;
            appInfo->errors = setting;
            break;

        case 's':
            setting = oper == on;
            appInfo->setdir = setting;
            break;

        case 'x':
            setting = oper == on;
            appInfo->hexout = setting;
            break;

        case '?':
        {
            string bStr = basicHelp(appInfo);
            bool apbRep = appInfo->report;
            appInfo->report = true;
            appInfo->prtReport(bStr);
            appInfo->report = apbRep;
        }
            break;

        case 'h':
        {
            bool apRep = appInfo->report;
            appInfo->report = true;

            stringstream hStr;
            if ((optParam == NULL) || optParam->empty() || (optParam->compare("all") == 0)) {
                hStr << progInfoHelp();
                hStr << "\nUsage:\n";
                hStr << "======\n    " << appInfo->appName << " [any length sequence of space separated <input-element>s]";
                hStr << "\nAn <input-element> is one of:\n\t<option>\n\t<file-input>\n\t<operation>\n\n";
                stringstream optHstr;
                optHstr << optionHelp('v') << '\n'
                        << optionHelp('q') << '\n'
                        << optionHelp('o') << '\n'
                        << optionHelp('r') << '\n'
                        << optionHelp('e') << '\n'
                        << optionHelp('i') << '\n'
                        << optionHelp('s') << '\n'
                        << optionHelp('x') << '\n'
                        << optionHelp('d') << '\n'
                        << optionHelp('h') << '\n'
                        << optionHelp('?');
                string s = optHstr.str();
                replaceStringInPlace(s, "\n", "\n\t");
                optHstr.str("");
//                replaceStringInPlace(optHstr, "\n", "\n\t");
                optHstr << "An <option> is one of:\n\t" << s << "\n";

                stringstream atHstr;
                s = atHelp();
                replaceStringInPlace(s, "\n", "\n\t");
                atHstr << "A <file-input> is:\n\t" << s << "\n";

                stringstream opHstr;
                opHstr << "An <operation> is one of:";
                
                opHstr << "\n    " << fullHelpBlock("GPIO Operations:", opGPIOFirst, opGPIOLast);
                opHstr << "\n\n    " << fullHelpBlock("I2C Operations:", opI2CFirst, opI2CLast);
                opHstr << "\n\n    " << fullHelpBlock("Flow Control Operations:", opFlowFirst, opFlowLast);
                opHstr << "\n\n    " << fullHelpBlock("File Operations:", opFileFirst, opFileLast);
                opHstr << "\n\n    " << fullHelpBlock("Miscellaneous Operations:", opMiscFirst, opMiscLast);
                
                opHstr << "\n\nMany operation parameters are represented by an expression.";
                opHstr << "\n    Expressions:" << replaceString("\n" + Expression::help(), "\n", "\n\t") << "\n";
                
                hStr << optHstr.str() << "\n" << atHstr.str() << "\n" << opHstr.str();
                
                hStr << "\n" << sourcesHelp();
            } else if (optParam->at(0) == '-') {
                if (optParam->length() != 2) {
                    appInfo->prtError(opInvalid, "Invalid -h parameter:" + *optParam);
                    return false;
                }
                char hC = optParam->at(1);
                switch (hC) {
                    case 'v':
                    case 'q':
                    case 'o':
                    case 'r':
                    case 'i':
                    case 'e':
                    case 'x':
                    case 's':
                    case 'h':
                    case 'd':
                    case '?':
                        hStr << optionHelp(hC);
                        break;

                    default:
                        appInfo->prtError(opInvalid, "Invalid -h parameter:" + *optParam);
                        return false;
                }
            } else if (optParam->at(0) == '@') {
                if (optParam->length() != 1) {
                    appInfo->prtError(opInvalid, "Invalid -h parameter:" + *optParam);
                    return false;
                }
                hStr << atHelp();
            } else if (optParam->compare("expression") == 0) {
                hStr << Expression::help();
            } else {
                OperationType opType = Operation::mapOpType(*optParam);
                switch (opType) {
                    case opInfo:
                    case opSet:
                    case opRead:
                    case opSetInput:
                    case opSetOutput:
                    case opGetDirection:
                    case opPwm:
                    case opPwmStop:
                    case opIrq:
                    case opIrq2:
                    case opIrqStop:
                    case opExpled:
                    case opExpledStop:
                    case opTone:
                    case opToneStop:
                    case opShiftOut:
                    case opShiftIn:
                    case opPulseOut:
                    case opPulseIn:
                    case opFreq:
                    case opDelay:
                    case opAssign:
                    case opFileIn:
                    case opFileOut:
                    case opFileDelete:
                    case opExec:
                    case opWhile:
                    case opEndWhile:
                    case opIf:
                    case opElse:
                    case opEndIf:
                    case opBreak:
                    case opExit:
                    case opI2CProbe:
                    case opI2CRead8:
                    case opI2CRead16:
                    case opI2CRead32:
                    case opI2CReadBuf:
                    case opI2CWrite8:
                    case opI2CWrite16:
                    case opI2CWrite32:
                    case opI2CWriteBuf:
                        hStr << operationHelp(opType);
                        break;

                    default:
                        appInfo->prtError(opType, "Invalid -h parameter:" + *optParam);
                        return false;
                }
            }
            appInfo->prtReport(hStr.str());
            appInfo->report = apRep;
        }
            break;
    }

    return true;
}

string Option::operationHelp(OperationType opType) {
    string hStr;
    switch (opType) {
        case opInfo:
            hStr = InfoOperation::help();
            break;

        case opSet:
            hStr = SetOperation::help();
            break;

        case opRead:
            hStr = ReadOperation::help();
            break;

        case opSetInput:
            hStr = SetInputOperation::help();
            break;

        case opSetOutput:
            hStr = SetOutputOperation::help();
            break;

        case opGetDirection:
            hStr = GetDirectionOperation::help();
            break;

        case opPwm:
            hStr = PwmOperation::help();
            break;

        case opPwmStop:
            hStr = PwmStopOperation::help();
            break;

        case opIrq:
            hStr = IrqOperation::help();
            break;

        case opIrq2:
            hStr = Irq2Operation::help();
            break;

        case opIrqStop:
            hStr = IrqStopOperation::help();
            break;

        case opExpled:
            hStr = ExpledOperation::help();
            break;

        case opExpledStop:
            hStr = ExpledStopOperation::help();
            break;

        case opTone:
            hStr = ToneOperation::help();
            break;

        case opToneStop:
            hStr = ToneStopOperation::help();
            break;

        case opShiftOut:
            hStr = ShiftOutOperation::help();
            break;

        case opShiftIn:
            hStr = ShiftInOperation::help();
            break;

        case opPulseOut:
            hStr = PulseOutOperation::help();
            break;

        case opPulseIn:
            hStr = PulseInOperation::help();
            break;

        case opFreq:
            hStr = FreqOperation::help();
            break;

        case opDelay:
            hStr = DelayOperation::help();
            break;

        case opAssign:
            hStr = AssignOperation::help();
            break;

        case opFileIn:
            hStr = FileInOperation::help();
            break;

        case opFileOut:
            hStr = FileOutOperation::help();
            break;

        case opFileDelete:
            hStr = FileDeleteOperation::help();
            break;

        case opExec:
            hStr = ExecOperation::help();
            break;

        case opWhile:
        case opEndWhile:
            hStr = WhileOperation::help();
            break;

        case opIf:
        case opElse:
        case opEndIf:
            hStr = IfOperation::help();
            break;

        case opBreak:
            hStr = BreakOperation::help();
            break;

        case opExit:
            hStr = ExitOperation::help();
            break;

        case opI2CProbe:
            hStr = I2CProbeOperation::help();
            break;

        case opI2CRead8:
        case opI2CRead16:
        case opI2CRead32:
            hStr = I2CReadOperation::help();
            break;

        case opI2CReadBuf:
            hStr = I2CReadBufOperation::help();
            break;

        case opI2CWrite8:
        case opI2CWrite16:
        case opI2CWrite32:
            hStr = I2CWriteOperation::help();
            break;

        case opI2CWriteBuf:
            hStr = I2CWriteBufOperation::help();
            break;
    }

    return hStr;

}

string Option::optionHelp(char optC) {
    stringstream hStr;
    switch (optC) {
        case 'v':
            hStr << "-v and -v+ - sets verbose mode; equivalent to -o -r -e\n";
            hStr << "-v- - resets verbose mode; equivalent to -q\n";
            hStr << "\tDefaults are: -o+ -r- -e+";
            break;

        case 'q':
            hStr << "-q and -q+ - sets quiet mode; equivalent to -o- -r- -e-\n";
            hStr << "-q- - resets quiet mode; equivalent to -v";
            break;

        case 'o':
            hStr << "-o and -o+ - enables output to stdout of any results of operation\n";
            hStr << "-o- - disables output to stdout of any results of operation\n";
            hStr << "\tDefault is: -o+";
            break;

        case 'r':
            hStr << "-r and -r+ - enables output to stderr of report on actions taken\n";
            hStr << "-r- - disables output to stderr of report on actions taken\n";
            hStr << "\tDefault is: -r-";
            break;

        case 'i':
            hStr << "-i and -i+ - enables ignoring of any errors during processing\n";
            hStr << "-i- - disables ignoring of any errors during processing\n";
            hStr << "\tDefault is: -i-";
            break;

        case 'e':
            hStr << "-e and -e+ - enables output to stderr of any errors during processing\n";
            hStr << "-e- - disables output to stderr of any errors during processing\n";
            hStr << "\tDefault is: -e+";
            break;

        case 's':
            hStr << "-s and -s+ - causes the program to ensure that the pin direction";
            hStr << "\n\tis set appropriately for each operation\n";
            hStr << "-s- - does not set the direction for each operation\n";
            hStr << "\tDefault is: -s-";
            break;

        case 'x':
            hStr << "-x and -x+ - sets output to be displayed in hex\n";
            hStr << "-x- - disables hex output, output is in decimal\n";
            hStr << "\tDefault is: -x-";
            break;

        case 'h':
            hStr << "-h or -h:all - displays all available help\n";
            hStr << "-h:-<option-letter> - displays help for the given option letter\n";
            hStr << "-h:@ - displays help for input from file\n";
            hStr << "-h:<operation> - displays help for the given operation\n";
            hStr << "-h:expression - displays help for expressions";
            break;

        case 'd':
            hStr << "-d - enables debugging output on entered data\n";
            hStr << "\tWhen used anywhere in the input causes debugging output\n";
            hStr << "\tto be displayed on scanned input and processed operation data\n";
            hStr << "\tprior to any execution.";
            hStr << "\tBy default, debugging output is disabled";
            break;

        case '?':
            hStr << "-? - displays basic usage help";
            break;
    }

    return hStr.str();
}

string Option::fullHelpBlock(string head, OperationType op1, OperationType op2) {
    stringstream hStr;
    hStr << head;
    
    int opI;
    OperationType  op;
    for (opI = op1; opI <= op2; opI++) {
        op = (OperationType)opI;
        if ((op != opEndWhile) 
                && (op != opElse) 
                && (op != opEndIf)
                && (op != opI2CRead16)
                && (op != opI2CRead32)
                && (op != opI2CWrite16)
                && (op != opI2CWrite32)) {
            hStr << replaceString("\n" + operationHelp(op), "\n", "\n\t");
        }
    }
    
    return hStr.str();
}

string Option::basicHelpBlock(string head, OperationType op1, OperationType op2) {
    stringstream hStr;
    hStr << "\t      " << head << ":\n";
    
    int opI;
    OperationType  op;
    int n = 0;
    for (opI = op1; opI <= op2; opI++) {
        op = (OperationType)opI;
        string hs;
        if ((n % 4) == 0) {
            hStr << "\t";
        }
        hs = Operation::mapFromOpType(op);
        hStr << "\t" << hs;
        if (hs.length() < 8) {
            hStr << "\t";
        }
        n++;
        if (((n % 4) == 0) || (op == op2)) {
            hStr << "\n";
        }
    }
    
    return hStr.str();
}

string Option::basicHelp(AppInfo * appInfo) {
    stringstream hStr;
    hStr << progInfoHelp();
    hStr << "\nBasic Usage:\n";
    hStr << "============\n    " << appInfo->appName << " [any length sequence of space separated <input-element>s]\n";
    hStr << "An <input-element> is one of:\n\t<option>\n\t<file-input>\n\t<operation>\n";
    hStr << "An <option> is one of:\n";
    hStr << "\t-v - verbose output\n";
    hStr << "\t-q - quiet output\n";
    hStr << "\t-o - result output\n";
    hStr << "\t-r - report output\n";
    hStr << "\t-i - ignore errors\n";
    hStr << "\t-e - error output\n";
    hStr << "\t-s - automatic setting of pin direction\n";
    hStr << "\t-x - hex output\n";
    hStr << "\t-h - various levels of help output\n";
    hStr << "\t-? - this basic usage output\n";
    hStr << "A <file-input> is of the form:\n";
    hStr << "\t@<file-name> - input commands from file\n";
    hStr << "An <operation> is of the form:\n";
    hStr << "\t<operation-name> <operation-parameters>\n";
    hStr << "\t    An <operation-name> is one of:\n";

    hStr << basicHelpBlock("GPIO Operations:", opGPIOFirst, opGPIOLast);
    hStr << basicHelpBlock("I2C Operations:", opI2CFirst, opI2CLast);
    hStr << basicHelpBlock("Flow Control Operations:", opFlowFirst, opFlowLast);
    hStr << basicHelpBlock("File Operations:", opFileFirst, opFileLast);
    hStr << basicHelpBlock("Miscellaneous Operations:", opMiscFirst, opMiscLast);
    
    hStr << "\t    The <operation-parameters> depend on the specific operation\n\n";
    hStr << "More information can be displayed by using one of:\n";
    hStr << "\t-h or -h:all - for all help\n";
    hStr << "\t-h:-<option-letter> - for help on the option\n";
    hStr << "\t-h:@ - for help on file input\n";
    hStr << "\t-h:<operation-name> - for help on the operation and its parameters\n";
    hStr << "\t-h:expression - for help on expressions\n\n";
    hStr << sourcesHelp();

    return hStr.str();
}

string Option::atHelp() {
    stringstream ss;
    ss << "@<file-name> - causes input to be taken from the given file";
    ss << "\n\tInput is free form sequence of space separated standard";
    ss << "\n\t\tinput elements: <option> <file-input> <operation>";
    ss << "\n\tAny line with first non-blank character of # is ignored";
    return ss.str();
}

string Option::progInfoHelp() {
    stringstream hStr;
    hStr << "A C++ program to control and interact with Omega GPIO pins,";
    hStr << "\n    I2C devices, and Arduino access via scripted operations.";
    hStr << "\n\tProgram version:         " << applicationVersion;
    hStr << "\n\tGPIO Library version:    " << GPIOAccess::getLibVersion();
    hStr << "\n\tI2C Library version:     " << I2CAccess::getLibVersion();
    hStr << "\n\tArduino Library version: " << ArduinoAccess::getLibVersion() << "\n";
    return hStr.str();
}

string Option::sourcesHelp() {
    string hStr = "Sources available at: https://github.com/KitBishop/Omega-IO\n";
    return hStr;
}
