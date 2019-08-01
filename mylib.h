/*
mylib.h contains prototypes of functions which is not included in OS model,
but used to make assertions.
Author: Yoohee
Date: 08/09/2016
*/
//#include <assert.h>
#include "readyQ.h"
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
		if (task_state[i] == Blocked)
			a++;
	}
	return a > 0 ? 1 : 0;
}

static __inline char Hava_to_RR()// readyQ???? ???????? task?? ????? priority????? check
{
	char a = 0;
	int i = 0;
	//for (i = 0; i < MAX_QUEUE_LENGTH; i++)

	if (task_static_info[readyQ[current_prio][front[current_prio]].tid].prio == current_prio)
		return 1;
	else
		return 0;

}

/*static __inline int task_position(unsigned char tid,unsigned char prio)
{
	for (int i = 0; i < MAX_QUEUE_LENGTH; i++)
	{
		if (readyQ[prio][i].tid == tid)
			return i;
	}
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
