#include "os.h"
#include <stdio.h>
#include "messageQ.h"
#include <assert.h>


Msgq* msgq;


void smain()
{
	msgq = (Msgq*)malloc(2 * sizeof(Msgq));
	msgq[1].maxcounter = 5;

	msgq[1].F = 0;
	msgq[1].R = 0;
	msgq[1].Message_Queue = (MQ*)calloc(msgq[1].maxcounter, sizeof(MQ));

	for (int i = 0; i < msgq[1].maxcounter; i++)
	{
		msgq[1].Message_Queue[i].message = (char*)calloc(10, sizeof(char));
	}


	char buf[] = "wo";
	char buf2[] = "shi";
	char buf3[] = "ni";
	char buf4[] = "baba";

	/*char *buf5 = (sizeof(buf)/sizeof(buf[0]));
	char *buf6; = (sizeof(buf2) / sizeof(buf[0]));
	char *buf7; = (sizeof(buf3) / sizeof(buf[0]));
	char *buf8; = (sizeof(buf4) / sizeof(buf[0]));
	char *buf9; = (sizeof(buf) / sizeof(buf[0]));
	*/

	push_message_into_MQ(1, buf, msgq);
	push_message_into_MQ(1, buf2, msgq);
	push_message_into_MQ(1, buf3, msgq);
	push_message_into_MQ(1, buf4, msgq);



	get_message_from_MQ(1, buf5, msgq);
	printf("%s", buf5);
	get_message_from_MQ(1, buf6, msgq);
	printf("%s", buf6);
	get_message_from_MQ(1, buf7, msgq);
	printf("%s", buf7);
	get_message_from_MQ(1, buf8, msgq);
	printf("%s", buf8);



}

void running()
{

}