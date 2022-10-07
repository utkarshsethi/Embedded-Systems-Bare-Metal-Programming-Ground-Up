/*********************************************************************
*		     SEGGER Microcontroller GmbH		     *
*			 The Embedded Experts			     *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File	: main.c
Purpose	: Generic application start

*/

/******************************************************************************/
/*
APB1 : USART3 [RCC -> APB1 | Bit18 (USART3_EN) ]

USART3 >
    > TX  PD-8
    > RX  PD-9
	Use port D instead of B	as we already enable clock to GPIO D for LEDs

USART_SR

USART_CR1
    > RE - receiver enable > bit 2
    > TE - transmitter enable >	bit 3
    > M	> Bit 12 (word length)
    > UE > Bit 13 (uart	enable)

AF register
GPIOx_AFRH (8..15) > AF7 (USART1..3)

USART_BRR (Baud	Rate Register)
    >

*/
/******************************************************************************/

#include "stm32f4xx.h"
#include <stdarg.h>
#include <stdio.h>

#define	toDigit(c) (c -	'0')	//convert	number received	in char	to int							      \
				//  C/C++	standard work on char encodings	where '0' - '9'	are always together		      \
				//  '9'	- '0' will remove	starting  ascii	of '0' from '9'	resulting in the actual	number i.e. 9 \
				//https://stackoverflow.com/questions/439573/how-to-convert-a-single-char-into-an-int

/******************************************************************************/
/*
	  sudo_delay(int delay)
	  introduces a sudo delay

	  delay: decide	duration of delay
*/
/******************************************************************************/

void sudo_delay(int delay)
{
    while(delay-- > 0) {
	volatile int i = 3195;
	while(i-- > 0) {
	    __asm("nop");
	}
    }
}

/******************************************************************************/
/*
    BAUD = Fck/(16*USARTDIV)
    i.e. USARTDIV = PCLK1/(Baud*16)
      @Baud=115200
      @PCLK1= 16 MHZ = 16000000	Hz
	USARTDIV = 8.680
	  i.e.	BRR_mantissa  =	8 = 0x8
		BRR_frac      =	0.680*16 = 10.88  = 11 =0xB
	Hence BRR = 0x8B
*/
/******************************************************************************/
// BRR =	(Pclk +	(BaudRate / 2))	/ BaudRate

static uint16_t	calc_BRR(uint32_t Pclk,	uint32_t BaudRate)
{
    uint16_t brr = (Pclk + (BaudRate / 2)) / BaudRate;
    return brr;
}

/******************************************************************************/
/*
      init()
      Initialise USART3	& GPIO
      PD8 : TX
      PD9 : RX
      @115200
      for TX

      PA0 User Button
      PD12-15 LEDs
*/
/******************************************************************************/

void init()
{
    // enable gpio	clock
    RCC->AHB1ENR |= (1 << 3);	 // enable port	D	(1<<3) to use USART3 (PB8,9) and PD12..15 for LEDs

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;    // enable PORT	A for USER button A0

    // enable usart	clock
    RCC->APB1ENR |= (1 << 18);	  // RCC_APB1ENR_USART3EN = (1<<18)

    // set gpio	out
    GPIOD->MODER |= GPIO_MODER_MODER12_0       //
		    | GPIO_MODER_MODER13_0     //
		    | GPIO_MODER_MODER14_0     //
		    | GPIO_MODER_MODER15_0;    // set	PD12..15 as o/p	> 0b01

    // set PD8,9	in alternate func mode
    GPIOD->MODER |= (0b10 << GPIO_MODER_MODER8_Pos)	  // 0b10 >	AF Mode
		    | (0b10 << GPIO_MODER_MODER9_Pos);	  // set	PD8,9 to AF

    //	0111(9)	0111(8)	>  0x77	put PD8,9 as AF7(0b0111)
    GPIOD->AFR[1] |= (0b111 << 4 * 0) |	(0b111 << 4 * 1);    //	blocks of 4bits.	So firs	t4 bits	= 4*0 and then 4*1 etc....

    /*
	BAUD = Fck/(16*USARTDIV)
	i.e. USARTDIV =	PCLK1/(Baud*16)
	  @Baud=115200
	  @PCLK1= 16 MHZ = 16000000	Hz
	    USARTDIV = 8.680
	      i.e.	BRR_mantissa  =	8 = 0x8
		    BRR_frac	  =	0.680*16 = 10.88  = 11 =0xB
	    Hence BRR =	0x8B
    */
    uint32_t Pclk = 16000000; //PeripheralClock
    uint32_t BaudRate =	115200;

    USART3->BRR	|= calc_BRR(Pclk, BaudRate);	// set baurdate	= 115200

    USART3->CR1	|= (1 << 3);	// Bit 3	TE: Transmitter	enable
    USART3->CR1	|= (1 << 2);	// Bit 2	RE: Receiver enable

    /* UE can be enables in same line, but is recommended to enable USART AFTER	enabling all other config */
    USART3->CR1	|= (1 << 13);	 // Bit	13 UE: USART	Enable

    // init complete
    GPIOD->BSRR	|= GPIO_BSRR_BS14;
    sudo_delay(500);
    GPIOD->BSRR	|= GPIO_BSRR_BR14;
}

/******************************************************************************/
/*
      uart3_SendChar(int ch)
      Write ch to USART_DR
      after checking that buffer is empty

      ch : data	to be written to register
*/
/******************************************************************************/

// write	single char to USART
void uart3_SendChar(int	ch)
{
    // check	Tx buffer is empty
    while((USART3->SR &	(1 << 7)) == 0)	{
	// Wait	while Tx Buffer	is	not empty
	// wait	until TXE = 1
    }	 // Bit	7 TXE: Transmit	data register empty

    // Load data	to DR register
    USART3->DR = (ch & 0xFF); /*	'&0xFF'	makes	sure value	sent is	8bits (my guess). Might	be better ways to implement
			       *	'&0x7F'	might be used when parity is  enables and data is 7 bits. (agin	my guess)
			       */
    // check TC	> Transmission complete
    while((USART3->SR &	(1 << 6)) == 0)	   // Bit 6	TC: Transmission complete
    {
	// Wait	till TC	is	set
    }
}

/******************************************************************************/
/*

    uart3_SendString (char *string)

    send string	in argument to usart

*/
/******************************************************************************/

void uart3_SendString(char *string)
{
    while(*string)    // while string is not null
    {
	// send	char in	string
	uart3_SendChar(*string++);
    }
}
/******************************************************************************/
/*
      uart3__GetChar()
      Read ch from USART_DR

      Returns char: data read from DR
*/
/******************************************************************************/

// read	single char from	USART
int uart3__GetChar(void)
{
    // Wait for	RXNE to	set
    while((USART3->SR &	(1 << 5)) == 0)	   // Bit 5 RXNE: Read data register not	empty
    {
	// Wait	while Rx	Buffer is	empty or wait till it is not empty i.e.	till data is filled into DR
	// RXNE	= 1: Received data is ready to	be read.
    }

    return USART3->DR;
}

/******************************************************************************/
/*
      led(int led)
      blink LED	based on inpput
*/
/******************************************************************************/

void led(int number)
{
    while(number-- > 0)	{
	sudo_delay(200);
	GPIOD->BSRR |= GPIO_BSRR_BS15;
	sudo_delay(200);
	GPIOD->BSRR |= GPIO_BSRR_BR15;
    }
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
    char my_string[] = "Hello World!!\r\n";
    int	number = 0;
    uart3_SendString(my_string);

    do {
	sudo_delay(50);
	uart3_SendString("Enter	a number\r\n");
	number = toDigit(uart3__GetChar());
	led(number);
	uart3_SendString("\r\n");
    } while(1);
}

/******************************************************************************/
/*
	OLD IMPLEMENTATION  WITHOUT stdio REDIRECTION/overloading
  */
/******************************************************************************/
/*
int main(void)
{

    //Initialise
    init();
    int	number;
    char my_string[] = "Hello World!!\r\n";

    // WRITE DATA TO Tx
    int	size_my_string = sizeof(my_string) / sizeof(*my_string);

    do {

	if(GPIOA->IDR &	GPIO_IDR_ID0) {	   // if	PA0	is high
					   //Read data	from Rx

	    number = toDigit(usart3_read());
	    led(number);
	}
	else {
	    //	Send one char at a time.
	    for(int i =	0; i <=	size_my_string;	i++) {
		usart3_write(my_string[i]);
		sudo_delay(10);
	    }
	    //sudo_delay(100);
	}
    } while(1);
}
    */
/*************************** End of file ****************************/