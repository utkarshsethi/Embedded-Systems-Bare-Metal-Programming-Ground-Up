#include "stm32f4xx.h"
#include "systick.h"

#define	SYSCLK	16000000
#define	SYSTICK_LOAD_VAL (SYSCLK * 0.001) //  (1/1000) = 0.001	= 1ms
#define	CTRL_ENABLE (1 << 0)
#define	CTRL_CLKSRC (1 << 2)
#define	CTRL_COUNTFLAG (1 << 16)


/*

  systick_delay_ms(int delay)

  Generate delay(in ms)	using systick timer

  @param delay:	time in	ms

*/
void systick_delay_ms(int delay)
{

    /*
  N > number of	cycles
  Clk >	system clock
  delay	= N/Clk
  1ms	= N/SYSCLK
  N = 16 MHz * 1ms
  N = SYSCLK * (1 * millisecond) = 16000
		  (16000000 * 1) / 1000	= 16000
  */

    // reload with number	of clocks/ms
    SysTick->LOAD = SYSTICK_LOAD_VAL - 1; // -1	as cout	starts from 0->LOAD

    //clear systick current value
    SysTick->VAL = 0;

    SysTick->CTRL |= CTRL_CLKSRC	      //select internal	clk source
		    | CTRL_ENABLE;    //enable systick

    for(int i =	0; i < delay; i++) {
	//wait until COUNTFLAG is	set
	while((SysTick->CTRL & CTRL_COUNTFLAG) == 0) {
	};
    }
    //RESET	systick	timer
    SysTick->CTRL = 0;
}