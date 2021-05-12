#ifndef DBGU_H
#define DBGU_H

#include <stdint.h>
#include <stdbool.h>
//#include <stdlib.h>
#include "include/AT91SAM9263.h"

#define CLK_FREQ 100000000

/*
enum ParityType
{
    EVEN = 0,
    ODD = 1 << 9,
    SPACE = 2 << 9,
    MARK = 3 << 9,
    NO_PARITY = 4 << 9
};

enum Mode
{
    NORMAL = 0,
    AUTOMATIC_ECHO = 1 << 14,
    LOCAL_LOOPBACK = 2 << 14,
    REMOTE_LOOPBACK = 3 << 14
};*/

int DBGU_Init(unsigned int parityType, unsigned int mode, unsigned int freq);
int DBGU_SendData(unsigned char *data);
int DBGU_ReadData(unsigned char *data, unsigned int size);
void DBGU_ReadAscii(unsigned char *byte);
void DBGU_SendAscii(unsigned char byte);

#endif