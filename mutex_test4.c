/*
	case of priority inheritance test
*/


#define TASK(t) TASK_##t()

#include <stdio.h>
#include "os.h"
#include <stdlib.h>

int task_counter[NUM_OF_TASKS];
int flag;

mutex_pt mutex;


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


void TASK(1)
{
	JUMP_1();
L_1_0:
	printf("Round robin to task2\n\n");
	current_pc[1]++;
	//flag = task_create(2);
	//if (!flag)
	scheduler();
	return;
L_1_1:
	printf("Task1 Termiante\n\n");
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
			goto L_2_2;break;\
		case 3:\
			goto L_2_3;break;\
	}\
}


void TASK(2)
{
	JUMP_2();
L_2_0:
	printf("task2 locking mutex\n\n");
	current_pc[2]++;
	flag = mutex_lock(mutex);
	if (!flag)
		scheduler();
	return;

L_2_1:
	printf("task2 create task3 \n\n");
	current_pc[2]++;
	flag = task_create(3);
	if (!flag)
		scheduler();
	return;
L_2_2:
	printf("task2 unlocking mutex\n\n");
	current_pc[2]++;
	flag = mutex_unlock(mutex);
	if (!flag)
		scheduler();
	return;
L_2_3:
	printf("task2 terminate\n\n");
	current_pc[2] = 0;
	flag = TerminateTask();
	if (!flag)
		scheduler();
	return;

}
#define JUMP_3(){\
	switch (current_pc[3])\
	{\
		case 0:\
			goto L_3_0; break;\
		case 1:\
			goto L_3_1; break;\
		case 2:\
			goto L_3_2; break;\
	}\
}


void TASK(3)
{
	JUMP_3();
L_3_0:
	printf("task3 try to locking mutex\n\n");
	current_pc[3]++;
	flag = mutex_lock(mutex);
	if (!flag)
		scheduler();
	return;
L_3_1:
	printf("task3 unlock the mutex\n\n");
	current_pc[3]++;
	flag = mutex_unlock(mutex);
	if (!flag)
		scheduler();
	return;
L_3_2:
	printf("task3 Terminate\n\n");
	current_pc[3] = 0;
	flag = TerminateTask();
	if (!flag)
		scheduler();
	return;

}



void main()
{
	//mutex = (mutex_pt*)malloc(sizeof(mutex_pt));

	mutex_create(&mutex);

	printf("create task1 && task2.\n\n");
	task_create(1);
	task_create(2);
	ubik_comp_start();

	mutex_delete(&mutex);
	//free(mutex);
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