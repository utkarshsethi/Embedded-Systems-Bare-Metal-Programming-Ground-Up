#include <stdio.h>

#define	USART_PORT  USART3    //SET TO WHICHEVER	USED

int USART_fgetc(void)
{
    while(!(USART_PORT->SR & (1	<< 5)))	{
    };

    return USART_PORT->DR;
}

int USART_fputc(int ch)
{
    while(!(USART_PORT->SR & (1	<< 7)))	{
    };
    USART_PORT->DR = (ch & 0xFF);
    return ch;
}

struct __FILE {
    int	handle;
};

FILE __stdin = {0};
FILE __stdout =	{1};
FILE __stderr =	{2};

int fgetc(FILE *f)
{
    int	ch;
    ch =  USART_fgetc();
    if(ch == '\r')  {	  //if end of line
	USART_fputc(a);	  //echo
	ch = '\n';	  //set	ch = newline
	}
    USART_fputc(ch);
    return ch;
}

int fputc(int ch, FILE *f)
{
    return USART_fputc(ch);
}