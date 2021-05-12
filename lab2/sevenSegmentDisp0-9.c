#include <stdint.h>
#include <stdbool.h>

#define PIOB_PER (volatile unsigned int *)(0xFFFFF400)
#define PIOB_OER (volatile unsigned int *)(0xFFFFF410)
#define PIOB_SODR (volatile unsigned int *)(0xFFFFF430)
#define PIOB_CODR (volatile unsigned int *)(0xFFFFF434)
#define PIOB_ODSR (volatile unsigned int *)(0xFFFFF438)

#define DISPLAY_ENABLE (1 << 29)
#define DISPLAY_ENABLE_RIGHT (1 << 30)
#define DISPLAY_ENABLE_LEFT (1 << 28)

#define DISPLAY_PINS (1 << 20 | 1 << 21 | 1 << 22 | 1 << 23 | 1 << 24 | 1 << 25 | 1 << 26 | 1 << 27)

#define LEFT 1
#define RIGHT 0

#define CHECK_BIT(var, pos) ((var) & (pos))

const unsigned int NUM_TABLE[10] = {0xB700000, 0x1400000, 0x7300000, 0x7600000, 0xD400000, 0xE600000, 0xE700000, 0x3400000, 0xF700000, 0xF600000};

void LED_init(void);
uint32_t LED_decode(uint8_t digit);
void LED_display_digit(uint8_t digit, uint8_t pos);
void LED_diplay_number(uint8_t number);
void delay_ms(unsigned int delay);

void delay_ms(unsigned int delay)
{
    volatile unsigned int i;
    delay *= 100000;
    for (i = 0; i < delay; i++)
    {
    }
}

void LED_init(void)
{
    *PIOB_PER = DISPLAY_ENABLE | DISPLAY_ENABLE_RIGHT | DISPLAY_ENABLE_LEFT | DISPLAY_PINS;
    *PIOB_OER = DISPLAY_ENABLE | DISPLAY_ENABLE_RIGHT | DISPLAY_ENABLE_LEFT | DISPLAY_PINS;

    *PIOB_SODR = DISPLAY_ENABLE;

    *PIOB_SODR = DISPLAY_ENABLE_LEFT | DISPLAY_ENABLE_RIGHT;
    *PIOB_CODR = DISPLAY_PINS;
}

uint32_t LED_decode(uint8_t digit)
{
    return NUM_TABLE[digit];
}

void LED_display_digit(uint8_t digit, uint8_t pos)
{
    *PIOB_SODR = DISPLAY_ENABLE_RIGHT | DISPLAY_ENABLE_LEFT;
    *PIOB_CODR = DISPLAY_PINS;
    *PIOB_SODR = LED_decode(digit);

    if (pos == RIGHT)
    {
        *PIOB_CODR = DISPLAY_ENABLE_RIGHT;
        delay_ms(10);
    }
    else
    {
        *PIOB_CODR = DISPLAY_ENABLE_LEFT;
        delay_ms(10);
    }
}

void LED_diplay_number(uint8_t number)
{
    uint8_t num1 = number % 10;
    uint8_t num2 = number / 10;

    LED_display_digit(num1, RIGHT);
    if (num2)
    {
        LED_display_digit(num2, LEFT);
    }
    else
    {
        delay_ms(10);
    }
}

int main(void)
{
    LED_init();

    uint8_t number = 0;

    while (1)
    {
        for (unsigned int i = 0; i < 50; i++)
        {
            LED_display_digit(number, LEFT);
        }
        number++;
        if (number > 9)
            number = 0;
    }
}
