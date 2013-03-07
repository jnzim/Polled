/*
 * Stream_USART.c
 *
 * Created: 3/3/2013 3:48:17 PM
 *  Author: jzimmerman
 */ 


#include <avr/io.h>

static int uart_putchar(char c, FILE *stream);
static void uart_init (void);

static FILE mystdout = FDEV_SETUP_STREAM (uart_putchar, NULL, _FDEV_SETUP_WRITE);


int main (void)
{
	uart_init();
	stdout = &mystdout;
	
	while (1)
	printf("Hello, world!\n");
}


static int uart_putchar (char c, FILE *stream)
{
	if (c == '\n')
	uart_putchar('\r', stream);
	
	// Wait for the transmit buffer to be empty
	while ( !( USARTC0.STATUS & USART_DREIF_bm) );
	
	// Put our character into the transmit buffer
	USARTC0.DATA = c;
	
	return 0;
}
