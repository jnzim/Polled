#include <avr/io.h>
#include <stdlib.h> 
#include "usart_driver.h"
#include "avr_compiler.h"
#include "tc_driver.h"
#include "myUtilities.h"
#include "PWM.h"


/*! Define that selects the UASRT used in example. */
#define F_CPU 32000000UL
//#define PC_USART USARTC0
#define	IMU_USART USARTE0
#define START_FRAME_TOKEN	0x24
#define END_FRAME_TOKEN		0x0D
#define COMMA				0x2C
#define MASK_TOP_BYTE		0x00FF

#define NUM_BYTES_IN_ANGLE	4
#define NUM_BYTES_IN_FRAME 32


//void  init_2_and32Mhz_clocks(void);
void init32MHzClock(void);
uint8_t get_USART_char(void);
void put_USART_char(uint8_t);
void initPWM(void);
void doPWM(uint16_t);
void sendData_uint16_t(void);
void sendFrame_ASCII(void);

char frame[NUM_BYTES_IN_FRAME];
char strYaw[NUM_BYTES_IN_ANGLE]; char strPitch[NUM_BYTES_IN_ANGLE]; char strRoll[NUM_BYTES_IN_ANGLE];
uint16_t pitch,roll,yaw;
int16_t pwm_delta = 250;

int main(void)
{
	initPWM();
	init32MHzClock();
	
	//init_2_and32Mhz_clocks();
	/* Variable used to send and receive data. */
	
	
	
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


	char sendData;
	
	char *p;
	sendData = 0;
	uint8_t i,k = 0;


	while(1) 
	{
		while (get_USART_char() != START_FRAME_TOKEN)  {;}			
		frame[k++] = START_FRAME_TOKEN;
		
		while ((sendData = get_USART_char()) != COMMA)
		{
			strYaw[i++] = sendData;
			frame[k++] = sendData;
		}
		frame[k++] = COMMA;  	
		strYaw[i++] = '\0';		
		yaw = strtoUint16(&strYaw[0],NULL,10);									// add termination character
																				// convert string of char to unint16_t
		i=0;
		
		while ((sendData = get_USART_char()) != COMMA)  
		{
		
			strPitch[i++] = sendData;
			frame[k++] = sendData;
		}
		frame[k++] = COMMA;  	
		strPitch[i++] = '\0';
		pitch = strtoUint16(&strPitch[0],NULL,10);										// add termination character
																						// signed long is 32 bits
		i = 0;
		
		while ((sendData = get_USART_char()) != END_FRAME_TOKEN)  
		{
			
			strRoll[i++] = sendData;
			frame[k++] = sendData;
		}
		frame[k++] = END_FRAME_TOKEN;
		strRoll[i++] = '\0';										// add termination character
		
		
		roll = strtoUint16(&strRoll[0],NULL,10);
		i = 0;	k = 0;

		
	//doPWM(yaw);
	//sendFrame_ASCII();
	sendData_uint16_t();
	}
	
	
}

// operate on global variables, split 
//void sendData_uint16_t(uint16_t *pYaw, uint16_t *pPitch, uint16_t *pRoll)
void sendData_uint16_t()
{
	char highbyte, lowbyte;
	
	//highbyte =  (yaw >> 8) & MASK_TOP_BYTE;
	//lowbyte = yaw & MASK_TOP_BYTE;
	//put_USART_char(highbyte);
	//put_USART_char(lowbyte);
	//
	highbyte = (pitch >> 8) & MASK_TOP_BYTE;
	lowbyte = pitch & MASK_TOP_BYTE;
	put_USART_char(highbyte);
	put_USART_char(lowbyte);

	//highbyte = (roll >> 8) & MASK_TOP_BYTE;
	//lowbyte = roll & MASK_TOP_BYTE;
	//put_USART_char(highbyte);
	//put_USART_char(lowbyte);
	//
}


void sendFrame_ASCII()
{
	int n = 0;
	while(frame[n] != END_FRAME_TOKEN)
	{
		put_USART_char(frame[n]);
		n++;
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







