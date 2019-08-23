/*
	���ؽ��� ���� �켱���� ���� ��Ȳ�� �����ϴ� ���ؽ���
	�켱���� ��� priority inheritance Ư�� ����
*/

#define TASK(t) TASK_##t()

#include <stdio.h>
#include "os.h"

int task_counter[NUM_OF_TASKS];
int flag;

mutex_pt *mutex;


#define JUMP_1(){\
	switch (current_pc[1])\
	{\
		case 0:\
			goto L_1_0;break;\
		case 1:\
			goto L_1_1;break;\
		case 2:\
			goto L_1_2;break;\
	}\
}


void TASK(1)
{
	JUMP_1();
L_1_0:
	printf("task1 locking mutex\n\n");
	current_pc[1]++;
	flag = mutex_lock(mutex);
	if (flag == 0)
	{
		scheduler();
		return;
	}
L_1_1:
	printf("task1 unlocking mutex\n\n");


	current_pc[1]++;
	flag = mutex_unlock(mutex);
	if (flag == 0)
	{
		scheduler();
		return;
	}
L_1_2:
	printf("task1 terminate\n\n");
	current_pc[1] = 0;
	flag = TerminateTask();
	if (flag == 0)
	{
		scheduler();
		return;
	}
}


#define JUMP_2(){\
	switch (current_pc[2])\
	{\
		case 0:\
			goto L_2_0; break;\
		case 1:\
			goto L_2_1; break;\
	}\
}


void TASK(2)
{
	JUMP_2();
L_2_0:
	current_pc[2]++;
	printf("i am task2\n\n");

L_2_1:

	printf("task2 Terminate\n\n");
	current_pc[2] = 0;
	flag = TerminateTask();
	if (flag == 0)
	{
		scheduler();
		return;
	}


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
		case 3:\
			goto L_3_3;break;\
		case 4:\
			goto L_3_4;break;\
	}\
}


void TASK(3)
{
	JUMP_3();
L_3_0:
	printf("태스크3 locking mutex \n\n");
	current_pc[3]++;
	flag = mutex_lock(mutex);
	if (flag == 0)
	{
		scheduler();
		return;
	}
		
L_3_1:
	printf("태스크3은 태스크1 생성\n\n");
	current_pc[3]++;
	flag = task_create(1);
	if (flag == 0)
	{
		scheduler();
		return;
	}

L_3_2:
	printf("태스크3은 태스크2를 생성\n\n");
	current_pc[3]++;
	flag = task_create(2);
	if (flag == 0)
	{
		scheduler();
		return;
	}
L_3_3:
	printf("태스크3은 unlocking mutex\n\n.");
	current_pc[3]++;
	flag = mutex_unlock(mutex);
	if (flag == 0)
	{
		scheduler();
		return;
	}
L_3_4:
	printf("Task3 Terminate\n\n");
	current_pc[2] = 0;
	flag = TerminateTask();
	if (flag == 0)
	{
		scheduler();
		return;
	}
}


void main()
{
	mutex = (mutex_pt*)malloc(sizeof(mutex_pt));
	mutex_create(&mutex);

	printf("main 함수에서 태스크3 생성.\n\n");
	task_create(3);
	ubik_comp_start();

	mutex_delete(&mutex);
}

int mutex_checker;
int sleep_checker;
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