/*

*/

/*

Test on basic functions of mutex
*/

#define TASK(t) TASK_##t()

#include <stdio.h>
#include "os.h"
#include <stdlib.h>

int task_counter[NUM_OF_TASKS];
int flag;

mutex_pt m1;
mutex_pt m2;
sem_pt s1;

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

	flag = mutex_lock(m1);
	current_pc[1]++;
	scheduler();
	return;
L_1_1:
	printf("task1 sem_take and t1 prio %d \n\n", task_dyn_info[1].dyn_prio);
	current_pc[1]++;
	sem_take(s1);
	scheduler();
	return;
L_1_2:
	printf("t1 unlock mutex t1 prio %d \n\n",task_dyn_info[1].dyn_prio);
	current_pc[1]++;
	mutex_unlock(m1);
	printf(" t1 prior %d \n\n", task_dyn_info[1].dyn_prio);
	scheduler();
	return;
L_1_3:

	printf(" Task1 Termiante\n\n");
	current_pc[1] = 0;
	flag = TerminateTask();
	scheduler();
	//if (flag)
	return;
	
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


void TASK(2)
{
	JUMP_2();
L_2_0:
	printf("task2 create task1\n\n");
	current_pc[2]++;
	task_create(1);
	scheduler();
	return;
L_2_1:
	printf("task2 locking mutex2\n\n");
	current_pc[2]++;
	flag = mutex_lock(m1);
	scheduler();
	return;
L_2_2:
	printf("task2 unlock m2\n\n");
	current_pc[2]++;
	flag = mutex_unlock(m1);
	scheduler();
	//if (flag)
L_2_3:
	printf("Task2 Terminate\n\n");
	current_pc[2] = 0;
	flag = TerminateTask();
	scheduler();
	//if (flag)
	return;

L_2_4:
	;
L_2_5:
	;
L_2_6:
	;
L_2_7:
	;
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
		case 5:\
			goto L_3_5;break;\
		case 6:\
			goto L_3_6;break;\
		case 7:\
			goto L_3_7;break;\
	}\
}



void TASK(3)

{
	JUMP_3();
L_3_0:
	printf("task3 create task2\n\n");
	current_pc[3]++;
	flag = task_create(2);
	scheduler();
	return;

L_3_1:
	printf("task3 sem give and ti pri %d\n\n",task_dyn_info[1].dyn_prio);
	current_pc[3]++;
	sem_give(s1);
	scheduler();
	return;

L_3_2:
	printf("Task3 Terminate\n\n");
	current_pc[3] = 0;
	flag = TerminateTask();
	scheduler();
	//if (flag)
	return;
L_3_3:
	;
L_3_4:
	;
L_3_5:
	;
L_3_6:
	;
L_3_7:
	;
}



void main()
{
	//mutex = (mutex_pt*)malloc(sizeof(mutex_pt));
	
	mutex_create(&m1);
	sem_create(&s1);

	//mutex_create(&m2);
	printf("create task1.\n\n");
	task_create(3);
	ubik_comp_start();

	mutex_delete(&m1);
	//mutex_delete(&m2);
	sem_delete(&s1);
	ShutDownOS();
	//free(mutex);
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