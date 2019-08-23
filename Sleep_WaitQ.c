#include "Sleep_WaitQ.h"
#include "kernel.h"
#include <stdio.h>
//#include "ubinos.h"


int FRONT = 0;
int REAR = 0;

int Sleep_Empty()
{
	if (FRONT == REAR)
		return 1;//empty
	else
		return 0;//nonempty
}

int Sleep_Full()
{
	if ((REAR + 1) % (NUM_OF_TASKS+1)  == FRONT)
		return 1;
	else
		return 0;
}

void sleepQ_sort()
{
	unsigned char temp_tid;
	int temp_time;
	
	int i = 0;
	int j = 0;
	
	for (i = 0; i < (REAR - FRONT + (NUM_OF_TASKS+1))%(NUM_OF_TASKS+1); i++)
	{
		int tp_front = FRONT;
		for (j = 0; j < ((REAR - FRONT+(NUM_OF_TASKS+1))%(NUM_OF_TASKS+1)) - i; j++)
		{
			
				if (Sleep_Q[tp_front].time > Sleep_Q[(tp_front + 1)%(NUM_OF_TASKS+1)].time && Sleep_Q[(tp_front+1)%(NUM_OF_TASKS+1)].time != -1)
				{
					temp_tid = Sleep_Q[tp_front].tid;
					//temp_prio = Sleep_Q[tp_front].prio;
					temp_time = Sleep_Q[tp_front].time;


					Sleep_Q[tp_front].tid = Sleep_Q[(tp_front + 1)%(NUM_OF_TASKS+1)].tid;
					//Sleep_Q[tp_front].prio = Sleep_Q[tp_front + 1].prio;
					Sleep_Q[tp_front].time = Sleep_Q[(tp_front + 1)%(NUM_OF_TASKS+1)].time;

					Sleep_Q[(tp_front + 1)%(NUM_OF_TASKS+1)].tid = temp_tid;
					//Sleep_Q[tp_front + 1].prio = temp_prio;
					Sleep_Q[(tp_front + 1)%(NUM_OF_TASKS+1)].time = temp_time;
				}
				tp_front = (tp_front + 1) % (NUM_OF_TASKS+1);
		}

	}
}
int push_sleep_task_into_WQ(unsigned char tid, unsigned char prio, int time)
{
	task_state[tid] = Blocked;

	if (Sleep_Full())
	{
		printf("messageQ is full!\n\n");
		return -1;
	}
	else
	{
		Sleep_Q[REAR].tid = tid;
		Sleep_Q[REAR].time = time;

		REAR = (REAR + 1) % (NUM_OF_TASKS+1);

		if ((REAR-FRONT + (NUM_OF_TASKS+1))%(NUM_OF_TASKS+1) > 1) //If the element in the queue is greater than one
		{
			sleepQ_sort();
		}
		return 0;
	}
}


int get_sleep_task_from_WQ(unsigned char* tid, unsigned char* prio)//DeQ is based on time
{
	//	printf("mess is %s \n\n", Message_Queue->messgae);
	if (Sleep_Empty())
	{
		printf("sleepQ is empty\n\n");
		return -1;
	}
	else
	{
		*tid = Sleep_Q[FRONT].tid;
		*prio = task_dyn_info[Sleep_Q[FRONT].tid].dyn_prio;
		Sleep_Q[FRONT].time = -1;


		FRONT = (FRONT + 1) % (NUM_OF_TASKS+1);
		return 0;
	}
}


int sleep_prio_change(unsigned char tid, unsigned char chan_prio,int loc)
{
	if (Sleep_Q[loc].tid == tid)
	{
		task_dyn_info[Sleep_Q[loc].tid].dyn_prio = chan_prio; //timed based
		return 0;
	}
	else
		return -1;
}

/*int get_sleep_task_from_WQ_position(unsigned char* tid, unsigned char* prio, int loc)
{
	if (Sleep_Empty())
	{
		printf("message is empty\n\n");
		return -1;
	}
	else
	{
		*tid = Sleep_Q[loc].tid;
		*prio = task_dyn_info[Sleep_Q[loc].tid].dyn_prio;

		Sleep_Q[loc].tid = 0;
		//Sleep_Q[loc].prio = 0;
		Sleep_Q[loc].time = -1;

		if (FRONT == loc)
		{
			FRONT = (FRONT + 1) % (NUM_OF_TASKS+1);
		}
		else if ((REAR+(NUM_OF_TASKS+1))%(NUM_OF_TASKS+1) == 0)
		{
			sleepQ_sort();
			REAR = NUM_OF_TASKS;
		}
		else
		{
			sleepQ_sort();
			REAR--;
		}
	
	}
}*/

