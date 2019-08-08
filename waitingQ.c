#include "waitingQ.h"
#include "ubinos.h"
#include <stdio.h>
#include <assert.h>







int act_counter[NUM_OF_TASKS + 1];
int P;
int temp;

 int Find_Btask(int *pri_loc, int *task_loc, OWNER owner)
{
	int i = 1;
	int j = 0;
	for (i = 1; i < MAX_PRIORITY; i++)
	{
		for (j = 0; j < QSIZE; j++)
		{
			if (waitingQ[i][j].owner == owner)
			{
				*pri_loc = i;
				*task_loc = j;
				return 1;//finded
			}
		}
	}
	return 0;//not find
}


void push_task_into_WQ(unsigned char tid, unsigned char p,OWNER owner)
{
	if (full(p))
	{
		printf("waittingQ is full!\n");
		//return 0;
	}
	else
	{
		//printf("enQ -> rear: %d\n\n", Rear);
		task_state[tid] = Blocked;
		//printf("task_state[tid][act_counter[tid]] = %d \n", task_state[tid]);
		temp = Rear[p];
		waitingQ[p][temp].tid = tid;
		waitingQ[p][temp].prio = p;
		waitingQ[p][temp].owner = owner;
		Rear[p] = (QSIZE + temp + 1) % QSIZE;
		//return 1;
	}

	SIZE[p]++;
	WHOLESIZE++;

	if (p > PRIORITY)
	{
		PRIORITY = p;
	}


}

int temp_Rear;

void get_task_from_WQ(unsigned char* tid, unsigned char* prio,OWNER owner, int pri_loc, int task_loc) //need more detail
{
	if (empty())
	{
		printf("waitingQ is empty\n");
		//current_tid = -1;
		//return 0;
	}
	else {
		//printf("deQ -> get_task_from_WQ ->front : %d\n\n", Front);
		*tid = waitingQ[pri_loc][task_loc].tid;
		*prio = waitingQ[pri_loc][task_loc].prio;
		assert(waitingQ[pri_loc][task_loc].owner == owner);


		waitingQ[pri_loc][task_loc].tid = -1;
		waitingQ[pri_loc][task_loc].prio = -1;
		//
		temp_Rear = Rear[pri_loc];

		if (Front[pri_loc] == task_loc)
		{
			Front[pri_loc ] = (Front[pri_loc] + 1) % QSIZE;
		}
		else if (task_loc < temp_Rear)//task가 waitQ의 중간에 꺼내면 이 task가 뒤에 있는 task들이 다 앞으로 다시 push 해야 한다.
		{
			if (temp_Rear == 0)
				temp_Rear = QSIZE; //temp_Rear는 0이면 1를 빼면 lowerbound 가능성이 있으니까 temp_Rear= 15로 하고 1빼면 14이기 때문에 0 칸 앞에 것은 딱 맞아.
			while ((temp_Rear -1 )!= Rear[pri_loc]) { //모든 task가 다 1칸 앞에서 push 하면 최종적으로 Rear는 1만큼을 감소한다.
				Rear[pri_loc] = task_loc;
				task_loc++;
				push_task_into_WQ(waitingQ[pri_loc][task_loc].tid, task_dyn_info[waitingQ[pri_loc][task_loc].tid].dyn_prio, owner);
			}
		}

		SIZE[*prio]--;
		WHOLESIZE--;

		PRIORITY = *prio;
		while (!SIZE[PRIORITY] && PRIORITY != 0)
		{
			PRIORITY--;
		}
	}

}