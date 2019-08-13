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
	printf("task1 create task2 \n");
	current_pc[1]++;
	flag = task_create(2);
	scheduler();
	return;
L_1_2:
	printf("task1 unlock m1 \n\n");
	current_pc[1]++;
	mutex_unlock(m1);
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
	printf("task2 locking mutex2\n\n");
	current_pc[2]++;
	flag = mutex_lock(m2);
	scheduler();
		return;
L_2_1:
	printf("task2 create task3\n\n");
	current_pc[2]++;
	flag = task_create(3);
	scheduler();
	return;
L_2_2:
	printf("task2 unlock m2\n\n");
	current_pc[2]++;
	flag = mutex_unlock(m2);
	scheduler();
	//if (flag)
		return;
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
	printf("task3 create task4\n\n");
	current_pc[3]++;
	flag = task_create(4);
	scheduler();
	return;

L_3_1:
	printf("task3 locking mutex1\n\n");
	current_pc[3]++;
	flag = mutex_lock(m1);
	scheduler();
	return;
	
L_3_2:
	printf("task3 unlocking mutex1\n\n.");
	current_pc[3]++;
	flag = mutex_unlock(m1);
	scheduler();
	//if (flag)
		return;
L_3_3:
	printf("Task3 Terminate\n\n");
	current_pc[3] = 0;
	flag = TerminateTask();
	scheduler();
	//if (flag)
		return;
L_3_4:
	;
L_3_5:
	;
L_3_6:
	;
L_3_7:
	;
}

#define JUMP_4(){\
	switch (current_pc[4])\
	{\
		case 0:\
			goto L_4_0; break;\
		case 1:\
			goto L_4_1; break;\
		case 2:\
			goto L_4_2; break;\
	}\
}

void TASK(4)
{
	JUMP_4();
L_4_0:
	printf("task4 lock m2\n\n");
	current_pc[current_tid]++;
	mutex_lock(m2);
	scheduler();
	return;
L_4_1:
	printf("task4 unlock m2\n\n");
	current_pc[current_tid]++;
	mutex_unlock(m2);
	scheduler();
	return;
L_4_2:
	printf("task4 terminate \n\n");
	current_pc[current_tid] = 0;
	TerminateTask();
	scheduler();
	return;
}


void main()
{
	//mutex = (mutex_pt*)malloc(sizeof(mutex_pt));

	mutex_create(&m1);

	mutex_create(&m2);
	printf("create task1.\n\n");
	task_create(1);
	ubik_comp_start();

	mutex_delete(&m1);
	mutex_delete(&m2);

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
		else if (current_tid == 4)
			TASK(4);
	}
}