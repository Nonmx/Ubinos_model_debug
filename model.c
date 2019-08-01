/*
model.c has the hook routines function of the model. The module do these procedures.
1)initialize attributes
2)start operating system with startup_process
3)call app() and launch tasks. We assume StartOS is already called.
4)tasks will execute in app(), until ShutDownOS() is called. We assume it is never called.
Author: Yoohee
Date: 08/09/2016
*/
#include "ubinos.h"
#include "kernel.h"
#include "readyQ.h"
#include <assert.h>
#include <stdio.h>
#include "mylib.h"


extern unsigned char e_code = 0;

//local var for ceiling priority
unsigned char current_prio = 0;
signed char current_tid = 0;
int current_pc[NUM_OF_TASKS + 1];

/*functions*/

extern int act_counter[NUM_OF_TASKS + 1];
/*void startup_process(unsigned char s, unsigned char e)
{
	unsigned char i = s;
	int j = 0;
	//unsigned char prio;
	//unsigned char tid;
	while (i <= e)
	{
		if (task_static_info[i].autostart)
		{
			task_dyn_info[i].act_cnt++;
			current_prio = task_static_info[i].prio;
			push_task_into_readyQ(i, current_prio, 0, 0);
			//task_state[i][act_counter[i]] = Ready;
		}
		i++;
	}
	i = 0;


	if (is_empty())
	{
		e_code = TEST;
	}
	else
	{
		get_task_from_readyQ(&current_tid, &current_prio);

		//task_state[current_tid][act_counter[current_tid]] = Running;
		//assert(get_running_task_num() == 1);
	}
}*/

