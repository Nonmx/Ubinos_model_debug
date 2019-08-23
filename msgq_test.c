

#define TASK(t) TASK_##t()

#include <stdio.h>
#include "os.h"
#include <stdlib.h>
#include <assert.h>

int task_counter[NUM_OF_TASKS];
int flag;

msgq_pt m1;


#define JUMP_1(){\
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
	}\
}

char buf[13] = "hello world";



void TASK(1)
{
	
	JUMP_1();
L_1_0:
	
	printf("task1 locking mutex\n\n");
	current_pc[1]++;
	
	flag = msgq_send(m1,buf);
	if (!flag)
		scheduler();
		return;
L_1_1:
	printf(" Task1 Termiante\n\n");
	current_pc[1] = 0;
	flag = TerminateTask();
	if (!flag)
		scheduler();
		return;
	L_1_2:
		;
	L_1_3:
		;
	L_1_4:
		;
}


#define JUMP_2(){\
	switch (current_pc[2])\
	{\
		case 0:\
			goto L_2_0; break;\
		case 1:\
			goto L_2_1; break;\
		case 2:\
			goto L_2_2; break;\
		case 3:\
			goto L_2_3;break;\
		case 4:\
			goto L_2_4;break;\
		case 5:\
			goto L_2_5;break;\
		case 6:\
			goto L_2_6;break;\
		case 7:\
			goto L_2_7;break;\
	}\
}

char buf1[13];

void TASK(2)
{

	JUMP_2();
	
L_2_0:

	
	printf("task2 create task1\n\n");
	current_pc[2]++;
	flag = task_create(1);
	if (flag)
		return;

L_2_1:
	printf("task2 locking mutex\n\n");
	current_pc[2]++;
	flag = msgq_receive(m1, buf1);
	if (!flag)
		scheduler();
	return;
L_2_2:
	printf("Task2 Terminate\n\n");
	current_pc[2] = 0;
	printf("%s\n", buf1);
	flag = TerminateTask();
	if (!flag)
		scheduler();
		return;
	L_2_3:
		;
	L_2_4:
		;
	L_2_5:
		;
	L_2_6:
		;
	L_2_7:
		;
}




void main()
{

	msgq_create(&m1,10,10);

	printf("create task2.\n\n");
	task_create(2);
	ubik_comp_start();

	msgq_delete(&m1);
	ShutDownOS();
}

int mutex_checker;
int sleep_checker;
void running()
{
	while (current_tid >= 0)
	{

		assert(!(buf1 == NULL));
		if (current_tid == 1)
			TASK(1);
		else if (current_tid == 2)
			TASK(2);
	}
		
}