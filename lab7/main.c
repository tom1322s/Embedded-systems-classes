#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "include/AT91SAM9263.h"
#include "DBGU.h"

#define LED1 (1 << 8)
#define LED2 (1 << 29)
#define BUT1 (1 << 5)
#define BUT2 (1 << 4)

#define CLK_PORT_CDE (1 << 4)

#define DELAY_MS(var) ((var / 1000.0) / (1.0 / (6.25e6))) //6.25e6 is freqency form prescaler, 1/(6.25e6) is time for one tick

#define CHECK_BIT(var, pos) ((var) & (pos))

#define DEFAULT_MODE 0
#define LED_MODE 1
#define BUTTON_MODE 2
#define DBGU_MODE 3

volatile bool pitGlobalFlag = false;
unsigned int pitInterrupts = 0;
unsigned int otherInterrupts = 0;
unsigned int pitOtherInterrupts = 0;
volatile unsigned int menuMode = DEFAULT_MODE;

void delay_ms(unsigned int delay);
void ConfigureLEDs(void);
void ConfigureButtons(void);
void PIT_Init(void);
void initPITInterrupts();
void turnOnLed1();
void turnOffLed1();
void turnOnLed2();
void turnOffLed2();
void changeLed1();
void changeLed2();
void blinkLed5(void (*funOn)(), void (*funOff)());
int readLed1();
int readLed2();
int readBut1();
int readBut2();
void enablePullUpBut1();
void enablePullUpBut2();
void disablePullUpBut1();
void disablePullUpBut2();
void PITIRQHandler();
void textBasedMenu(FIFO *queue);
void readCommand(FIFO *queue);
void processCommandDefault(FIFO *queue);
void processCommandLed(FIFO *queue);
void processCommandButton(FIFO *queue);
void processCommandDBGU(FIFO *queue);
void toLowerCase(unsigned char *str);
void splitCommand(unsigned char *command, unsigned char *param);
void printInfoAboutButton(int (*fun)(), unsigned char c);
void printInfoAboutLed(int (*fun)(), unsigned char c);
void printDBGUInfo();
void printInt(unsigned int val);

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
    enablePullUpBut1();

    AT91C_BASE_PIOC->PIO_PER = BUT2;
    AT91C_BASE_PIOC->PIO_ODR = BUT2;
    enablePullUpBut2();
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
void changeLed1()
{
    if (readLed1)
        turnOffLed1();
    else
        turnOnLed1();
}
void changeLed2()
{
    if (readLed2)
        turnOffLed2();
    else
        turnOnLed2();
}
int readLed1()
{
    return CHECK_BIT(AT91C_BASE_PIOB->PIO_ODSR, LED1);
}
int readLed2()
{
    return CHECK_BIT(AT91C_BASE_PIOC->PIO_ODSR, LED2);
}
void blinkLed5(void (*funOn)(), void (*funOff)())
{
    for (int times = 0; times < 5; times++)
    {
        funOn();
        delay_ms(1000);
        funOff();
        delay_ms(1000);
    }
}
void printInfoAboutLed(int (*fun)(), unsigned char c)
{
    if (fun())
    {
        DBGU_SendData("Led \0");
        DBGU_SendAscii(c);
        DBGU_SendData(" jest wlaczona \n\r\0");
    }
    else
    {
        DBGU_SendData("Led \0");
        DBGU_SendAscii(c);
        DBGU_SendData(" nie jest wlaczona \n\r\0");
    }
}
int readBut1()
{
    return !CHECK_BIT(AT91C_BASE_PIOC->PIO_PDSR, BUT1);
}
int readBut2()
{
    return !CHECK_BIT(AT91C_BASE_PIOC->PIO_PDSR, BUT2);
}

void printInfoAboutButton(int (*fun)(), unsigned char c)
{
    if (fun())
    {
        DBGU_SendData("Przycisk \0");
        DBGU_SendAscii(c);
        DBGU_SendData(" jest wcisniety \n\r\0");
    }
    else
    {
        DBGU_SendData("Przycisk \0");
        DBGU_SendAscii(c);
        DBGU_SendData(" nie jest wcisniety \n\r\0");
    }
}

void enablePullUpBut1()
{
    AT91C_BASE_PIOC->PIO_PPUER = BUT1;
}

void enablePullUpBut2()
{
    AT91C_BASE_PIOC->PIO_PPUER = BUT2;
}

void disablePullUpBut1()
{
    AT91C_BASE_PIOC->PIO_PPUDR = BUT1;
}

void disablePullUpBut2()
{
    AT91C_BASE_PIOC->PIO_PPUDR = BUT2;
}

void textBasedMenu(FIFO *queue)
{
    switch (menuMode)
    {
    case DEFAULT_MODE:
        DBGU_SendData("\n\r>");
        readCommand(queue);
        processCommandDefault(queue);
        break;
    case LED_MODE:
        DBGU_SendData("\n\rLED> \0");
        readCommand(queue);
        processCommandLed(queue);
        break;
    case BUTTON_MODE:
        DBGU_SendData("\n\rBUTTON> \0");
        readCommand(queue);
        processCommandButton(queue);
        break;
    case DBGU_MODE:
        DBGU_SendData("\n\rDBGU> \0");
        readCommand(queue);
        processCommandDBGU(queue);
        break;

    default:
        DBGU_SendData("Error\n\n\r\0");
        break;
    }
}

void readCommand(FIFO *queue)
{
    bool error = false;
    unsigned char c;
    do
    {
        DBGU_ReadAscii(&c);
        if (c != '\n')
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
}

void toLowerCase(unsigned char *str)
{
    while (*str)
    {
        *str = tolower(*str);
        str++;
    }
}

void processCommandDefault(FIFO *queue)
{
    unsigned char command[BUFFERSIZE];

    unsigned char c;
    int i;
    for (i = 0; FIFO_pop(queue, &c) == 1; i++)
    {
        command[i] = c;
    }
    command[i] = '\0';
    DBGU_SendData(command);
    DBGU_SendData("\n\r\0");

    toLowerCase(command);

    if (!strcmp(command, "help"))
    {
        DBGU_SendData("LED - led mode\n\r\0");
        DBGU_SendData("BUTTON - led mode\n\r\0");
        DBGU_SendData("DBGU - led mode\n\r\0");
    }
    else if (!strcmp(command, "led\0"))
    {
        menuMode = LED_MODE;
    }
    else if (!strcmp(command, "button\0"))
    {
        menuMode = BUTTON_MODE;
    }
    else if (!strcmp(command, "dbgu\0"))
    {
        menuMode = DBGU_MODE;
    }
    else
    {
        DBGU_SendData("method \"\0");
        DBGU_SendData(command);
        DBGU_SendData("\" not found\n\r\0");
    }
}

void processCommandLed(FIFO *queue)
{
    unsigned char command[BUFFERSIZE];

    unsigned char c;
    int i;
    for (i = 0; FIFO_pop(queue, &c) == 1; i++)
    {
        command[i] = c;
    }
    command[i] = '\0';
    DBGU_SendData(command);
    DBGU_SendData("\n\r\0");

    toLowerCase(command);
    unsigned char param;
    splitCommand(command, &param);

    if (!strcmp(command, "help"))
    {
        DBGU_SendData("SetLed parameter- trun on the led\n\r\0");
        DBGU_SendData("ClearLed parameter- turn of the led\n\r\0");
        DBGU_SendData("BlinkLed parameter- blink led 5 times\n\r\0");
        DBGU_SendData("LedStatus parameter- read status of the led\n\r\0");
        DBGU_SendData("ChangeLed parameter- change status of the led\n\r\0");
        DBGU_SendData("return - return to mode selection\n\r\0");
    }
    else if (!strcmp(command, "setled\0"))
    {
        if (param == 'a')
            turnOnLed1();
        else if (param == 'b')
            turnOnLed2();
        else
        {
            DBGU_SendData("parameter \"\0");
            DBGU_SendAscii(param);
            DBGU_SendData("\" not found\n\r\0");
        }
    }
    else if (!strcmp(command, "clearled\0"))
    {
        if (param == 'a')
            turnOffLed1();
        else if (param == 'b')
            turnOffLed2();
        else
        {
            DBGU_SendData("parameter \"\0");
            DBGU_SendAscii(param);
            DBGU_SendData("\" not found\n\r\0");
        }
    }
    else if (!strcmp(command, "blinkled\0"))
    {
        if (param == 'a')
            blinkLed5(turnOnLed1, turnOffLed1);
        else if (param == 'b')
            blinkLed5(turnOnLed2, turnOffLed2);
        else
        {
            DBGU_SendData("parameter \"\0");
            DBGU_SendAscii(param);
            DBGU_SendData("\" not found\n\r\0");
        }
    }
    else if (!strcmp(command, "ledstatus\0"))
    {
        if (param == 'a')
            printInfoAboutLed(readLed1, param);
        else if (param == 'b')
            printInfoAboutLed(readLed2, param);
        else
        {
            DBGU_SendData("parameter \"\0");
            DBGU_SendAscii(param);
            DBGU_SendData("\" not found\n\r\0");
        }
    }
    else if (!strcmp(command, "changeled\0"))
    {
        if (param == 'a')
            changeLed1();
        else if (param == 'b')
            changeLed2();
        else
        {
            DBGU_SendData("parameter \"\0");
            DBGU_SendAscii(param);
            DBGU_SendData("\" not found\n\r\0");
        }
    }
    else if (!strcmp(command, "return\0"))
    {
        menuMode = DEFAULT_MODE;
    }
    else
    {
        DBGU_SendData("method \"\0");
        DBGU_SendData(command);
        DBGU_SendData("\" not found\n\r\0");
    }
}

void processCommandButton(FIFO *queue)
{
    unsigned char command[BUFFERSIZE];

    unsigned char c;
    int i;
    for (i = 0; FIFO_pop(queue, &c) == 1; i++)
    {
        command[i] = c;
    }
    command[i] = '\0';
    DBGU_SendData(command);
    DBGU_SendData("\n\r\0");

    toLowerCase(command);
    unsigned char param;
    splitCommand(command, &param);

    if (!strcmp(command, "help"))
    {
        DBGU_SendData("readButton parameter- read status of the button\n\r\0");
        DBGU_SendData("pullUpEn parameter- enable pull-up resistor\n\r\0");
        DBGU_SendData("pullUpDis parameter- disable pull-up resistor\n\r\0");
        DBGU_SendData("return - return to mode selection\n\r\0");
    }
    else if (!strcmp(command, "readbutton\0"))
    {
        if (param == 'a')
            printInfoAboutButton(readBut1, param);
        else if (param == 'b')
            printInfoAboutButton(readBut2, param);
        else
        {
            DBGU_SendData("parameter \"\0");
            DBGU_SendAscii(param);
            DBGU_SendData("\" not found\n\r\0");
        }
    }
    else if (!strcmp(command, "pullupen\0"))
    {
        if (param == 'a')
            enablePullUpBut1();
        else if (param == 'b')
            enablePullUpBut2();
        else
        {
            DBGU_SendData("parameter \"\0");
            DBGU_SendAscii(param);
            DBGU_SendData("\" not found\n\r\0");
        }
    }
    else if (!strcmp(command, "pullupdis\0"))
    {
        if (param == 'a')
            disablePullUpBut1();
        else if (param == 'b')
            disablePullUpBut2();
        else
        {
            DBGU_SendData("parameter \"\0");
            DBGU_SendAscii(param);
            DBGU_SendData("\" not found\n\r\0");
        }
    }
    else if (!strcmp(command, "return\0"))
    {
        menuMode = DEFAULT_MODE;
    }
    else
    {
        DBGU_SendData("method \"\0");
        DBGU_SendData(command);
        DBGU_SendData("\" not found\n\r\0");
    }
}

void processCommandDBGU(FIFO *queue)
{
    unsigned char command[BUFFERSIZE];

    unsigned char c;
    int i;
    for (i = 0; FIFO_pop(queue, &c) == 1; i++)
    {
        command[i] = c;
    }
    command[i] = '\0';
    DBGU_SendData(command);
    DBGU_SendData("\n\r\0");

    toLowerCase(command);

    if (!strcmp(command, "help"))
    {
        DBGU_SendData("return - return to mode selection\n\r\0");
        DBGU_SendData("deviceStatus - print DGBU information\n\r\0");
    }
    else if (!strcmp(command, "return\0"))
    {
        menuMode = DEFAULT_MODE;
    }
    else if (!strcmp(command, "devicestatus\0"))
    {
        printDBGUInfo();
    }
    else
    {
        DBGU_SendData("method \"\0");
        DBGU_SendData(command);
        DBGU_SendData("\" not found\n\r\0");
    }
}

void splitCommand(unsigned char *command, unsigned char *param)
{
    bool flag = false;
    while (*command)
    {
        if (*command == ' ')
        {
            flag = true;
            break;
        }

        command++;
    }

    if (flag)
    {
        *command = '\0';
        command++;
        *param = *command;
        command++;
        if (*command != '\0')
            *param = '\0';
    }
    else
    {
        *param = '\0';
    }
}

void printDBGUInfo()
{
    DBGU_SendData("Procesor DBGU info \n\r\0");
    DBGU_SendData("Bit parity: \n\r\0");
    switch (AT91C_BASE_DBGU->DBGU_MR & (1 << 9 | 1 << 10 | 1 << 11))
    {
    case (0 << 9 | 0 << 10 | 0 << 11):
        DBGU_SendData("even parity \n\r\0");
        break;
    case (1 << 9 | 0 << 10 | 0 << 11):
        DBGU_SendData("odd parity \n\r\0");
        break;
    case (0 << 9 | 1 << 10 | 0 << 11):
        DBGU_SendData("patiry force to zero\n\r\0");
        break;
    case (1 << 9 | 1 << 10 | 0 << 11):
        DBGU_SendData("parity force to 1 \n\r\0");
        break;
    default:
        DBGU_SendData("no parity\n\r\0");
        break;
    }

    DBGU_SendData("\n8 data bits \n\r\0");

    unsigned int baudrate = CLK_FREQ / (16 * AT91C_BASE_DBGU->DBGU_BRGR);

    DBGU_SendData("Baudrate wynosi: \0");
    printInt(baudrate);
    DBGU_SendData("\n\r\0");
}

void printInt(unsigned int val)
{
    unsigned char str[10];
    int i = 0;
    do
    {
        str[i] = (val % 10) + '0';
        i++;
    } while (val /= 10);

    i--;
    while (i >= 0)
    {
        DBGU_SendAscii(str[i]);
        i--;
    }
}

int main(void)
{
    ConfigureLEDs();
    ConfigureButtons();
    PIT_Init();
    initPITInterrupts();
    DBGU_Init(AT91C_US_PAR_NONE, AT91C_US_CHMODE_NORMAL, 19200);

    FIFO fifo;
    FIFO *queue = &fifo;
    FIFO_init(queue);

    DBGU_SendData("Simple menu by Tomasz Sagan\n\n\r\0");
    DBGU_SendData("Write \"help\" to obtain more information\n\n\r\0");

    while (1)
    {
        textBasedMenu(queue);
    }
}
