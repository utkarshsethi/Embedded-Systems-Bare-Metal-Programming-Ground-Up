#include "stm32f4xx.h"
    #define LEDPORT (GPIOD)
    #define LED1 (12)
    #define ENABLE_GPIO_CLOCK ( \
        RCC->AHB1ENR |= 0   \
            |  RCC_AHB1ENR_GPIODEN  \
    ) /*
    Using CNAME "ENABLE_GPIO_CLOCK" USEES EXPRESSION (RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN)
            RCC->AHB1ENR |= 0   //We OR with '0' to reset the regsiter
            |  RCC_AHB1ENR_GPIODEN  //We OR with RCC_AHB1ENR_GPIODEN
    RCC_AHB1ENR_GPIODEN is predefined constant in stm32f4xx.h to enable GPIO Port D > GPIO-D-EN
    To enable both Port A&B
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOAEN 
    */
    #define _MODER    MODER
    #define GPIOMODER (GPIO_MODER_MODER12_0)

void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=500;
      while (x-- > 0)
         __asm("nop");
   }
}

//Alternates blue and green LEDs quickly
int main(void)
{
    ENABLE_GPIO_CLOCK;              // enable the clock to GPIO
    LEDPORT->_MODER |= GPIOMODER;   // set pins to be general purpose output
    for (;;) {
    ms_delay(250);
    LEDPORT->ODR ^= (1<<LED1);  // toggle diodes
    }

    return 0;
}
