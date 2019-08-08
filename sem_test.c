#define TASK(t) TASK_##t()
/*
	semphore test
*/

#include "os.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
//#include "readyQ.h"


sem_pt sem;
int sem_flag[NUM_OF_TASKS];
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
void TASK(1) //Adding

{
	jump_1();
L_1_0:

	current_pc[1]++;
	scheduler();
		return;
L_1_1:
	current_pc[1] = 2;
	//int i = 0;
	while (i < 6)
	{
		jump_1();
	L_1_2:
		current_pc[1] = 3;
		i++;
		//num++;
		printf("task1 has produced one\n\n");
		//printf("task3 sem_data: %d\n\n", sem[0]
			//.counter);
		sem_flag[1] = sem_give(&sem);
		if (!sem_flag[1])
			scheduler();
		return;

	L_1_3:

		current_pc[1] = 4;
		scheduler();
		return;


	L_1_4:
		current_pc[1] = 2;
	}
	current_pc[1] = 5;
	scheduler();
	return;
L_1_5:
	current_pc[1] = 0;
	printf("task1 going to Terminate\n\n");
	flag = TerminateTask();
	if (!flag)
		scheduler();
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
	jump_2();
L_2_0:
	current_pc[2]++;
	while (j < 3)
	{
		j++;
		//printf("i am task2");
		//printf("task2 sem_data: %d\n\n", sem[0].counter);
		sem_flag[2] = sem_take(&sem);
		//printf("task2 sem_data: %d\n\n", sem[0].counter);
		if (!sem_flag[2])
			scheduler();
		return;
		//printf("task 2 consumes one\n\n");
	L_2_1:
		printf("task2 counsumes one\n\n");
	}
L_2_2:
	current_pc[2] = 0;
	printf("task2 going to Terminate\n\n");
	flag = TerminateTask();
	if (!flag)
		scheduler();
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
	jump_3();
L_3_0:
	current_pc[3]++;
	while (l < 3)
	{
		l++;
		//printf("task3 sem_data: %d\n\n", sem[0].counter);
		sem_flag[3] = sem_take(&sem);
		//printf("task3 sem_data: %d\n\n", sem[0].counter);
		if (!sem_flag[3])
			scheduler();
		return;
		//printf("task 3 consumes one\n\n");
	L_3_1:

		printf("task 3 counsumes one\n\n");
	}
L_3_2:
	current_pc[3] = 0;
	printf("task3 going to Terminate\n\n");
	flag = TerminateTask();
	if (!flag)
		scheduler();
	return;

L_3_3:
	;

}



void main()
{
	//sem = (sem_pt*)malloc(sizeof(sem_pt));

	sem_create(&sem);

	task_create(1);
	task_create(2);
	task_create(3);

	ubik_comp_start();

	sem_delete(&sem);
	//assert(num == 0);
	//system("pause");
	//free(sem);
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
