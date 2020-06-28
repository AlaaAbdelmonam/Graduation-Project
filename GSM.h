/*
 * GSM.h
 *
 *  Created on: Jun 28, 2020
 *      Author: Alaa
 */

#ifndef GSM_H_
#define GSM_H_

extern char message_received[60];
#include <stdbool.h>
void GSM_Begin();
void GSM_Calling(char *);
void GSM_HangCall();
void GSM_Response();
void GSM_Response_Display();
void GSM_Msg_Read(int);
bool GSM_Wait_for_Msg();
void GSM_Msg_Display();
void GSM_Msg_Delete(unsigned int);
void GSM_Send_Msg(char *,char*);
void GSM_Delete_All_Msg();


#endif /* GSM_H_ */
