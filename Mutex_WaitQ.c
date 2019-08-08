#include "Mutex_WaitQ.h"
#include "ubinos.h"
#include <stdio.h>
#include <assert.h>


int MUTEX_WQ_FULL(mutex_pt* mutex, unsigned char p)
{
	if (mutex->SIZE[p] == mutex->WHOLESIZE - 1)
		return 1;
	else
		return 0;

}

int MUTEX_WQ_EMPTY(mutex_pt* mutex)
{
	if (mutex->WHOLESIZE == 0)
		return 1;
	else
		return 0;
}


int Find_mutex_Btask(int *pri_loc, int *task_loc, mutex_pt * mutex) //use for mutex lock_timed 
{
	int i = 1;
	int j = 0;
	for (i = 1; i < MAX_PRIORITY; i++)
	{
		for (j = 0; j < WAITQ_SIZE; j++)
		{
			if (mutex->mutexQ[i][j].timed_flag > 0)
			{
				*pri_loc = i;
				*task_loc = j;
				return 1;//finded
			}
		}
	}
	return 0;//not find
}


void push_mutex_task_into_WQ(unsigned char tid, unsigned char p, mutex_pt * mutex)
{

	int temp;
	if (MUTEX_WQ_FULL(mutex,p))
	{
		printf("waittingQ is full!\n");
		//return 0;
	}
	else
	{
		//printf("enQ -> rear: %d\n\n", Rear);
		task_state[tid] = Blocked;
		//printf("task_state[tid][act_counter[tid]] = %d \n", task_state[tid]);
		temp = mutex->Rear[p];
		mutex->mutexQ[p][temp].tid = tid;
		mutex->mutexQ[p][temp].prio = p;
		if (mutex->timed_flag > 0)
			mutex->mutexQ[p][temp].timed_flag++;
			
	}

	mutex->Rear[p] = (WAITQ_SIZE + temp + 1) % WAITQ_SIZE;
		//return 1;
	

	mutex->SIZE[p]++;
	mutex->WHOLESIZE++;

	if (p > mutex-> PRIORITY)
	{
		mutex->PRIORITY = p;
	}

}

int temp_Rear;


void get_mutex_task_from_WQ(unsigned char* tid, unsigned char* prio, mutex_pt* mutex)
{
	if (MUTEX_WQ_EMPTY(mutex))
	{
		printf("waitingQ is empty\n");
		//current_tid = -1;
		//return 0;
	}
	else {
		//printf("deQ -> get_task_from_WQ ->front : %d\n\n", Front);
		*tid = mutex->mutexQ[mutex->PRIORITY][mutex->Front[mutex->PRIORITY]].tid;
		*prio = mutex->mutexQ[mutex->PRIORITY][mutex->Front[mutex->PRIORITY]].prio;

		mutex->mutexQ[mutex->PRIORITY][mutex->Front[mutex->PRIORITY]].tid = -1;
		mutex->mutexQ[mutex->PRIORITY][mutex->Front[mutex->PRIORITY]].prio = -1;

		mutex->Front[mutex->PRIORITY] = (mutex->Front[mutex->PRIORITY] + 1) % WAITQ_SIZE;

		mutex->SIZE[*prio]--;
		mutex->WHOLESIZE--;

		mutex->PRIORITY = *prio;
		while (!(mutex->SIZE[mutex->PRIORITY]) && mutex->PRIORITY != 0)
		{
			mutex->PRIORITY--;
		}
	}
}

void get_mutex_task_from_WQ_position(unsigned char* tid, unsigned char* prio,mutex_pt* mutex,int pri_loc, int task_loc) //use for lock timed
{
	if (MUTEX_WQ_EMPTY(mutex))
	{
		printf("waitingQ is empty\n");
		//current_tid = -1;
		//return 0;
	}
	else {
		//printf("deQ -> get_task_from_WQ ->front : %d\n\n", Front);
		*tid = mutex->mutexQ[pri_loc][task_loc].tid;
		*prio = mutex->mutexQ[pri_loc][task_loc].prio;
		assert(mutex->mutexQ[pri_loc][task_loc].timed_flag > 0);
		mutex->mutexQ[pri_loc][task_loc].timed_flag--;


		mutex->mutexQ[pri_loc][task_loc].tid = -1;
		mutex->mutexQ[pri_loc][task_loc].prio = -1;
		//
		temp_Rear = mutex->Rear[pri_loc];

		if (mutex->Front[pri_loc] == task_loc)
		{
			mutex->Front[pri_loc ] = (mutex->Front[pri_loc] + 1) % WAITQ_SIZE;
		}
	
		else if (task_loc< temp_Rear)//task�� waitQ�� �߰��� ������ �� task�� �ڿ� �ִ� task���� �� ������ �ٽ� push �ؾ� �Ѵ�.
		{
			if (task_loc + 1 == temp_Rear)
			{
				mutex->Rear[pri_loc]--;
			}
			else if(task_loc +1 < temp_Rear)
			{
				if (temp_Rear == 0)
					temp_Rear = WAITQ_SIZE;//temp_Rear�� 0�̸� 1�� ���� lowerbound ���ɼ��� �����ϱ� temp_Rear= 15�� �ϰ� 1���� 14�̱� ������ 0 ĭ �տ� ���� �� �¾�.
				while ((temp_Rear - 1) != mutex-> Rear[pri_loc]) { //��� task�� �� 1ĭ �տ��� push �ϸ� ���������� Rear�� 1��ŭ�� �����Ѵ�.
					mutex-> Rear[pri_loc] = task_loc;
					task_loc++;
					push_mutex_task_into_WQ(mutex->mutexQ[pri_loc][task_loc].tid, task_dyn_info[mutex->mutexQ[pri_loc][task_loc].tid].dyn_prio, mutex);
				}
			}
		}

		mutex->SIZE[*prio]--;
		mutex->WHOLESIZE--;

		mutex->PRIORITY = *prio;
		while (!(mutex->SIZE[mutex->PRIORITY]) && mutex->PRIORITY != 0)
		{
			mutex-> PRIORITY--;
		}
	}

}


