#include "Sem_WaitQ.h"
//#include "ubinos.h"
#include <stdio.h>
#include <assert.h>


int SEM_WQ_FULL(sem_pt sid, Sem* sem)
{
	if ((sem[sid].Rear + 1) % WAITQ_SIZE  == sem[sid].Front)
		return 1;
	else
		return 0;

}

int SEM_WQ_EMPTY(sem_pt sid, Sem* sem)
{
	if (sem[sid].Front == sem[sid].Rear)
		return 1;
	else
		return 0;
}

int Find_sem_Btask(int* task_loc, int tid, sem_pt sid, Sem* sem) //use for mutex lock_timed 
{
	int j = 0;

	for (j = 0; j < WAITQ_SIZE; j++)
	{
		if (sem[sid].semQ[j].tid == tid)
		{
			*task_loc = j;
			return 0;//finded
		}
	}

	return -1;//not find
}

void semQ_sort(sem_pt sid, Sem* sem)
{
	unsigned char temp_tid;
	unsigned char temp_prio;
	int i = 0;
	int j = 0;

	for (i = 0; i < (sem[sid].Rear - sem[sid].Front + WAITQ_SIZE)%WAITQ_SIZE; i++)
	{
		int tp_front = sem[sid].Front;
		for (j = 0; j < (sem[sid].Rear - sem[sid].Front + WAITQ_SIZE)%WAITQ_SIZE-i; j++)
		{
			if (task_dyn_info[sem[sid].semQ[tp_front].tid].dyn_prio < task_dyn_info[sem[sid].semQ[tp_front+1].tid].dyn_prio)
			{
				temp_tid = sem[sid].semQ[tp_front+1].tid;
				//temp_prio = sem[sid].semQ[tp_front+1].prio;

				sem[sid].semQ[tp_front].tid = sem[sid].semQ[tp_front+1].tid;
				//sem[sid].semQ[tp_front].prio = sem[sid].semQ[tp_front+1].prio;

				sem[sid].semQ[tp_front+1].tid = temp_tid;
				//sem[sid].semQ[tp_front+1]. = temp_tid;
			}
			tp_front++;
		}
	}
}

int push_sem_task_into_WQ(unsigned char tid, unsigned char p, sem_pt sid, Sem* sem)
{
	task_state[tid] = Blocked;

	if (SEM_WQ_FULL(sid, sem))
	{
		printf("sem_waittingQ is full!\n");
		return -1;
	}
	else
	{
		//printf("enQ -> rear: %d\n\n", Rear);
		task_state[tid] = Blocked;
		//printf("task_state[tid][act_counter[tid]] = %d \n", task_state[tid]);
		sem[sid].semQ[sem[sid].Rear].tid = tid;
		task_dyn_info[sem[sid].semQ[sem[sid].Rear].tid].dyn_prio = p;

		sem[sid].Rear = (WAITQ_SIZE + 1 + sem[sid].Rear) % WAITQ_SIZE;
		if ((sem[sid].Rear- sem[sid].Front+WAITQ_SIZE )% WAITQ_SIZE > 1)//More than one element, sorting
		{
			semQ_sort(sid, sem);
		}
		return 0;
	}


	


}

int temp_Rear;


int get_sem_task_from_WQ(unsigned char* tid, unsigned char* prio, sem_pt sid, Sem* sem)
{
	if (SEM_WQ_EMPTY(sid, sem))
	{
		printf("sem_waitQ is empty\n");
		return -1;
	}

	*tid = sem[sid].semQ[sem[sid].Front].tid;
	*prio = task_dyn_info[sem[sid].semQ[sem[sid].Front].tid].dyn_prio;

	sem[sid].Front = (sem[sid].Front + 1) % WAITQ_SIZE;
	return 0;

}

int sem_prio_change(unsigned char tid, unsigned char chan_prio,sem_pt sid, Sem *sem, int loc)
{
	if (sem[sid].semQ[loc].tid == tid) {

		task_dyn_info[tid].dyn_prio = chan_prio;
		semQ_sort(sid,sem);
		return 0;
	}
	else
		return -1;
}

void get_sem_task_from_WQ_position(unsigned char* tid, unsigned char* prio, sem_pt sid, Sem* sem, int task_loc) //use for lock timed
{
	if (SEM_WQ_EMPTY(sid,sem))
	{
		printf("waitingQ is empty\n");
		//current_tid = -1;
		//return 0;
	}
	else {
		//printf("deQ -> get_task_from_WQ ->front : %d\n\n", Front);
		*tid = sem[sid].semQ[task_loc].tid;
		*prio = task_dyn_info[sem[sid].semQ[sem[sid].Front].tid].dyn_prio;



		sem[sid].semQ[task_loc].tid = 0; //
		//sem[sid].semQ[task_loc].prio = 0;
		//

		if (sem[sid].Front == task_loc)
		{
			sem[sid].Front = (sem[sid].Front + 1) % WAITQ_SIZE;
		}
		else if ((sem[sid].Rear+WAITQ_SIZE)%WAITQ_SIZE  == 0)
		{
			semQ_sort(sid, sem);
			sem[sid].Rear = WAITQ_SIZE-1;
		}
		else
		{
			semQ_sort(sid, sem);
			sem[sid].Rear--;
		}

	}

}



