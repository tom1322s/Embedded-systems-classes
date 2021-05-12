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
void printAlphabet(void);
void printUperLower(void);

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

void printAlphabet(void)
{
    for (unsigned char c = 'a'; c != 'z' + 1; c++)
    {
        DBGU_SendAscii(c);
    }
    for (unsigned char c = 'A'; c != 'Z' + 1; c++)
    {
        DBGU_SendAscii(c);
    }
}

void printUperLower(void)
{
    char c;
    DBGU_ReadAscii(&c);
    if (isalpha(c))
    {
        if (isupper(c))
            c = tolower(c);
        else
            c = toupper(c);
    }
    DBGU_SendAscii(c);
}

int main(void)
{
    PIT_Init();
    //DGBU_Init(NO_PARITY, NORMAL);
    DBGU_Init(AT91C_US_PAR_NONE, AT91C_US_CHMODE_NORMAL, 115200);

    printAlphabet();
    DBGU_SendAscii('\n');
    DBGU_SendData("My string to print\0");
    DBGU_SendAscii('\n');
    printUperLower();
    DBGU_SendAscii('\n');

    FIFO *queue;
    FIFO_init(queue);

    while (1)
    {
        DBGU_FIFO_SendData("My Fifo test\n", queue);
        delay_ms(500);
    }
}
