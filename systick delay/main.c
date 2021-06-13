/*********************************************************************
*		     SEGGER Microcontroller GmbH		     *
*			 The Embedded Experts			     *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File	: main.c
Purpose	: Systick delay

*/

#include "stm32f4xx.h"
#include "systick.h"

/*********************************************************************
*
*	main()
*
*  Function description
*   Application	entry point.
*/

int main(void)
{

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;    //enable port D

    //set pin 15 as output
    GPIOD->MODER |= (0b01 << 2 * 15);	 //0b01	= o/p
    do {

	GPIOD->ODR ^= (1 << 15);    // toggle diodes
	systick_delay_ms(1000);
    } while(1);
    return 0;
}

/*************************** End of file ****************************/