/*********************************************************************
*		     SEGGER Microcontroller GmbH		     *
*			 The Embedded Experts			     *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File	: main.c
Purpose	: Generic application start

*/

#include "stm32f4xx.h"
/*********************************************************************
*
*	main()
*
*  Function description
*   Application	entry point.
*/

int main(void)
{
    //enable clocks to PORT D >	LEDS D12-D15
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    //and PORT A > USER	button A0
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    //set gpio D 12,13,14,15 > op
    GPIOD->MODER |= GPIO_MODER_MODE12_0	| GPIO_MODER_MODE13_0 |	GPIO_MODER_MODE14_0 | GPIO_MODER_MODE15_0;
    //set gpio A 0 > i/p
    //GPIOA->MODER|=GPIO_MODER_MODE0; //sets as	'0b11' but not required	as reset state
    do {
	if(GPIOA->IDR &	GPIO_IDR_ID0) {	   // if	PA0	is high
	    GPIOD->BSRR	|= GPIO_BSRR_BS14;  //on
	    //GPIOD->BSRR	|= GPIO_BSRR_BR14;  //off
	}
	else {
	    //GPIOD->BSRR	|= GPIO_BSRR_BS14;  //on
	    GPIOD->BSRR	|= GPIO_BSRR_BR14;  //off
	}
    } while(1);
}

/*************************** End of file ****************************/