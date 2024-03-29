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
#include <stdint.h>
#define	toDigit(c) (c-'0')    //convert	number received	in char	to int
			      //  C/C++	standard work on char encodings	where '0' - '9'	are always together
			      //  '9' -	'0' will remove	starting  ascii	of '0' from '9'	resulting in the actual	number i.e. 9

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
    uint32_t Pclk     =	16000000;
    uint32_t BaudRate =	115200;

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
//BRR =	(Pclk +	(BaudRate / 2))	/ BaudRate

static uint16_t	calc_BRR(uint32_t Pclk,	uint32_t BaudRate)
{
  uint16_t brr = (Pclk + (BaudRate / 2)) / BaudRate;
  return  brr;
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

void init() {
    RCC->AHB1ENR  |= (1	<< 3);	  //enable port	D	(1<<3) to use USART3 (PB8,9) and PD12..15 for LEDs

    RCC->APB1ENR  |= (1	<< 18);	  //RCC_APB1ENR_USART3EN = (1<<18)

    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOAEN;	  //enable PORT	A for USER button A0

    GPIOD->MODER  |= GPIO_MODER_MODER12_0
		  | GPIO_MODER_MODER13_0
		  | GPIO_MODER_MODER14_0
		  | GPIO_MODER_MODER15_0;	  //set	PD12..15 as o/p	> 0b01



    GPIOD->MODER  |= GPIO_MODER_MODER8_1	  // 0b10 > AF Mode
		  | GPIO_MODER_MODER9_1;	  //set	PD8,9 to AF


    //	0111(9)	0111(8)	>  0x77	put PD8,9 as AF7
    GPIOD->AFR[1] |= (0b111 << 4 * 0) |	(0b111 << 4 * 1);    //blocks of 4bits.	So firs	t4 bits	= 4*0 and then 4*1 etc....

/*
    BAUD = Fck/(16*USARTDIV)
    i.e. USARTDIV = PCLK1/(Baud*16)
      @Baud=115200 @PCLK1= 16 MHZ
	USARTDIV = 8.680
	  i.e.	BRR_mantissa  =	8 = 0x8
		BRR_frac      =	0.680*16 = 10.88  = 11 =0xB
	Hence BRR = 0x8B
    */
    uint32_t Pclk     =	16000000;
    uint32_t BaudRate =	115200;

    USART3->BRR	|= calc_BRR(Pclk, BaudRate);	//set baurdate	= 115200

    USART3->CR1	|= (1 << 3);	//Bit 3	TE: Transmitter	enable
    USART3->CR1	|= (1 << 2);	//Bit 2	RE: Receiver enable

    /* UE can be enables in same line, but is recommended to enable USART AFTER	enabling all other config */
    USART3->CR1	|= (1 << 13);	 //Bit 13 UE: USART	Enable


    //init complete
    GPIOD->BSRR	|= GPIO_BSRR_BS14;
    sudo_delay(500);
    GPIOD->BSRR	|= GPIO_BSRR_BR14;
  }

/******************************************************************************/
/*
      usart_write(int ch)
      Write ch to USART_DR
      after checking that buffer is empty

      ch : data	to be written to register
*/
/******************************************************************************/

//void usart3_write(int	ch)
//{
//    //check	Tx buffer is empty
//    while(!(USART3->SR & (1 << 7))) {
//    };			//Bit	7 TXE: Transmit	data register empty
//    USART3->DR = (ch & 0xFF);	/* '&0xFF' makes	sure value	sent is	8bits (my guess). Might	be better ways to implement
//				     *	'&0x7F'	might be used when parity is  enables and data is 7 bits. (agin	my guess)
//				     */
//}

//write	single char to USART
void usart3_putc(char ch)
{
    //check	Tx buffer is empty
    while(!(USART3->SR & (1 << 7))) { //Wait while Tx Buffer is	not empty
    };			      //Bit	7 TXE: Transmit	data register empty
    USART3->DR = (ch & 0xFF); /*	'&0xFF'	makes	sure value	sent is	8bits (my guess). Might	be better ways to implement
				     *	'&0x7F'	might be used when parity is  enables and data is 7 bits. (agin	my guess)
				     */
}
/******************************************************************************/
/*
      usart_read()
      Read ch from USART_DR

      Returns char: data read from DR
*/
/******************************************************************************/

//char usart3_read()
//{

//    while(!(USART3->SR & (1 << 5))) {
//    };    //Bit	5 RXNE:	Read data	register not empty; RXNE = 1

//    return USART3->DR;
//}

//read single char from	USART
char usart3_getc()
{
    while(!(USART3->SR & (1 << 5))) {	//Wait while Rx	Buffer is empty	or wait	till it	is not empty
    };	  //Bit	5 RXNE:	Read data	register not empty; RXNE = 1

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
    //Initialise
    init();
    int	number;
    static char	my_string[] = "\r\nWelcome to UART Demo!!\r\nEnter Number of times to blink the	led:\r\n";
    int	size_my_string = sizeof(my_string) / sizeof(*my_string);
    do {
	// WRITE DATA TO Tx
	for(int	i = 0; i <= size_my_string; i++) {
	    usart3_putc(my_string[i]);
	    sudo_delay(10);
	}
	sudo_delay(50);

	//Read number of time to blink
	number = toDigit(usart3_getc());
	led(number);

    } while(1);
}

/*************************** End of file ****************************/