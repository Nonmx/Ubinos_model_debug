#define _CRT_SECURE_NO_WARNINGS

#include "messageQ.h"
#include <stdio.h>
//#include "ubinos.h"
#include <string.h>


extern Msgq* msgq_list;

int MQ_empty(msgq_pt msid)
{
	if (msgq_list[msid].F == msgq_list[msid].R)
		return 1;//empty
	else
		return 0;//nonempty
}

int MQ_full(msgq_pt msid)
{
	if ((msgq_list[msid].R + 1) % msgq_list[msid].maxcounter == msgq_list[msid].F)
		return 1;
	else
		return 0;
}

int push_message_into_MQ(msgq_pt msid, unsigned char* message)
{
	if (MQ_full(msid))
	{
		printf("messageQ is full!\n\n");
		return -1;
	}
	else
	{
		strcpy(msgq_list[msid].Message_Queue[msgq_list[msid].R].message, message);

		msgq_list[msid].R = (msgq_list[msid].R + 1) % msgq_list[msid].maxcounter;

		return 0;
	}
}

int get_message_from_MQ(msgq_pt msid, unsigned char* message)
{
	//	printf("mess is %s \n\n", Message_Queue->messgae);
	if (MQ_empty(msid))
	{
		printf("messageQ is empty\n\n");
		return -1;
	}
	else
	{
		strcpy(message,msgq_list[msid].Message_Queue[msgq_list[msid].R].message);

		//printf("mess is %s \n\n", message);
		msgq_list[msid].F = (msgq_list[msid].F + 1) % msgq_list[msid].maxcounter;
		return 0;
	}
}

