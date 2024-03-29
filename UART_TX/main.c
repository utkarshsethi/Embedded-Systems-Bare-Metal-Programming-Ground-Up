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

/******************************************************************************/
/*
      usart3_init()
      Initialise USART3
      PD8 : TX
      PD9 : RX
      @115200
      for TX
*/
/******************************************************************************/

void usart3_init() {
    RCC->AHB1ENR  |= (1	<< 3);	  //enable port	D	(1<<3) to use USART3 (PB8,9) and PD12..15 for LEDs

    RCC->APB1ENR  |= (1	<< 18);	  //RCC_APB1ENR_USART3EN = (1<<18)

    GPIOD->MODER  |= GPIO_MODER_MODER12_0
		  | GPIO_MODER_MODER13_0
		  | GPIO_MODER_MODER14_0
		  | GPIO_MODER_MODER15_0;	  //set	PD12..15 as o/p	> 0b01

    GPIOD->MODER  |= GPIO_MODER_MODER8_1	  // 0b10 > AF Mode
		  | GPIO_MODER_MODER9_1;	  //set	PD8,9 to AF

    GPIOD->AFR[1] |= (0b111 << 4 * 0) |	(0b111 << 4 * 1);   //	0111(9)	0111(8)	>  0x77	//put PD8,9 as AF7

    /*
    BRR	Calculations:

    HCLK = HSI = 16 Mhz
    USART > APB1 clk > PCLK1
    PCLK1 = 4 MHz

    BAUD = Fck/(16*USARTDIV)

    i.e. USARTDIV = PCLK1/(Baud*16)
      @Baud=115200 @PCLK1=4 MHZ
	USARTDIV = 2.170
	  i.e.	BRR_mantissa  =	2 = 0x2
		BRR_frac      =	0.170*16 = 2.72	= 3 =0x3

	Hence BRR = 0x23

    TEST for Ref Mnaual	if 115200 &
	Fpclk =	  4 Mhz	> BRR =	0x23
	Fpclk =	  8 Mhz	> BRR =	0x45
	Fpclk =	 16 Mhz	> BRR =	0x113
*/
    USART3->BRR	|= 0x8B;    //set baurdate	= 115200    See	calc below
    /******************************************************************************/
    /*
	  BRR =	0x23 is	giving actual bud of 460800
	  Pclk = 1Mhz?
	  lets try >
	  USARTDIV = PCLK1/(Baud*16)
	  @Baud=115200 @PCLK1=1	MHZ
	    > USARTDIV =  0.542
	    i.e.	BRR_mantissa  =	0 = 0x0
			BRR_frac      =	0.542*16 = 8.672  = 9 =	0x9
			Hence BRR = 0x09  ?

			NOT WORKING, NEED TO CHECK CALCULATIONS
			WORKING	WITH 0X0683 FOR	BAUD = 9600.
      */
    //TODO
    /*
	Experiment reveals by default APB1 prescalar might be set to 1.
	Will experiment	more after learning clock cofig
    */
    //TODO: try	115200 with new	leaning	Pclk = 16Mhz
    /*
    BAUD = Fck/(16*USARTDIV)
    i.e. USARTDIV = PCLK1/(Baud*16)
      @Baud=115200 @PCLK1= 16 MHZ
	USARTDIV = 8.680
	  i.e.	BRR_mantissa  =	8 = 0x8
		BRR_frac      =	0.680*16 = 10.88  = 11 =0xB

	Hence BRR = 0x8B

	THIS WORKS
	HENCE >	Pclk = 16Mhz
    */
/******************************************************************************/

    USART3->CR1	|= (1 << 3);	//Bit 3	TE:	Transmitter enable
    /* UE can be enables in same line, but is recommended to enable USART AFTER	enabling all other config */
    USART3->CR1	|= (1 << 13);	 //Bit 13 UE: USART	Enable
}

/******************************************************************************/
/*
      usart_write(int ch)
      Write ch to USART_DR
      after checking that buffer is empty

      ch : data	to be written to register
*/
/******************************************************************************/

void usart_write(int ch)
{
    //check	Tx buffer is empty
    while(USART3->SR & (1 << 7)) {    //Bit	7 TXE: Transmit	data register empty
	USART3->DR = (ch & 0xFF);     /* '&0xFF' makes	sure value	sent is	8bits (my guess). Might	be better ways to implement
				     *	'&0x7F'	might be used when parity is  enables and data is 7 bits. (agin	my guess)
				     */
    }
}

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

/*********************************************************************
*
*	main()
*
*  Function description
*   Application	entry point.
*/
int main(void)
{

    usart3_init();
    char my_string[] = "Hello World!!\n";
    int	size_my_string = sizeof(my_string) / sizeof(*my_string);
    do {
	//	Send one char at a time.
	for(int	i = 0; i <= size_my_string; i++) {
	    usart_write(my_string[i]);
	    sudo_delay(10);
	}
	sudo_delay(100);
    } while(1);
}

/*************************** End of file ****************************/