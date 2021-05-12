#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#define PIOB_PER (volatile unsigned int *)(0xFFFFF400)
#define PIOB_OER (volatile unsigned int *)(0xFFFFF410)
#define PIOB_SODR (volatile unsigned int *)(0xFFFFF430)
#define PIOB_CODR (volatile unsigned int *)(0xFFFFF434)
#define PIOB_ODSR (volatile unsigned int *)(0xFFFFF438)

#define PIOC_PER (volatile unsigned int *)(0xFFFFF600)
#define PIOC_OER (volatile unsigned int *)(0xFFFFF610)
#define PIOC_SODR (volatile unsigned int *)(0xFFFFF630)
#define PIOC_CODR (volatile unsigned int *)(0xFFFFF634)
#define PIOC_ODSR (volatile unsigned int *)(0xFFFFF638)

#define LED1 (1 << 8)
#define LED2 (1 << 29)

#define CHECK_BIT(var, pos) ((var) & (pos))

void delay_ms(unsigned int delay);
void ConfigureLEDs(void);
void turnOnLed1();
void turnOffLed1();
void turnOnLed2();
void turnOffLed2();

void delay_ms(unsigned int delay)
{
    volatile unsigned int i;
    delay *= 100000;
    for (i = 0; i < delay; i++)
    {
    }
}

void ConfigureLEDs(void)
{
    *PIOB_PER = LED1;
    *PIOB_OER = LED1;

    *PIOC_PER = LED2;
    *PIOC_OER = LED2;

    turnOffLed1();
    turnOffLed2();
}

void turnOnLed1()
{
    *PIOB_CODR = LED1;
}
void turnOffLed1()
{
    *PIOB_SODR = LED1;
}
void turnOnLed2()
{
    *PIOC_CODR = LED2;
}
void turnOffLed2()
{
    *PIOC_SODR = LED2;
}

int main(void)
{
    ConfigureLEDs();

    while (1)
    {
        if (CHECK_BIT(*PIOB_ODSR, LED1))
            turnOnLed1();
        else
            turnOffLed1();

        delay_ms(1000);
    }
}
