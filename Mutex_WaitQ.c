#include "Mutex_WaitQ.h"
#include "ubinos.h"
#include <stdio.h>
#include <assert.h>


int MUTEX_WQ_FULL(mutex_pt mid,Mutex * mutex)
{
	if ((mutex[mid].Rear + 1) % WAITQ_SIZE  == mutex[mid].Front)
		return 1;
	else
		return 0;

}

int MUTEX_WQ_EMPTY(mutex_pt mid, Mutex* mutex)
{
	if (mutex[mid].Front == mutex[mid].Rear)
		return 1;
	else
		return 0;
}


int Find_mutex_Btask(int *task_loc, int tid,mutex_pt mid, Mutex* mutex) //use for mutex lock_timed 
{
	int j = 0;

	for (j = 0; j < WAITQ_SIZE; j++)
	{
		if (mutex[mid].mutexQ[j].tid == tid)
		{
			*task_loc = j;
			return 0;//finded
		}
	}
	
	return -1;//not find
}

void mutexQ_sort(mutex_pt mid, Mutex * mutex)
{
	unsigned char temp_tid;
	unsigned char temp_prio;
	int i = 0;
	int j = 0;
	for (i = mutex[mid].Front; i < mutex[mid].Rear; i++)
	{
		for (j = mutex[mid].Front; j < mutex[mid].Rear-i; j++)
		{
			if (task_dyn_info[mutex[mid].mutexQ[i].tid].dyn_prio < task_dyn_info[mutex[mid].mutexQ[i++].tid].dyn_prio)
			{
				temp_tid = mutex[mid].mutexQ[i].tid;
				temp_prio = mutex[mid].mutexQ[i].prio;

				mutex[mid].mutexQ[i].tid = mutex[mid].mutexQ[i++].tid;
				mutex[mid].mutexQ[i].prio = mutex[mid].mutexQ[i++].prio;

				mutex[mid].mutexQ[i++].tid = temp_tid;
				mutex[mid].mutexQ[i++].tid = temp_tid;
			}
		}
	}
}

int push_mutex_task_into_WQ(unsigned char tid, unsigned char p, mutex_pt mid,Mutex* mutex)
{

	task_state[tid] = Blocked;

	if (MUTEX_WQ_FULL(mid,mutex))
	{
		printf(" mutex waittingQ is full!\n");
		return -1;
	}
	else
	{
		//printf("enQ -> rear: %d\n\n", Rear);
		task_state[tid] = Blocked;
		//printf("task_state[tid][act_counter[tid]] = %d \n", task_state[tid]);
		mutex[mid].mutexQ[mutex[mid].Rear].tid = tid;
		mutex[mid].mutexQ[mutex[mid].Rear].prio = p;

		mutex[mid].Rear = (WAITQ_SIZE + 1 + mutex[mid].Rear) % WAITQ_SIZE;

		if ((mutex[mid].Rear - mutex[mid].Front+WAITQ_SIZE)%WAITQ_SIZE > 1)//More than one element, sorting
		{
			mutexQ_sort(mid,mutex);
		}

		//if (mutex->timed_flag > 0)
			//mutex->mutexQ[mutex->Rear].timed_flag++;
		return 0;
			
	}


		
	

}

int temp_Rear;


int get_mutex_task_from_WQ(unsigned char* tid, unsigned char* prio, mutex_pt mid, Mutex * mutex)
{
	if (MUTEX_WQ_EMPTY(mid,mutex))
	{
		printf("mutex_waitQ is empty\n");
		return -1;
	}

	*tid = mutex[mid].mutexQ[mutex[mid].Front].tid;
	*prio = mutex[mid].mutexQ[mutex[mid].Front].prio;

	mutex[mid].Front = (mutex[mid].Front + 1) % WAITQ_SIZE;
	return 0;

}

int mutex_prio_change(unsigned char tid, unsigned char chan_prio, mutex_pt mid, Mutex* mutex, int loc)
{
	if (mutex[mid].mutexQ[loc].tid == tid) {
		mutex[mid].mutexQ[loc].prio = chan_prio;
		mutexQ_sort(mid, mutex);
		return 0;
	}
	else
		return -1;
}

void get_mutex_task_from_WQ_position(unsigned char* tid, unsigned char* prio,mutex_pt mid, Mutex* mutex,int task_loc) //use for lock timed
{
	if (MUTEX_WQ_EMPTY(mid,mutex))
	{
		printf("waitingQ is empty\n");
		//current_tid = -1;
		//return 0;
	}
	else {
		//printf("deQ -> get_task_from_WQ ->front : %d\n\n", Front);
		*tid = mutex[mid].mutexQ[task_loc].tid;
		*prio = mutex[mid].mutexQ[task_loc].prio;
		//assert(mutex->mutexQ[pri_loc][task_loc].timed_flag > 0);
		//mutex->mutexQ[pri_loc][task_loc].timed_flag--;


		mutex[mid].mutexQ[task_loc].tid = 0; //
		mutex[mid].mutexQ[task_loc].prio = 0;
		//


		if (mutex[mid].Front == task_loc)
		{
			mutex[mid].Front = (mutex[mid].Front + 1) % WAITQ_SIZE;
		}
		else if ((mutex[mid].Rear+WAITQ_SIZE)%WAITQ_SIZE == 0)
		{
			mutexQ_sort(mid, mutex);
			mutex[mid].Rear = WAITQ_SIZE;
		}
		else
		{
			mutexQ_sort(mid, mutex);
			mutex[mid].Rear--;
		}

	}

}


