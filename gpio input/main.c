/*********************************************************************
*		     SEGGER Microcontroller GmbH		     *
*			 The Embedded Experts			     *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File	: main.c
Purpose	: Generic application start

*/

#include "stm32f4xx.h"

/*****************************************************************************

init()

Function: setup	GPIO pins

*/

int init(void)
{

    // enable clocks	to	PORT D >	LEDS D12-D15
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    // and PORT	A > USER	button A0
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // set gpio	D 12,13,14,15 >	op
    GPIOD->MODER |= (0b01 << GPIO_MODER_MODE12_Pos) | (0b01 << GPIO_MODER_MODE13_Pos) |	(0b01 << GPIO_MODER_MODE14_Pos)	| (0b01	<< GPIO_MODER_MODE15_Pos);
    // set gpio	A 0 > i/p   PA0
    //	_0 = 01;	_ = 11;	_1 = 10
    GPIOA->MODER |= (0b00 << GPIO_MODER_MODE0_Pos);    // sets as	'0b00' but not required	as reset state
}

/*********************************************************************
 *
 *	main()
 *
 *  Function description
 *   Application	entry point.
 */

int main(void)
{
    init();

    while(1) {
	if(GPIOA->IDR &	GPIO_IDR_ID0)	 //	if	PA0	is high
	{
	    GPIOD->BSRR	|= GPIO_BSRR_BS12;    // on  Set_12
	    GPIOD->BSRR	|= GPIO_BSRR_BS13;    // on  Set_13
	    GPIOD->BSRR	|= GPIO_BSRR_BS14;    // on  Set_14
	    GPIOD->BSRR	|= GPIO_BSRR_BS15;    // on  Set_15
	}
	else {
	    GPIOD->BSRR	|= GPIO_BSRR_BR12;    // off   Reset_14
	    GPIOD->BSRR	|= GPIO_BSRR_BR13;    // off   Reset_14
	    GPIOD->BSRR	|= GPIO_BSRR_BR14;    // off   Reset_14
	    GPIOD->BSRR	|= GPIO_BSRR_BR15;    // off   Reset_14
	}
    }
}

/*************************** End of file ****************************/