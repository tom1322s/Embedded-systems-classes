#include "DBGU.h"

int DBGU_Init(unsigned int parityType, unsigned int mode, unsigned int freq)
{
    //reset
    AT91C_BASE_DBGU->DBGU_IDR = ~(0u);
    AT91C_BASE_DBGU->DBGU_CR = AT91C_US_RSTRX | AT91C_US_RSTTX;

    //conigure PIO Assigns the I/O line to the Peripheral A function
    AT91C_BASE_PIOC->PIO_ASR = AT91C_PC30_DRXD | AT91C_PC31_DTXD;
    AT91C_BASE_PIOC->PIO_PDR = AT91C_PC30_DRXD | AT91C_PC31_DTXD;

    //configure
    AT91C_BASE_DBGU->DBGU_BRGR = CLK_FREQ / (16 * freq);
    AT91C_BASE_DBGU->DBGU_MR = parityType | mode;
    AT91C_BASE_DBGU->DBGU_CR = AT91C_US_RXEN;
    //AT91C_US_TXEN it starts immediately after set
    AT91C_BASE_DBGU->DBGU_CR = AT91C_US_TXEN;

    return 0;
}

int DBGU_SendData(unsigned char *data)
{
    int counter = 0;
    for (unsigned int i = 0; data[i] != '\0'; i++)
    {
        DBGU_SendAscii(data[i]);
        counter++;
    }
    return counter;
}

int DBGU_ReadData(unsigned char *data, unsigned int size)
{
    int counter = 0;
    do
    {
        DBGU_ReadAscii(&data[counter]);
        counter++;
    } while (data[counter - 1] != '\0');
    return counter;
}

void DBGU_ReadAscii(unsigned char *byte)
{
    while (AT91C_US_RXRDY != (AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_RXRDY))
        ;
    *byte = AT91C_BASE_DBGU->DBGU_RHR;
}

void DBGU_SendAscii(unsigned char byte)
{
    //

    //
    while (AT91C_US_TXRDY != (AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY))
        ;
    AT91C_BASE_DBGU->DBGU_THR = byte;
}

void FIFO_init(FIFO *fifo)
{
    fifo->head = 0;
    fifo->tail = 0;
}

int FIFO_push(FIFO *fifo, char data)
{
    if ((fifo->tail - fifo->head) == 1 || (fifo->head - fifo->tail) == BUFFERSIZE)
    {
        return -1;
    }
    fifo->buffer[fifo->head] = data;
    fifo->head = (fifo->head + 1) & BUFFERSIZE;
    return 1;
}

int FIFO_pop(FIFO *fifo, char *data)
{
    if (fifo->head != fifo->tail)
    {
        *data = fifo->buffer[fifo->tail];
        fifo->tail = (fifo->tail + 1) & BUFFERSIZE;
        return 1;
    }
    return -1;
}

void FIFO_empty(FIFO *fifo)
{
    fifo->head = fifo->tail;
}

/*int DBGU_FIFO_SendData(unsigned char *data, FIFO *fifo)
{
    for (unsigned int i = 0; data[i] != '\0'; i++)
    {
        if (FIFO_push(fifo, data[i]) == -1)
        {
            return -1;
        }
    }

    //in nest version in interupts
    int counter = 0;
    char c;
    while (FIFO_pop(fifo, &c) == 1)
    {
        DBGU_SendAscii(c);
        counter++;
    }
    return counter;
}*/

int FIFO_SendData(FIFO *fifo)
{
    int counter = 0;
    char c;
    while (FIFO_pop(fifo, &c) == 1)
    {
        DBGU_SendAscii(c);
        counter++;
    }
    return counter;
}