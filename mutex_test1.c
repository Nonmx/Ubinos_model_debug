/*

Test on basic functions of mutex
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
	printf("task1 locking mutex\n\n");
	current_pc[1]++;
	flag = mutex_lock(&mutex);
	if (flag)
		return;
L_1_1:
	printf("task1 ublocking mutex\n");
	current_pc[1]++;
	flag = mutex_unlock(&mutex);
	if (flag)
		return;
L_1_2:
	printf("task1 sleeping\n\n");
	current_pc[1]++;
	flag = task_sleep(1000);
	if (flag)
		return;
L_1_3:
	printf("time1 try locking mutex after 1000 sec\n\n");
	current_pc[1]++;
	//flag = mutex_lock_timed(&mutex, 1000);
	if (flag)
		return;
L_1_4:
	printf(" Task1 Termiante\n\n");
	current_pc[1] = 0;
	flag = TerminateTask();
	if (flag)
		return;
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


void TASK(2)
{
	JUMP_2();
L_2_0:
	printf("task2 locking mutex\n\n");
	current_pc[2]++;
	flag = mutex_lock(&mutex);
	if (flag)
		return;
L_2_1:
	printf("task2 create task1\n\n");
	current_pc[2]++;
	flag = task_create(1);
	if (flag)
		return;
L_2_2:
	printf("task2 locking mutex one more time\n\n");
	current_pc[2]++;
	flag = mutex_lock(&mutex);
	if (flag)
		return;
L_2_3:
	printf("task2 unlocking mutex\n\n.");
	current_pc[2]++;
	flag = mutex_unlock(&mutex);
	if (flag)
		return;
L_2_4:
	printf("task2 unlock mutex one more time\n\n.");
	current_pc[2]++;
	flag = mutex_unlock(&mutex);
	if (flag)
		return;
L_2_5:
	printf("task2 lockint mutex\n\n");
	current_pc[2]++;
	flag = mutex_lock(&mutex);
	if (flag)
		return;
L_2_6:
	printf("task2 unblocking mutex \n\n");
	current_pc[2]++;
	flag = mutex_unlock(&mutex);
	if (flag)
		return;
L_2_7:
	printf("Task2 Terminate\n\n");
	current_pc[2] = 0;
	flag = TerminateTask();
	if (flag)
		return;
}




void main()
{
	//mutex = (mutex_pt*)malloc(sizeof(mutex_pt));

	mutex_create(&mutex);

	printf("create task2.\n\n");
	task_create(2);
	ubik_comp_start();

	mutex_delete(&mutex);
	//free(mutex);
}

int mutex_checker;
int sleep_checker;
void running()
{
	while (current_tid >= 0)
	{
		mutex_checker++;
		sleep_checker++;
		if (mutex_checker < 10 && sleep_checker < 10)
		{
		sleep_time:

			if (current_tid == 1)
				TASK(1);
			else if (current_tid == 2)
				TASK(2);
		}
		else if (sleep_checker == 10)
		{
			if (time_checker(1))
				goto sleep_time;
		}
		else if (mutex_checker == 11)
		{
			//mutex_time_checker(&mutex, 1);
			mutex_checker = 0;
			sleep_checker = 0;
		}

	}
}