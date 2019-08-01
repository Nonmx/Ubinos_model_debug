#define TASK(t) TASK_##t()
/*
이 test case는 같은 priority인 task들이 main함수에서 한꺼번에 정의해서
round robin 식으로 수행하는 test case입니다.
*/

#include "os.h"
#include <stdio.h>
#include "assert.h"
//#include "readyQ.h"

extern int Round_Robin_Schedule();


//int RR();

void LED_1();
void LED_2();
void LED_3();


int task_counter[NUM_OF_TASKS];

int flag;
int counter;


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
	}\
}

void TASK(1)
{
	jump_1();
L_1_0:

	current_pc[1] = 0;
	LED_1();
	current_pc[1]++;
	flag = Round_Robin_Schedule();
	if (flag) return;
L_1_1:
	LED_1();
	flag = Round_Robin_Schedule();
	current_pc[1]++;
	if (flag) return;
L_1_2:
	printf("task1 terminate\n");
	current_pc[1] = 0;
	flag = TerminateTask();
	if (flag)
		return;
L_1_3:
	;
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
	current_pc[2] = 0;
	LED_2();
	flag = Round_Robin_Schedule();
	current_pc[2]++;
	if (flag) return;

L_2_1:

	LED_2();
	flag = Round_Robin_Schedule();
	current_pc[2]++;
	if (flag)
		return;
L_2_2:
	LED_2();
	flag = Round_Robin_Schedule();
	current_pc[2] ++;
	if (flag)
		return;
L_2_3:
	printf("task2 terminate\n");
	current_pc[2] = 0;
	flag = TerminateTask();
	if (flag)
		return;

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
	current_pc[3] = 0;
	LED_3();
	flag = Round_Robin_Schedule();
	current_pc[3]++;
	if (flag) return;
L_3_1:
	LED_3();
	flag = Round_Robin_Schedule();
	current_pc[3]++;
	if (flag)return;
L_3_2:
	LED_3();
	flag = Round_Robin_Schedule();
	current_pc[3]++;
	if (flag)
		return;
L_3_3:
	printf("task3 Terminate\n");
	current_pc[3] = 0;
	flag = TerminateTask();
	if (flag)
		return;
}


void LED_1()
{
	printf("LED1\n");
}

void LED_2()
{
	printf("LED2\n");
}

void LED_3()
{
	printf("LED3\n");
}


void main()
{


	task_create(1);
	task_create(2);
	task_create(3);

	ubik_comp_start();

	//system("pause");
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

