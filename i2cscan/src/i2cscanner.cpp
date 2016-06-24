#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "I2CDevice.h"
//#include "I2CAccess.h"

using namespace std;

void usage(char * nm) {
    printf("Usage\n");
    printf("Command:\n");
    printf("\t%s <i2c-chan>\n",nm);
    printf("\t\tScan for all I2C devices with addresses from 0x03 to 0x7f\n");
}

bool processArgs(int argc, char** argv) {
    if (argc > 1) {
        return false;
    }
    
    return true;
}

void reportOne(char val, I2C_Result r) {
    if (r == I2C_OK) {
        printf("%c", val);
    } else {
        printf(".");
    }
}

int main(int argc, char** argv) {
    if (!processArgs(argc, argv)) {
        usage(argv[0]);
        return -1;
    }
    
    int i;
    int j;
    int first = 0x03;
    int last = 0x7f;
    I2C_Result r;
    
    printf("Scanning I2C\n");
    printf("Result:RWQ where R=r=read; W=w=write; Q=q=quick probe; .=no valid response\n");
    
    printf("    0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f\n");
    printf("    --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---\n");

    for (i = 0; i < 128; i += 16) {
        printf("%02x: ", i);
        for(j = 0; j < 16; j++) {
            fflush(stdout);

            if ((i+j < first) || (i+j > last)) {
                printf("    ");
                continue;
            }

            I2CDevice * dev = new I2CDevice(i + j);

            unsigned char v = 255;
            r = dev->read8(v);
            reportOne('r', r);

            r = dev->write8(0);
            reportOne('w', r);
            
            r = dev->probe();
            reportOne('q', r);
        
            printf(" ");
            fflush(stdout);

            delete dev;
        }
        
        printf("\n");
    }
    
    return 0;
}

