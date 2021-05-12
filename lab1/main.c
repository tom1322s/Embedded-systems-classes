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

#define PMC_PCER (volatile unsigned int *)(0xFFFFFC10)

#define PIOC_PER (volatile unsigned int *)(0xFFFFF600)
#define PIOC_ODR (volatile unsigned int *)(0xFFFFF614)
#define PIOC_PDSR (volatile unsigned int *)(0xFFFFF63C)
#define PIOC_PUER (volatile unsigned int *)(0xFFFFF664)

#define LED1 (1 << 8)
#define LED2 (1 << 29)
#define BUT1 (1 << 5)
#define BUT2 (1 << 4)

#define CLK_PORT_CDE (1 << 4)

#define CHECK_BIT(var, pos) ((var) & (pos))
//#define CHECK_BIT(var,pos) ((var>>(pos)) % 2)

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

    resetLed1();
    resetLed2();
}
void ConfigureButtons(void)
{
    *PMC_PCER = CLK_PORT_CDE;

    *PIOC_PER = BUT1;
    *PIOC_ODR = BUT1;
    *PIOC_PUER = BUT1;

    *PIOC_PER = BUT2;
    *PIOC_ODR = BUT2;
    *PIOC_PUER = BUT2;
}
void setLed1()
{
    *PIOB_CODR = LED1;
}
void resetLed1()
{
    *PIOB_SODR = LED1;
}
void setLed2()
{
    *PIOC_CODR = LED2;
}
void resetLed2()
{
    *PIOC_SODR = LED2;
}
int readBut1()
{
    return !CHECK_BIT(*PIOC_PDSR, BUT1);
}
int readBut2()
{
    return !CHECK_BIT(*PIOC_PDSR, BUT2);
}
int main(void)
{
    ConfigureLEDs();
    ConfigureButtons();

    while (1)
    {
        if (CHECK_BIT(*PIOB_ODSR, LED1))
            setLed1();
        else
            resetLed1();

        for (unsigned int i = 0; i < 1000; i++)
        {
            if (readBut1())
                setLed2();
            if (readBut2())
                resetLed2();
            delay_ms(1);
        }
    }
}
