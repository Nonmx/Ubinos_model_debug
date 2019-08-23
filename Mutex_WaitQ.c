#include "Mutex_WaitQ.h"
#include "ubinos.h"
#include <stdio.h>
#include <assert.h>


extern Mutex* mutex_list;

int MUTEX_WQ_FULL(mutex_pt mid)
{
	if ((mutex_list[mid].Rear + 1) % NUM_OF_TASKS  == mutex_list[mid].Front)
		return 1;
	else
		return 0;

}

int MUTEX_WQ_EMPTY(mutex_pt mid)
{
	if (mutex_list[mid].Front == mutex_list[mid].Rear)
		return 1;
	else
		return 0;
}


/*int Find_mutex_Btask(int *task_loc, int tid,mutex_pt mid) //use for mutex lock_timed 
{
	int j = 0;

	for (j = 0; j < NUM_OF_TASKS; j++)
	{
		if (mutex_list[mid].mutexQ[j].tid == tid)
		{
			*task_loc = j;
			return 0;//finded
		}
	}
	
	return -1;//not find
}*/

void mutexQ_sort(mutex_pt mid)
{
	unsigned char temp_tid;
	int i = 0;
	int j = 0;
	for (i = 0; i < (mutex_list[mid].Rear - mutex_list[mid].Front + (NUM_OF_TASKS+1))%(NUM_OF_TASKS+1); i++)
	{
		int tp_front = mutex_list[mid].Front;
		for (j = 0; j < (mutex_list[mid].Rear - mutex_list[mid].Front + (NUM_OF_TASKS+1)) % (NUM_OF_TASKS+1) - i; j++)
		{
			//assert(mutex_list[mid].mutexQ[tp_front + 1].tid < 10);
			if (task_dyn_info[mutex_list[mid].mutexQ[tp_front].tid].dyn_prio < task_dyn_info[mutex_list[mid].mutexQ[(tp_front+1)%(NUM_OF_TASKS+1)].tid].dyn_prio)
			{
				temp_tid = mutex_list[mid].mutexQ[tp_front].tid;
				//temp_prio = mutex[mid].mutexQ[j+1].prio;

				mutex_list[mid].mutexQ[tp_front].tid = mutex_list[mid].mutexQ[(tp_front+1)%(NUM_OF_TASKS+1)].tid;
				//mutex[mid].mutexQ[i].prio = mutex[mid].mutexQ[j+1].prio;

				mutex_list[mid].mutexQ[(tp_front+1)%(NUM_OF_TASKS+1)].tid = temp_tid;
				//mutex[mid].mutexQ[j+1].tid = temp_tid;
			}
			tp_front = (tp_front+1)%(NUM_OF_TASKS+1);
		}
	}
}

int push_mutex_task_into_WQ(unsigned char tid, unsigned char p, mutex_pt mid)
{

	task_state[tid] = Blocked;

	if (MUTEX_WQ_FULL(mid))
	{
		printf(" mutex waittingQ is full!\n");
		return -1;
	}
	else
	{
		//printf("enQ -> rear: %d\n\n", Rear);
		task_state[tid] = Blocked;
		//printf("task_state[tid][act_counter[tid]] = %d \n", task_state[tid]);
		mutex_list[mid].mutexQ[mutex_list[mid].Rear].tid = tid;
		task_dyn_info[mutex_list[mid].mutexQ[mutex_list[mid].Rear].tid].dyn_prio = p;
		//mutex[mid].mutexQ[mutex[mid].Rear].prio = p;

		mutex_list[mid].Rear = (1 + mutex_list[mid].Rear) % (NUM_OF_TASKS+1);

		if ((mutex_list[mid].Rear - mutex_list[mid].Front+(NUM_OF_TASKS+1))%(NUM_OF_TASKS+1) > 1)//More than one element, sorting
		{
			mutexQ_sort(mid);
		}

		//if (mutex->timed_flag > 0)
			//mutex->mutexQ[mutex->Rear].timed_flag++;
		return 0;
			
	}


		
	

}

int temp_Rear;


int get_mutex_task_from_WQ(unsigned char* tid, unsigned char* prio, mutex_pt mid)
{
	if (MUTEX_WQ_EMPTY(mid))
	{
		printf("mutex_waitQ is empty\n");
		return -1;
	}

	*tid = mutex_list[mid].mutexQ[mutex_list[mid].Front].tid;
	*prio = task_dyn_info[mutex_list[mid].mutexQ[mutex_list[mid].Front].tid].dyn_prio;

	mutex_list[mid].mutexQ[mutex_list[mid].Front].tid = 0;
	task_dyn_info[mutex_list[mid].mutexQ[mutex_list[mid].Front].tid].dyn_prio = 0;


	mutex_list[mid].Front = (mutex_list[mid].Front + 1) % (NUM_OF_TASKS+1);
	return 0;

}

int mutex_prio_change(unsigned char tid, unsigned char chan_prio, mutex_pt mid, int loc)
{
	if (mutex_list[mid].mutexQ[loc].tid == tid) {

		task_dyn_info[mutex_list[mid].mutexQ[loc].tid].dyn_prio = chan_prio;
		mutexQ_sort(mid);
		return 0;
	}
	else
		return -1;
}

int get_mutex_task_from_WQ_position(unsigned char* tid, unsigned char* prio,mutex_pt mid,int task_loc) //use for lock timed
{
	if (MUTEX_WQ_EMPTY(mid))
	{
		printf("waitingQ is empty\n");
		//current_tid = -1;
		return -1;
	}
	else {
		//printf("deQ -> get_task_from_WQ ->front : %d\n\n", Front);
		*tid = mutex_list[mid].mutexQ[task_loc].tid;
		*prio = task_dyn_info[mutex_list[mid].mutexQ[task_loc].tid].dyn_prio;
		//assert(mutex->mutexQ[pri_loc][task_loc].timed_flag > 0);
		//mutex->mutexQ[pri_loc][task_loc].timed_flag--;


		mutex_list[mid].mutexQ[task_loc].tid = 0; //
		task_dyn_info[mutex_list[mid].mutexQ[task_loc].tid].dyn_prio = 0;
		//


		if (mutex_list[mid].Front == task_loc)
		{
			mutex_list[mid].Front = (mutex_list[mid].Front + 1) % (NUM_OF_TASKS+1);
		}
		else if ((mutex_list[mid].Rear+(NUM_OF_TASKS+1))%(NUM_OF_TASKS+1) == 0)
		{
			mutexQ_sort(mid);
			mutex_list[mid].Rear = NUM_OF_TASKS;
		}
		else
		{
			mutexQ_sort(mid);
			mutex_list[mid].Rear--;
		}

	}

}


