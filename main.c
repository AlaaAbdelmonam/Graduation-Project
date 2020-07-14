

#define F_CPU 12000000UL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LCD.h"
#include "GPRS.h"
#include "USART_Interrupt.h"
#include "Std_Types.h"
#include "Utils.h"

int main(void)
{
	buffer_pointer = 0;
	memset(message_received, 0, 60);
	SETBIT(DDRD, 2) ;
    LCD_Init();
	USART_Init(9600);
	sei();
	LCD_String_xy(1,0,"GSM reading");
	_delay_ms(3000);
	LCD_Clear();
	LCD_String_xy(1,0,"AT");
	GPRS_Begin();
	LCD_Clear();
	CLRBIT(PORTD, 2) ;
	GPRS_Send_Msg("+201120842065","GSM");
	_delay_ms(1000);

	while (1){}
}


