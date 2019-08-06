#define TASK(t) TASK_##t()
/*
	message API test
*/

#include "os.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
//#include "readyQ.h"

//unsigned char buf1[25];
//unsigned char buf2[25];
msgq_pt _g_msgq;
int mess_flag[NUM_OF_TASKS];
int RR();

int flag;
int counter;

int i = 0;
int j = 0;
int l = 0;

#define jump_1(){\
	switch (current_pc[1])\
	{\
		case 0:\
			goto L_1_0;break;\
		case 1:\
			goto L_1_1;break;\
		case 2:\
			goto L_1_2;break;\
		case 3:\
			goto L_1_3;break;\
		case 4:\
			goto L_1_4;break;\
		case 5:\
			goto L_1_5;break;\
	}\
}

int num = 0;
//unsigned char buf[25] = "hello world\n\n";
void TASK(1) //Adding
{
	unsigned char buf[25] = "hello world \n\n";
	jump_1();
L_1_0:

	current_pc[1] = 1;
	//flag = RR();
	//if (flag)
		//return;
L_1_1:

	//int i = 0;
	current_pc[1] = 2;
	while (i < 41)
	{
		jump_1();
	L_1_2:

		current_pc[1] = 3;
		i++;
		//num++;
		printf("task1 has produced one\n\n");
		//printf("task3 sem_data: %d\n\n", sem[0]
			//.counter);
		mess_flag[1] = msgq_send(&_g_msgq, buf);
		if (mess_flag[1])
			return;

	L_1_3:

		current_pc[1] = 4;
		flag = RR();
		if (flag)
			return;

	L_1_4:
		current_pc[1] = 2;
	}
	current_pc[1] = 5;
	flag = RR();
	if (flag)
		return;
L_1_5:
	current_pc[1] = 0;
	printf("task1 going to Terminate\n\n");
	flag = TerminateTask();
	if (flag)
		return;

}



#define jump_2(){\
	switch (current_pc[2])\
	{\
		case 0:\
			goto L_2_0;break;\
		case 1:\
			goto L_2_1;break;\
		case 2:\
			goto L_2_2;break;\
		case 3:\
			goto L_2_3;break;\
	}\
}

void TASK(2)
{
	unsigned char buf[25];
	jump_2();
L_2_0:

	while (j < 20)
	{
		j++;
		//printf("i am task2");
		//printf("task2 sem_data: %d\n\n", sem[0].counter);
		mess_flag[2] = msgq_receive(&_g_msgq, buf);
				//printf("task2, mess is %s\n\n", buf);
		current_pc[2] = 1;
		//printf("task2 sem_data: %d\n\n", sem[0].counter);
		if (mess_flag[2])
			return;
		//printf("task 2 consumes one\n\n");

	L_2_1:
		printf("task2 counsumes one\n\n");
		printf("mess is %s\n\n", buf);
	}
L_2_2:
	current_pc[2] = 0;
	printf("task2 going to Terminate\n\n");
	flag = TerminateTask();
	if (flag)
		return;
L_2_3:
	;


}

#define jump_3(){\
	switch (current_pc[3])\
	{\
		case 0:\
			goto L_3_0;break;\
		case 1:\
			goto L_3_1;break;\
		case 2:\
			goto L_3_2;break;\
		case 3:\
			goto L_3_3;break;\
	}\
}

void TASK(3)
{
	unsigned char buf[25];
	jump_3();
L_3_0:
	current_pc[3] = 1;
	while (l < 20)
	{
		l++;
		//printf("task3 sem_data: %d\n\n", sem[0].counter);
		mess_flag[3] = msgq_receive(&_g_msgq, buf); 
		printf("task3 ,mess is %s\n\n", buf);
		//printf("task3 sem_data: %d\n\n", sem[0].counter);
		if (mess_flag[3])
			return;
		//printf("task 3 consumes one\n\n");
	L_3_1:

		printf("task 3 counsumes one\n\n");
		
	}
L_3_2:
	current_pc[3] = 0;
	printf("task3 going to Terminate\n\n");
	flag = TerminateTask();
	if (flag)
		return;

L_3_3:
	;

}



void main()
{


	msgq_create(&_g_msgq, 25, 0);

	task_create(1);
	task_create(2);
	task_create(3);

	ubik_comp_start();
	msgq_delete(&_g_msgq);

	
}

void running()
{
	while (current_tid >= 0)
	{
		if (current_tid == 1)
			TASK(1);
		else if (current_tid == 2)
			TASK(2);
		else if (current_tid == 3)
			TASK(3);
	}
}

extern int Round_Robin_Schedule();

int RR()
{
	if ((Round_Robin_Schedule()) != 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}