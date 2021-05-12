// LEDs:    PD12(Green), PD13(Orange), PD14(RED),PD15(BLUE)

#include "stm32f4xx.h"
    #define LED1 (12)   //Green
    #define LED2 (13)   //Orange
    #define LED3 (14)   //Red
    #define LED4 (15)   //Blue
    const int LED[] = {LED1,LED2,LED3,LED4}; //Arrary of Leds
    #define _MODER    MODER
    #define GPIOMODER ( GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0 )

void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=500;
      while (x-- > 0)
         __asm("nop");
   }
}

//Alternates LEDs quickly
int main(void)
{
        RCC->AHB1ENR |= 0
                     |  RCC_AHB1ENR_GPIODEN     //We OR with RCC_AHB1ENR_GPIODEN
        ;        
    /*
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN
            RCC->AHB1ENR |= 0   
                         |  RCC_AHB1ENR_GPIODEN  
    RCC_AHB1ENR_GPIODEN is predefined constant in stm32f4xx.h to enable GPIO Port D > GPIO-D-EN
    To enable both Port A&B
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOAEN 
    */
    GPIOD->_MODER |= GPIOMODER;   // set pins to be general purpose output
    int i = 0;
    do {
/*
GPIOx_ODR 	-	O/p [ 0-15]
GPIOD->ODR ^= (1<<PORTNO);
Output Data Register > specifies which Pin of the Port
In this case Pin 12 for Green of Port D
    ODR is one of GPIO registers which holds all output states of pins of a GPIO port. We can alter with 16bit binary value and we specify the pin values with this register.

<< left shit operator
basically adds specified no of 12 to the end
1<<12 becomes
0001 0000 0000 0000
and hence Pin 12 of Port D = Green LED

^= is XOR

A   B   O
0   0   0
0   1   1
1   0   1
1   1   0

GPIOD->ODR ^= (1<<LED[i]); will use only
0   1   1   if 0 will become 1
1   1   0   if 1 will become 0
*/
    GPIOD->ODR ^= (1<<LED[i]);  // toggle diodes
    ms_delay(200);
    GPIOD->ODR ^= (1<<LED[i]);  // toggle diodes
    ms_delay(500);
    i++;
    if(i == 4)
        i = 0;
    } while (1);
    return 0;
}
