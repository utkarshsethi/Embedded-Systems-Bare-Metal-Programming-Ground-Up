
// LEDs
//	Green		- PD12
//	Orange 	- PD13
//	RED			-	PD14
//	BLUE		- PD15
//	PORT D - GPIODEN
//	AHB1
//	RCC -> AHB1ENR
//	GPIOx_MODER -	Mode
//	GPIOx_ODR 	-	O/p [ 0-15]

#include "stm32f4xx.h"                  // Device header
void delay(int delay);



int main(void) {
	
	RCC->AHB1ENR |= 8;										//enable GPIOD clock
	GPIOD->MODER |= 0x40000000;					//2b 0100 0000 0000 0000 0000 0000 0000 0000 
	
	
	while(1) {
		
		GPIOD->ODR |= 0x8000;							//2b	1000 0000	0000	0000 PD15
		delay(50);
		GPIOD->ODR &=~ 0x8000;						// & operation and not operation to invert work of last command
		delay(800);
	}
}


void delay(int delay)	{
	int i;
	for(; delay>0;delay--)	{
		for(i = 0; i<5000;i++)	{
			//nothing
		}
	}
}
