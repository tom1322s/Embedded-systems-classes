#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include "include/AT91SAM9263.h"
#include "DBGU.h"

#define DELAY_MS(var) ((var / 1000.0) / (1.0 / (6.25e6))) //6.25e6 is freqency form prescaler, 1/(6.25e6) is time for one tick

#define CHECK_BIT(var, pos) ((var) & (pos))

void delay_ms(unsigned int delay);
void PIT_Init(void);

void delay_ms(unsigned int delay)
{
    AT91C_BASE_PITC->PITC_PIVR;
    for (unsigned int i = 0; i < delay; i++)
    // one taking a loop last 1 ms
    {
        AT91C_BASE_PITC->PITC_PIMR |= AT91C_PITC_PITEN;
        while (!AT91C_BASE_PITC->PITC_PISR)
            ;
        AT91C_BASE_PITC->PITC_PIVR;
    }
}

void PIT_Init(void)
{
    //AT91C_BASE_PITC->PITC_PIMR &= (~AT91C_PITC_PITEN);
    //AT91C_BASE_PITC->PITC_PIMR &= (~AT91C_PITC_PITIEN);

    AT91C_BASE_PITC->PITC_PIMR = DELAY_MS(1); //this line contains indirectly also two lines above
    //AT91C_BASE_PITC->PITC_PIMR = 6250;
}

int main(void)
{
    PIT_Init();
    //DGBU_Init(NO_PARITY, NORMAL);
    DBGU_Init(AT91C_US_PAR_NONE, AT91C_US_CHMODE_NORMAL, 115200);

    DBGU_SendData("Fifo labolatory\n\r\0");

    FIFO fifo;
    FIFO *queue = &fifo;
    FIFO_init(queue);

    while (1)
    {
        DBGU_SendData("Press keys then enter\n\r\0");

        bool error = false;
        unsigned char c;
        do
        {
            DBGU_ReadAscii(&c);
            if (FIFO_push(queue, c) == -1)
                error = true;
            //DBGU_SendAscii(c);

        } while (!error && c != '\n');

        if (error)
        {
            DBGU_SendData("\n\rFifo buffer is full error\n\r\0");
            FIFO_SendData(queue);
            FIFO_empty(queue);
        }
        else
        {
            FIFO_SendData(queue);
        }
        DBGU_SendData("\n\r\n\r\0");
    }
}
