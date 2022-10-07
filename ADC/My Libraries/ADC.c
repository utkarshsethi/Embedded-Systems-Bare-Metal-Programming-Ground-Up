/*
APB2 ADCx; x = [1,2,3]

RCC_APB2ENR > Bit 8 ADC1EN: ADC1 clock enable

ADC123_IN1  PA1

channel	1 as regular sequence 1

*/

#include "ADC.h"
#include "stm32f4xx.h"

void adc_pa1_init(void)
{

    // Enable clock to ADC pin PORT PA1
    RCC->AHB1ENR |= (0b1 << 0);	   // Bit 0	GPIOAEN: IO	port A clock enable

    // set mode	of PA1 to analog
    GPIOA->MODER |= (0b11 << 2 * 1);	// 11: Analog mode

    // configure	ADC	peripheral
    // enable clock to ADC
    RCC->APB2ENR |= (0b1 << 8);

    // config ADC parameters
    // conversion seq start
    ADC1->SQR3 |= (0b1 << 5 * 0);    //	channel	1 as sequence	1;
    // conversion seq length
    ADC1->SQR1 |= (0b0101 << 20);	  // SQR1: Bits	23:20 L[3:0]: Regular channel sequence length
    // enable ADC
    ADC1->CR2 =	(0b1 <<	0);    // Bit 0	ADON: A/D	Converter ON / OFF ; 0b1:	Enable ADC
}

void start_conversion(void)
{
    // start ADC	conversion
    ADC1->CR2 |= (0b1 << 30);	 // Bit	30 SWSTART: Start conversion	of regular channels

    // Continous Mode
    //0: Single	conversion mode
    //1: Continuous conversion mode
    ADC1->CR2 |= (0b1 << 1);  // Bit 1 CONT: Continuous	conversion
}

uint32_t adc_read(void)
{
    // wait fo rconsersion to complete
    // ADC_SR: Bit	1 EOC: Regular channel end of conversion
    while((ADC1->SR & (0b1 << 1)) == 0)	{
	// wait	while !=1	i.e. conversion	not complete
	//0: Conversion	not complete (EOCS=0), or sequence of conversions not complete (EOCS=1)
	//1: Conversion	complete (EOCS=0), or sequence of conversions complete (EOCS=1)
    };

    // read converted result
    // ADC_DR: Bits 15:0	DATA[15:0]: Regular data;	default: right aligned i.e. 12 bit from	right in regular mode
    // uint32_t	value =	(ADC1->DR & 0x0FFF);	//read 12 bits of data register
    uint32_t value = (ADC1->DR);
    return value;
}