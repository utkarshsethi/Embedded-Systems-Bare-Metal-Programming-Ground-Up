/*********************************************************************
*		     SEGGER Microcontroller GmbH		     *
*			 The Embedded Experts			     *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File	: main.c
Purpose	: Generic application start

*/

#include "stm32f4xx.h";
/*********************************************************************
*
*	main()
*
*  Function description
*   Application	entry point.
*/

int main(void)
{
    //enable clocks to PORT D >	LEDS
    //and PORT A > USER	button
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    //set gpio D 12,13,14,15 > op
    GPIOD->MODER |= GPIO_MODER_MODE12|GPIO_MODER_MODE13|GPIO_MODER_MODE14|GPIO_MODER_MODE15;
    //set gpio A 0 > i/p
    //GPIOA->MODER|=GPIO_MODER_MODE0; sets as 11 but not required as reset state
    do {

    } while(1);
}

/*************************** End of file ****************************/