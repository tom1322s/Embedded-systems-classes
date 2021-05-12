#ifndef DBGU_H
#define DBGU_H

#include <stdint.h>
#include <stdbool.h>
//#include <stdlib.h>
#include "include/AT91SAM9263.h"

#define CLK_FREQ 100000000
#define BUFFERSIZE 0xF

typedef struct FIFO
{
    char buffer[BUFFERSIZE + 1];
    unsigned int head;
    unsigned int tail;
} FIFO;

int DBGU_Init(unsigned int parityType, unsigned int mode, unsigned int freq);
int DBGU_SendData(unsigned char *data);
int DBGU_ReadData(unsigned char *data, unsigned int size);
void DBGU_ReadAscii(unsigned char *byte);
void DBGU_SendAscii(unsigned char byte);
void FIFO_init(FIFO *fifo);
int FIFO_push(FIFO *fifo, char data);
int FIFO_pop(FIFO *fifo, char *data);
void FIFO_empty(FIFO *fifo);
int DBGU_FIFO_SendData(unsigned char *data, FIFO *fifo);

#endif