#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include "include/AT91SAM9263.h"
//#include "DBGU.h"

#define LED1 (1 << 8)
#define LED2 (1 << 29)
#define BUT1 (1 << 5)
#define BUT2 (1 << 4)

#define CLK_PORT_CDE (1 << 4)

#define DELAY_MS(var) ((var / 1000.0) / (1.0 / (6.25e6))) //6.25e6 is freqency form prescaler, 1/(6.25e6) is time for one tick

#define CHECK_BIT(var, pos) ((var) & (pos))

volatile bool pitGlobalFlag = false;
unsigned int pitInterrupts = 0;
unsigned int otherInterrupts = 0;
unsigned int pitOtherInterrupts = 0;

void delay_ms(unsigned int delay);
void ConfigureLEDs(void);
void ConfigureButtons(void);
void PIT_Init(void);
void initPITInterrupts();
void turnOnLed1();
void turnOffLed1();
void turnOnLed2();
void turnOffLed2();
int readBut1();
int readBut2();
void PITIRQHandler();

void delay_ms(unsigned int delay)
{
    for (unsigned int i = 0; i < delay; i++)
    // one taking a loop last 1 ms
    {
        AT91C_BASE_PITC->PITC_PIVR;
        AT91C_BASE_PITC->PITC_PIMR |= AT91C_PITC_PITEN;
        while (!pitGlobalFlag)
            ;
        pitGlobalFlag = false;
        AT91C_BASE_PITC->PITC_PIMR &= ~(AT91C_PITC_PITEN);
    }
}

void PIT_Init(void)
{
    //AT91C_BASE_PITC->PITC_PIMR &= (~AT91C_PITC_PITEN);
    //AT91C_BASE_PITC->PITC_PIMR &= (~AT91C_PITC_PITIEN);

    AT91C_BASE_PITC->PITC_PIMR = DELAY_MS(1); //this line contains indirectly also two lines above
    //AT91C_BASE_PITC->PITC_PIMR = 6250;
    AT91C_BASE_PITC->PITC_PIMR |= AT91C_PITC_PITIEN;
}

void initPITInterrupts()
{
    AT91C_BASE_AIC->AIC_IDCR = (1 << AT91C_ID_SYS);
    AT91C_BASE_AIC->AIC_SVR[AT91C_ID_SYS] = (unsigned int)PITIRQHandler;
    AT91C_BASE_AIC->AIC_SMR[AT91C_ID_SYS] = AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE | AT91C_AIC_PRIOR_LOWEST;
    AT91C_BASE_AIC->AIC_ICCR = (1 << AT91C_ID_SYS);
    AT91C_BASE_AIC->AIC_IECR = (1 << AT91C_ID_SYS);
}

void PITIRQHandler()
{
    if (AT91C_BASE_PITC->PITC_PIMR & AT91C_PITC_PITIEN)
    {
        if (AT91C_BASE_PITC->PITC_PISR)
        {
            AT91C_BASE_PITC->PITC_PIVR;
            pitGlobalFlag = true;
            pitInterrupts++;
        }
        else
        {
            pitOtherInterrupts++;
        }
    }
    else
    {
        otherInterrupts++;
    }
}

void ConfigureLEDs(void)
{
    AT91C_BASE_PIOB->PIO_PER = LED1;
    AT91C_BASE_PIOB->PIO_OER = LED1;

    AT91C_BASE_PIOC->PIO_PER = LED2;
    AT91C_BASE_PIOC->PIO_OER = LED2;

    turnOffLed1();
    turnOffLed2();
}

void ConfigureButtons(void)
{
    AT91C_BASE_PMC->PMC_PCER = CLK_PORT_CDE;

    AT91C_BASE_PIOC->PIO_PER = BUT1;
    AT91C_BASE_PIOC->PIO_ODR = BUT1;
    AT91C_BASE_PIOC->PIO_PPUER = BUT1;

    AT91C_BASE_PIOC->PIO_PER = BUT2;
    AT91C_BASE_PIOC->PIO_ODR = BUT2;
    AT91C_BASE_PIOC->PIO_PPUER = BUT2;
}

void turnOnLed1()
{
    AT91C_BASE_PIOB->PIO_CODR = LED1;
}
void turnOffLed1()
{
    AT91C_BASE_PIOB->PIO_SODR = LED1;
}
void turnOnLed2()
{
    AT91C_BASE_PIOC->PIO_CODR = LED2;
}
void turnOffLed2()
{
    AT91C_BASE_PIOC->PIO_SODR = LED2;
}
int readBut1()
{
    return !CHECK_BIT(AT91C_BASE_PIOC->PIO_PDSR, BUT1);
}
int readBut2()
{
    return !CHECK_BIT(AT91C_BASE_PIOC->PIO_PDSR, BUT2);
}

int main(void)
{
    ConfigureLEDs();
    ConfigureButtons();
    PIT_Init();
    initPITInterrupts();

    while (1)
    {
        if (CHECK_BIT(AT91C_BASE_PIOB->PIO_ODSR, LED1))
            turnOnLed1();
        else
            turnOffLed1();

        for (unsigned int i = 0; i < 1000; i++)
        {
            if (readBut1())
                turnOnLed2();
            if (readBut2())
                turnOffLed2();
            delay_ms(1);
        }
    }
}
