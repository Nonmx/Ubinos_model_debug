/*
mylib.h contains prototypes of functions which is not included in OS model,
but used to make assertions.
Author: Yoohee
Date: 08/09/2016
*/
//#include <assert.h>
#include "readyQ.h"
//#include "waitingQ.h"
#ifndef MY_LIB_H_
#define MY_LIB_H_


static __inline char is_idle()
{
	char a = 0;
	int i;
	for (i = 0; i < NUM_OF_TASKS; i++)
	{
		if (task_state[i] == Running)
			a++;
	}
	return a > 0 ? 0 : 1;
}

static __inline char is_sleeping()
{
	char a = 0;
	int i;
	for (i = 0; i < NUM_OF_TASKS; i++)
	{
		if(Sleep_Q[i].time > 0)
			a++;
	}
	return a > 0 ? 1 : 0;
}

static __inline char Hava_to_RR()
{
	char a = 0;
	int i = 0;

	if (task_dyn_info[readyQ[current_prio][front[current_prio]].tid].dyn_prio == current_prio)
		return 1;
	else
		return 0;

}

static __inline int find_task_readyQ(unsigned char tid,unsigned char prio)//find the task that locks the mutex
{
	for (int i = 0; i < MAX_QUEUE_LENGTH; i++)
	{
		if (readyQ[prio][i].tid == tid)
			return i;
	}

	return -1;//not found
}

static __inline int find_task_mutexQ(unsigned char tid, Mutex* mutex, mutex_pt mid)
{
	
	for (int i = 0; i < NUM_OF_TASKS+1; i++)
	{
		if (mutex[mid].mutexQ[i].tid == tid)
			return i;
	}
	

	return -1;//not found
}

static __inline int find_task_semQ(unsigned char tid, Sem* sem,sem_pt sid)
{
	
	for (int i = 0; i < NUM_OF_TASKS+1; i++)
	{
		if (sem[sid].semQ[i].tid == tid)
			return i;
	}
	

	return -1;//not found
}

static __inline int find_task_sleepQ(unsigned char tid)
{
	for (int i = 0; i < NUM_OF_TASKS+1; i++)
	{
		if (Sleep_Q[i].tid== tid)
			return i;
	}

	return -1;//not found
}

static __inline int find_task_msgqWQ(unsigned char tid, Msgq* msgq, msgq_pt msid)
{
	
	for (int i = 0; i < NUM_OF_TASKS+1; i++)
	{
		if (msgq[msid].msgqQ[i].tid == tid)
		return i;
	}

}

/*static __inline int Find_Btask(int *pri_loc, int *task_loc, OWNER owner)
{
	int i = 1;
	int j = 0;
	for (i = 1; i < MAX_PRIORITY; i++)
	{
		for (j = 0; j < MAX_QUEUE_LENGTH; j++)
		{
			if (waitingQ[i][j].owner = owner)
			{
				*pri_loc = i;
				*task_loc = j;
				return 1;//finded
			}
		}
	}
	return 0;//not find
}*/

/*static __inline int get_running_task_num() {
	int i = 0;
	int sum = 0;
	for (i = 0; i < NUM_OF_TASKS; i++) {
		if (task_state[i] == Running)
			sum++;
	}

	return sum;
}

static __inline int is_switching()
{
	if (is_idle())
	{
		if (!is_empty())
		{
			get_task_from_readyQ(&current_tid, &current_prio);
		}
		else {
			current_tid = 0; //idle tid
			return 0; //means it should be idle.
		}
	}
	return 1;
}*/


typedef struct {
	API api_kind;
	unsigned char resid;
	unsigned char eventid;
	unsigned char reftask;
}ApiInfo;
extern ApiInfo tinfo[NUM_OF_TASKS][10];

#endif
