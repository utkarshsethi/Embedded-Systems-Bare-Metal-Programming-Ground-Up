/*********************************************************************
*		     SEGGER Microcontroller GmbH		     *
*			 The Embedded Experts			     *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File	: main.c
Purpose	: Generic application start

*/

#include "ADC.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>

uint32_t adc1_pa1_val;

/*********************************************************************
*
*	main()
*
*  Function description
*   Application	entry point.
*/
int main(void)
{

    adc_pa1_init();
    start_conversion();

    do {

	adc1_pa1_val = adc_read();    //read adc value

	printf("ADC PA1	value :	%d\r\n", adc1_pa1_val);

    } while(1);
}

/*************************** End of file ****************************/