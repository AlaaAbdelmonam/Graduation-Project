/*
 * GPRS.c
 *
 *  Created on: Jun 28, 2020
 *      Author: Alaa
 */

#define F_CPU 12000000UL						/* define Clock Frequency */
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Std_Types.h"
#include "Utils.h"
#include "USART.h"
#include "GPRS.h"
#include "LCD.h"

char Mobile_no[14];
char message_received[60];
int position = 0;


void GPRS_Begin()
{
	SETBIT(PORTD, 2);
	while(1)
		{
		LCD_Command(0xc0);
		USART_SendString("AT\r\n");			     /* send AT to check module is ready or not */
		USART_SendString("AT+IPR=0\r\n");
		_delay_ms(500);
		USART_SendString("AT\r\n");
		_delay_ms(2000);
		//AT+IPR=0
		if(strstr(buff,"OK\r\n"))
				{
					GPRS_Response();
					memset(buff,0,160);
					LCD_String("OK");
					break;
				}
				else
				{
					LCD_String("Error");
				}
		}
	_delay_ms(1000);
	LCD_Clear();
	USART_SendString("AT+CNMP=13\r\n");        //to set GPRS mode
	_delay_ms(200);
	LCD_String_xy(1,0,"Text Mode");
	LCD_Command(0xc0);
	USART_SendString("AT+CMGF=1\r\n");        /* select message format as text */
	GPRS_Response();
	_delay_ms(1000);
	CLRBIT(PORTD, 2) ;
}



void GSM_Send_Msg(char *num ,char *sms)
{
	SETBIT(PORTD, 2) ;
	char sms_buffer[35];
	buffer_pointer=0;
	sprintf(sms_buffer,"AT+CMGS=\"%s\"\r\n",num);
	USART_SendString(sms_buffer);                       /*send command AT+CMGS="Mobile No."\r */
	_delay_ms(200);
	while(1)
	{
		if(buff[buffer_pointer]==0x3e)                  /* wait for '>' character*/
		{
			buffer_pointer = 0;
			memset(buff,0,strlen(buff));
			USART_SendString(sms);                      /* send msg to given no. */
			USART_TxChar(0x1a);                         /* send Ctrl+Z then only message will transmit*/
			break;
		}
		buffer_pointer++;
	}
	_delay_ms(300);
	buffer_pointer = 0;
	memset(buff,0,strlen(buff));
	memset(sms_buffer,0,strlen(sms_buffer));
	CLRBIT(PORTD, 2) ;
	LCD_String("SMS Send");
}



void GPRS_Response()
{
	unsigned int timeout=0;
	int CRLF_Found=0;
	char CRLF_buff[2];
	int Response_Length=0;
	while(1)
	{
		if(timeout>=60000)								/*if timeout occur then return */
		return;
		Response_Length = strlen(buff);
		if(Response_Length)
		{
			_delay_ms(2);
			timeout++;
			if(Response_Length==strlen(buff))
			{
				for(int i=0;i<Response_Length;i++)
				{
					memmove(CRLF_buff,CRLF_buff+1,1);
					CRLF_buff[1]=buff[i];
					if(strncmp(CRLF_buff,"\r\n",2))
					{
						if(CRLF_Found++==2)				/* search for \r\n in string */
						{
							GPRS_Response_Display();		/* display response */
							return;
						}
					}

				}
				CRLF_Found = 0;

			}

		}
		_delay_ms(1);
		timeout++;
	}
	status_flag=0;
}

void GPRS_Response_Display()
{
	buffer_pointer = 0;
	int lcd_pointer = 0;
	while(1)
	{
		if(buff[buffer_pointer]== '\r' || buff[buffer_pointer]== '\n')  /* search for \r\n in string */
		{
			buffer_pointer++;
		}
		else
			break;
	}


	LCD_Command(0xc0);
	while(buff[buffer_pointer]!='\r')								   /* display response till "\r" */
	{
		LCD_Char(buff[buffer_pointer]);
		buffer_pointer++;
		lcd_pointer++;
		if(lcd_pointer==15)								               /* check for end of LCD line */
		LCD_Command(0x80);
	}
	buffer_pointer=0;
	memset(buff,0,strlen(buff));
}

void GPRS_Msg_Read(int position)
{
	char read_cmd[10];
	sprintf(read_cmd,"AT+CMGR=%d\r",position);
	USART_SendString(read_cmd);							/* read message at specified location/position */
	GSM_Msg_Display();									/* display message */
}

void GPRS_Msg_Display()
{
	_delay_ms(500);
	if(!(strstr(buff,"+CMGR")))                         /*check for +CMGR response */
	{
		LCD_String_xy(1,0,"No message");
	}
	else
	{
		buffer_pointer = 0;

		while(1)
		{
			if(buff[buffer_pointer]=='\r' || buff[buffer_pointer]== 'n')  /*wait till \r\n not over*/
			{
				buffer_pointer++;
			}
			else
			break;
		}

		/* search for 1st ',' to get mobile no.*/
		while(buff[buffer_pointer]!=',')
		{
			buffer_pointer++;
		}
		buffer_pointer = buffer_pointer+2;

		/* extract mobile no. of message sender */
		for(int i=0;i<=12;i++)
		{
			Mobile_no[i] = buff[buffer_pointer];
			buffer_pointer++;
		}

		do
		{
			buffer_pointer++;
		}while(buff[buffer_pointer-1]!= '\n');

		LCD_Command(0xC0);
		int i=0;

		/* display and save message */
		while(buff[buffer_pointer]!= '\r' && i<31)
		{
				LCD_Char(buff[buffer_pointer]);
				message_received[i]=buff[buffer_pointer];

				buffer_pointer++;
				i++;
				if(i==16)
					LCD_Command(0x80);						/* display on 1st line */
		}

		buffer_pointer = 0;
		memset(buff,0,strlen(buff));
	}
	status_flag = 0;
}
