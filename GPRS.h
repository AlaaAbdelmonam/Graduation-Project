/*
 * GPRS.h
 *
 *  Created on: Jun 28, 2020
 *      Author: Alaa
 */

#ifndef GSM_H_
#define GSM_H_

extern char message_received[60];
#include <stdbool.h>
void GPRS_Begin();
void GPRS_Send_Msg(char *,char*);
void GPRS_Response();
void GPRS_Response_Display();
void GPRS_Msg_Read(int);
void GPRS_Msg_Display();
#endif /* GSM_H_ */
