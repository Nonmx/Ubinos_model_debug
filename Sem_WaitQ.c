#include "Sem_WaitQ.h"
//#include "ubinos.h"
#include <stdio.h>
#include <assert.h>

extern Sem* sem_list;

int SEM_WQ_FULL(sem_pt sid)
{
	if ((sem_list[sid].Rear + 1) % (NUM_OF_TASKS+1)  == sem_list[sid].Front)
		return 1;
	else
		return 0;

}

int SEM_WQ_EMPTY(sem_pt sid)
{
	if (sem_list[sid].Front == sem_list[sid].Rear)
		return 1;
	else
		return 0;
}

/*int Find_sem_Btask(int* task_loc, int tid, sem_pt sid) //use for mutex lock_timed 
{
	int j = 0;

	for (j = 0; j < (NUM_OF_TASKS+1); j++)
	{
		if (sem_list[sid].semQ[j].tid == tid)
		{
			*task_loc = j;
			return 0;//finded
		}
	}

	return -1;//not find
}*/

void semQ_sort(sem_pt sid)
{
	unsigned char temp_tid;
	//unsigned char temp_prio;
	int i = 0;
	int j = 0;

	for (i = 0; i < (sem_list[sid].Rear - sem_list[sid].Front + (NUM_OF_TASKS+1))%(NUM_OF_TASKS+1); i++)
	{
		int tp_front = sem_list[sid].Front;
		for (j = 0; j < (sem_list[sid].Rear - sem_list[sid].Front + (NUM_OF_TASKS+1))%(NUM_OF_TASKS+1)-i; j++)
		{
			//assert(sem_list[sid].semQ[tp_front + 1].tid < 10);
			if (task_dyn_info[sem_list[sid].semQ[tp_front].tid].dyn_prio < task_dyn_info[sem_list[sid].semQ[(tp_front+1)%(NUM_OF_TASKS+1)].tid].dyn_prio)
			{
				
				temp_tid = sem_list[sid].semQ[tp_front].tid;
				//temp_prio = sem[sid].semQ[tp_front+1].prio;

				sem_list[sid].semQ[tp_front].tid = sem_list[sid].semQ[(tp_front+1)%(NUM_OF_TASKS+1)].tid;
				//sem[sid].semQ[tp_front].prio = sem[sid].semQ[tp_front+1].prio;

				sem_list[sid].semQ[(tp_front+1)%(NUM_OF_TASKS+1)].tid = temp_tid;
				//sem[sid].semQ[tp_front+1]. = temp_tid;
			}
			tp_front = (tp_front + 1) % (NUM_OF_TASKS+1);
		}
	}
}


int push_sem_task_into_WQ(unsigned char tid, unsigned char p, sem_pt sid)
{
	task_state[tid] = Blocked;

	if (SEM_WQ_FULL(sid))
	{
		printf("sem_waittingQ is full!\n");
		return -1;
	}
	else
	{
		//printf("enQ -> rear: %d\n\n", Rear);
		task_state[tid] = Blocked;
		//printf("task_state[tid][act_counter[tid]] = %d \n", task_state[tid]);
		sem_list[sid].semQ[sem_list[sid].Rear].tid = tid;
		task_dyn_info[sem_list[sid].semQ[sem_list[sid].Rear].tid].dyn_prio = p;


		sem_list[sid].Rear = (1 + sem_list[sid].Rear) % (NUM_OF_TASKS+1);
		if ((sem_list[sid].Rear- sem_list[sid].Front+(NUM_OF_TASKS+1) )% (NUM_OF_TASKS+1) > 1)//More than one element, sorting
		{
		semQ_sort(sid);
		}
		return 0;
	}

	


}

int temp_Rear;


int get_sem_task_from_WQ(unsigned char* tid, unsigned char* prio, sem_pt sid)
{
	if (SEM_WQ_EMPTY(sid))
	{
		printf("sem_waitQ is empty\n");
		return -1;
	}

	*tid = sem_list[sid].semQ[sem_list[sid].Front].tid;
	*prio = task_dyn_info[sem_list[sid].semQ[sem_list[sid].Front].tid].dyn_prio;


	sem_list[sid].semQ[sem_list[sid].Front].tid = 0;
	task_dyn_info[sem_list[sid].semQ[sem_list[sid].Front].tid].dyn_prio = 0;

	sem_list[sid].Front = (sem_list[sid].Front + 1) % (NUM_OF_TASKS+1);
	return 0;

}

int sem_prio_change(unsigned char tid, unsigned char chan_prio,sem_pt sid, int loc)
{
	if (sem_list[sid].semQ[loc].tid == tid) {

		task_dyn_info[tid].dyn_prio = chan_prio;
		semQ_sort(sid);
		return 0;
	}
	else
		return -1;
}

int get_sem_task_from_WQ_position(unsigned char* tid, unsigned char* prio, sem_pt sid, int task_loc) //use for lock timed
{
	if (SEM_WQ_EMPTY(sid))
	{
		printf("waitingQ is empty\n");
		//current_tid = -1;
		return -1;
	}
	else {
		//printf("deQ -> get_task_from_WQ ->front : %d\n\n", Front);
		*tid = sem_list[sid].semQ[task_loc].tid;
		*prio = task_dyn_info[sem_list[sid].semQ[task_loc].tid].dyn_prio;



		sem_list[sid].semQ[task_loc].tid = 0; //
		task_dyn_info[sem_list[sid].semQ[task_loc].tid].dyn_prio = 0;
		//

		if (sem_list[sid].Front == task_loc)
		{
			sem_list[sid].Front = (sem_list[sid].Front + 1) % (NUM_OF_TASKS+1);
		}
		else if ((sem_list[sid].Rear+(NUM_OF_TASKS+1))%(NUM_OF_TASKS+1)  == 0)
		{
			semQ_sort(sid);
			sem_list[sid].Rear = NUM_OF_TASKS;
		}
		else
		{
			semQ_sort(sid);
			sem_list[sid].Rear--;
		}

		return 0;

	}

}



