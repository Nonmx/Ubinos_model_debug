#include "messageQ.h"
#include <stdio.h>
//#include "ubinos.h"
#include <string.h>




int MQ_empty(msgq_pt msid, Msgq* msgq)
{
	if (msgq[msid].F == msgq[msid].R)
		return 1;//empty
	else
		return 0;//nonempty
}

int MQ_full(msgq_pt msid, Msgq* msgq)
{
	if ((msgq[msid].R + 1+ msgq[msid].maxcounter) % msgq[msid].maxcounter == msgq[msid].F)
		return 1;
	else
		return 0;
}

int push_message_into_MQ(msgq_pt msid, unsigned char* message,Msgq* msgq)
{
	if (MQ_full(msid,msgq))
	{
		printf("messageQ is full!\n\n");
		return -1;
	}
	else
	{
		memcpy(msgq[msid].Message_Queue[msgq[msid].R].message, message, sizeof(message)/sizeof(char));

		msgq[msid].R = (msgq[msid].maxcounter + msgq[msid].R + 1) % msgq[msid].maxcounter;

		return 0;
	}
}

int get_message_from_MQ(msgq_pt msid, unsigned char* message, Msgq* msgq)
{
	//	printf("mess is %s \n\n", Message_Queue->messgae);
	if (MQ_empty(msid,msgq))
	{
		printf("messageQ is empty\n\n");
		return -1;
	}
	else
	{
		memcpy(message, msgq[msid].Message_Queue[msgq[msid].F].message ,sizeof(msgq[msid].Message_Queue[msgq[msid].F].message)/sizeof(char));

		//printf("mess is %s \n\n", message);
		msgq[msid].F = (msgq[msid].F + 1) % msgq[msid].maxcounter;
		return 0;
	}
}

