#include <avr/io.h>
#include "usart_driver.h"
#include "avr_compiler.h"


/*! Define that selects the Usart used in example. */
#define F_CPU 32000000UL
#define PC_USART USARTC0
#define	IMU_USART USARTE0

void init32MHzClock(void);
uint8_t get_USART_char(void);
void put_USART_char(uint8_t);
/*! Success variable, used to test driver. */


int main(void)
{
	
	init32MHzClock();
	/* Variable used to send and receive data. */
	uint8_t sendData;
	
	
	/* PC3 (TXD0) as output. */
	PORTC.DIRSET = PIN3_bm;
	/* PC2 (RXD0) as input. */
	PORTC.DIRCLR = PIN2_bm;
	/* PC3 (TXD0) as output. */
	PORTE.DIRSET = PIN3_bm;
	/* PC2 (RXD0) as input. */
	PORTE.DIRCLR = PIN2_bm;

	/* USARTC0, 8 Data bits, No Parity, 1 Stop bit. */
	USART_Format_Set(&IMU_USART, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);
	USART_Format_Set(&PC_USART, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);


	USART_Baudrate_Set(&IMU_USART, 1079 , -5);
	USART_Baudrate_Set(&PC_USART, 1079 , -5);

	/* Enable both RX and TX. */
	USART_Rx_Enable(&IMU_USART);
	USART_Tx_Enable(&PC_USART);



	/* Send data from 255 down to 0*/
	sendData = 0;
	while(1) 
	{
	
		sendData = get_USART_char();
		put_USART_char(sendData);

	}

}

uint8_t get_USART_char()
{
	do{
	
	
	}while(!USART_IsRXComplete(&IMU_USART));
	return USART_GetChar(&IMU_USART);
}

void put_USART_char(uint8_t sendThis)
{
	    /* Send one char. */
		do{
		/* Wait until it is possible to put data into TX data register.
		 * NOTE: If TXDataRegister never becomes empty this will be a DEADLOCK. */
		}while(!USART_IsTXDataRegisterEmpty(&PC_USART));
		USART_PutChar(&PC_USART, sendThis);
}

void init32MHzClock(void)
{

	//	Enable the 32 MHz internal RC oscillator.
	OSC.CTRL|=OSC_RC32MEN_bm;
	//	The R32MRFY flag is set when the 32 MHz internal RC oscillator is stable
	//	and ready to be used as the System Clock source.
	while(!(OSC.STATUS &OSC_RC32MRDY_bm ));
	CCP=CCP_IOREG_gc;
	//	Select the 32MHz RC oscillator for the system clock
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
	
	//  Use to output clock to a pin
	//  this must be done on port pin 7, see data sheet.
	//PORTD.DIRSET =  PIN7_bm;                   /* To output clock, port must be set as output */
	//PORTCFG.CLKEVOUT = PORTCFG_CLKOUT_PD7_gc; /* Output the clock frequency on PD7 to measure on Counter/Scope */
	
}

