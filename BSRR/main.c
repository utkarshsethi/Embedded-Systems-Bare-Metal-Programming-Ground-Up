/*********************************************************************
*		     SEGGER Microcontroller GmbH		     *
*			 The Embedded Experts			     *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File	: main.c
Purpose	: BSSR LEARN

*/

#include "stm32f4xx.h"

/*********************************************************************
*
*	main()
*
*  Function description
*   Application	entry point.
*/

void delay(int ms)
{

    while(ms-- > 0) {
	volatile int i = 3195;
	while(i-- > 0)
	    __asm("nop")
	    ;
    }
}

//void ms_delay(int ms)
//{
//    while(ms-- > 0) {
//	  volatile int x = 500;
//	  while(x-- > 0)
//	      __asm("nop");
//    }
//}

int main(void)
{
    //enable	GPIOD
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    //set GPIOD	as output (a	0b01 binary set	mask)
    GPIOD->MODER |= GPIO_MODER_MODE12_0	| GPIO_MODER_MODE13_0 |	GPIO_MODER_MODE14_0 | GPIO_MODER_MODER15_0;

    while(1) {

	GPIOD->BSRR |= GPIO_BSRR_BS13;
	delay(50);

	GPIOD->BSRR |= GPIO_BSRR_BR13;
	delay(1000);
    }
}

/*************************** End of file ****************************/