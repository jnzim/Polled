#include <avr/io.h>
#include "usart_driver.h"
#include "avr_compiler.h"
#include <stdio.h>
#include <string.h>



/*! Define that selects the Usart used in example. */
#define F_CPU 32000000UL
//#define PC_USART USARTC0
#define	IMU_USART USARTE0
#define START_FRAME_TOKEN	0x24
#define END_FRAME_TOKEN		0x0D
#define COMMA				0x2C


//void  init_2_and32Mhz_clocks(void);
void init32MHzClock(void);
uint8_t get_USART_char(void);
void put_USART_char(uint8_t);
unsigned char strYaw[8];
int sendData;
int pitch,roll;
int yaw;
sendData = 0;
	



int main(void)
{
	init32MHzClock();

	PORTE.DIRSET = PIN3_bm;
	/* PC2 (RXD0) as input. */
	PORTE.DIRCLR = PIN2_bm;

	/* USARTC0, 8 Data bits, No Parity, 1 Stop bit. */
	USART_Format_Set(&IMU_USART, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);
	//USART_Format_Set(&PC_USART, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);


	USART_Baudrate_Set(&IMU_USART, 1079 , -5);
	//USART_Baudrate_Set(&PC_USART, 1079 , -5);

	/* Enable both RX and TX. */
	USART_Rx_Enable(&IMU_USART);
	USART_Tx_Enable(&IMU_USART);

	uint8_t i = 0;
	while(1) 
	{
		char *p;
		long li;
		char myData[10] = {'1', '2', '3'};
		s = "20y";
		li = strtol(myData,&p,0);
		put_USART_char(li);
		//put_USART_char(myData[3]);
		//put_USART_char(0x0D);
		/*
		 * At this point, "p" will point at the character 'y'
		 * in the string constant, and "li" would have the
		 * value 20
		 */
		//li = strtol(s,NULL,10);   /* assigns 20 to li */
		//put_USART_char(li);
		//put_USART_char(0x0D);
		_delay_ms(250);
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
		}while(!USART_IsTXDataRegisterEmpty(&IMU_USART));
		USART_PutChar(&IMU_USART, sendThis);
}

\
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


