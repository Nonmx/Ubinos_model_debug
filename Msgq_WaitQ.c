#include "ubinos.h"
#include <stdio.h>
#include <assert.h>


int MSGQ_WQ_FULL(msgq_pt* msgq, unsigned char p)
{
	if (msgq->SIZE[p] == msgq->WHOLESIZE - 1)
		return 1;
	else
		return 0;
}

int MSGQ_WQ_EMPTY(msgq_pt* msgq)
{
	if (msgq->WHOLESIZE == 0)
		return 1;
	else
		return 0;
}


/*int Find_Btask(int* pri_loc, int* task_loc, msgq_pt* sem) // 
{
	int i = 1;
	int j = 0;
	for (i = 1; i < MAX_PRIORITY; i++)
	{
		for (j = 0; j < WAITQ_SIZE; j++)
		{
			if (sem->mutexQ[i][j].timed_flag > 0)
			{
				*pri_loc = i;
				*task_loc = j;
				return 1;//finded
			}
		}
	}
	return 0;//not find
}*/


void push_msgq_task_into_WQ(unsigned char tid, unsigned char p, msgq_pt* msgq)
{
	int temp;
	if (MUTEX_WQ_FULL(msgq, p))
	{
		printf("waittingQ is full!\n");
		//return 0;
	}
	else
	{
		//printf("enQ -> rear: %d\n\n", Rear);
		task_state[tid] = Blocked;
		//printf("task_state[tid][act_counter[tid]] = %d \n", task_state[tid]);
		temp = msgq->Rear[p];
		msgq->mutexQ[p][temp].tid = tid;
		msgq->mutexQ[p][temp].prio = p;
	}

	msgq->Rear[p] = (WAITQ_SIZE + temp + 1) % WAITQ_SIZE;
	//return 1;


	msgq->SIZE[p]++;
	msgq->WHOLESIZE++;

	if (p > msgq->PRIORITY)
	{
		msgq->PRIORITY = p;
	}

}

int temp_Rear;


void get_msgq_task_from_WQ(unsigned char* tid, unsigned char* prio, msgq_pt* msgq)
{
	if (MUTEX_WQ_EMPTY(msgq))
	{
		printf("waitingQ is empty\n");
		//current_tid = -1;
		//return 0;
	}
	else {
		//printf("deQ -> get_task_from_WQ ->front : %d\n\n", Front);
		*tid = msgq->mutexQ[msgq->PRIORITY][msgq->Front[msgq->PRIORITY]].tid;
		*prio = msgq->mutexQ[msgq->PRIORITY][msgq->Front[msgq->PRIORITY]].prio;

		msgq->mutexQ[msgq->PRIORITY][msgq->Front[msgq->PRIORITY]].tid = -1;
		msgq->mutexQ[msgq->PRIORITY][msgq->Front[msgq->PRIORITY]].prio = -1;

		msgq->Front[msgq->PRIORITY] = (msgq->Front[msgq->PRIORITY] + 1) % WAITQ_SIZE;

		msgq->SIZE[*prio]--;
		msgq->WHOLESIZE--;

		msgq->PRIORITY = *prio;
		while (!(msgq->SIZE[msgq->PRIORITY]) && msgq->PRIORITY != 0)
		{
			msgq->PRIORITY--;
		}
	}
}

/*void get_task_from_WQ_position(unsigned char* tid, unsigned char* prio, msgq_pt* sem, int pri_loc, int task_loc) //use for lock timed
{
	if (MUTEX_WQ_EMPTY(sem))
	{
		printf("waitingQ is empty\n");
		//current_tid = -1;
		//return 0;
	}
	else {
		//printf("deQ -> get_task_from_WQ ->front : %d\n\n", Front);
		*tid = sem->mutexQ[pri_loc][task_loc].tid;
		*prio = sem->mutexQ[pri_loc][task_loc].prio;
		assert(sem->mutexQ[pri_loc][task_loc].timed_flag > 0);
		sem->mutexQ[pri_loc][task_loc].timed_flag--;


		sem->mutexQ[pri_loc][task_loc].tid = -1;
		sem->mutexQ[pri_loc][task_loc].prio = -1;
		//
		temp_Rear = sem->Rear[pri_loc];

		if (sem->Front[pri_loc] == task_loc)
		{
			sem->Front[pri_loc] = (sem->Front[pri_loc] + 1) % WAITQ_SIZE;
		}

		else if (task_loc < temp_Rear)//task가 waitQ의 중간에 꺼내면 이 task가 뒤에 있는 task들이 다 앞으로 다시 push 해야 한다.
		{
			if (task_loc + 1 == temp_Rear)
			{
				sem->Rear[pri_loc]--;
			}
			else if (task_loc + 1 < temp_Rear)
			{
				if (temp_Rear == 0)
					temp_Rear = WAITQ_SIZE;//temp_Rear는 0이면 1를 빼면 lowerbound 가능성이 있으니까 temp_Rear= 15로 하고 1빼면 14이기 때문에 0 칸 앞에 것은 딱 맞아.
				while ((temp_Rear - 1) != sem->Rear[pri_loc]) { //모든 task가 다 1칸 앞에서 push 하면 최종적으로 Rear는 1만큼을 감소한다.
					sem->Rear[pri_loc] = task_loc;
					task_loc++;
					push_task_into_WQ(sem->mutexQ[pri_loc][task_loc].tid, task_dyn_info[sem->mutexQ[pri_loc][task_loc].tid].dyn_prio, sem);
				}
			}
		}

		sem->SIZE[*prio]--;
		sem->WHOLESIZE--;

		sem->PRIORITY = *prio;
		while (!(sem->SIZE[sem->PRIORITY]) && sem->PRIORITY != 0)
		{
			sem->PRIORITY--;
		}
	}

}*/


