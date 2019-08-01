#include "waitingQ.h"
#include "ubinos.h"
#include <stdio.h>



int act_counter[NUM_OF_TASKS + 1];
int P;
int temp;

void push_task_into_WQ(unsigned char tid, unsigned char p)
{
	if (full(p))
	{
		printf("waittingQ is full!\n");
		//return 0;
	}
	else
	{
		//printf("enQ -> rear: %d\n\n", Rear);
		task_state[tid] = Blocked;
		//printf("task_state[tid][act_counter[tid]] = %d \n", task_state[tid]);
		temp = Rear[p];
		waitingQ[p][temp].tid = tid;
		waitingQ[p][temp].prio = p;
		Rear[p] = (QSIZE + temp + 1) % QSIZE;
		//return 1;
	}

	SIZE[p]++;
	WHOLESIZE++;

	if (p > PRIORITY)
	{
		PRIORITY = p;
	}


}



void get_task_from_WQ(unsigned char* tid, unsigned char* prio) //not running
{
	if (empty())
	{
		printf("waitingQ is empty\n");
		//current_tid = -1;
		//return 0;
	}
	else {
		//printf("deQ -> get_task_from_WQ ->front : %d\n\n", Front);
		*tid = waitingQ[PRIORITY][Front[PRIORITY]].tid;
		*prio = waitingQ[PRIORITY][Front[PRIORITY]].prio;

		waitingQ[PRIORITY][Front[PRIORITY]].tid = -1;
		waitingQ[PRIORITY][Front[PRIORITY]].prio = -1;

		Front[PRIORITY] = (Front[PRIORITY] + 1) % QSIZE;
		P = *prio;

		SIZE[*prio]--;
		WHOLESIZE--;

		PRIORITY = *prio;
		while (!SIZE[PRIORITY] && PRIORITY != 0)
		{
			PRIORITY--;
		}
	}

}