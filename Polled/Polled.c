#include <avr/io.h>
#include "usart_driver.h"
#include "avr_compiler.h"


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

uint16_t pitch,roll,yaw;


int main(void)
{
	init32MHzClock();
	//init_2_and32Mhz_clocks();
	/* Variable used to send and receive data. */
	uint8_t sendData;
	
	
	///* PC3 (TXD0) as output. */
	//PORTC.DIRSET = PIN3_bm;
	///* PC2 (RXD0) as input. */
	//PORTC.DIRCLR = PIN2_bm;
	/* PC3 (TXD0) as output. */
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


	//  %
	
	sendData = 0;
	uint8_t i;
	while(1) 
	{
		// find the start frame ($) char
		while (get_USART_char() != START_FRAME_TOKEN)  
		{;}			
		
		while ((sendData = get_USART_char()) != COMMA)
		{
			//  shift data over 8 bits and then add new bits
			//yaw = (yaw << 8) + sendData;
			put_USART_char(sendData);

		}		
		put_USART_char(0x0D);
		while ((sendData = get_USART_char()) != COMMA)  
		{
			//pitch = (pitch << 8) + sendData;
			put_USART_char(sendData);
		}
		put_USART_char(0x0D);
		while ((sendData = get_USART_char()) != END_FRAME_TOKEN)  
		{
			//roll = (roll << 8) + sendData;
			put_USART_char(sendData);
		
		}
		put_USART_char(0x0D);
	
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

//void init_2_and32Mhz_clocks()
//{
	   //OSC.CTRL |= OSC_RC32MEN_bm | OSC_RC2MEN_bm | OSC_RC32KEN_bm;
   //while((OSC.STATUS & OSC_RC32MRDY_bm)==0){} // wait until stable
   //while((OSC.STATUS & OSC_RC2MRDY_bm)==0){} // wait until stable 
   //while((OSC.STATUS & OSC_RC32KRDY_bm)==0){} // wait until stable 
//
   //// enable DFLL (on both RC clocks) 
   //DFLLRC32M.CTRL = DFLL_ENABLE_bm ; 
   //DFLLRC2M.CTRL = DFLL_ENABLE_bm ; 
//
   //// And now switch to the 32M as a clocksource. 
   //CCP = CCP_IOREG_gc;            // protected write follows    
   //CLK.CTRL = CLK_SCLKSEL_RC32M_gc;   // The System clock is now the 32Mhz internal RC 
//}
//
