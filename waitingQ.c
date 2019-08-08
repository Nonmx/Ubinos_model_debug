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
		else if (task_loc < temp_Rear)//task�� waitQ�� �߰��� ������ �� task�� �ڿ� �ִ� task���� �� ������ �ٽ� push �ؾ� �Ѵ�.
		{
			if (temp_Rear == 0)
				temp_Rear = QSIZE; //temp_Rear�� 0�̸� 1�� ���� lowerbound ���ɼ��� �����ϱ� temp_Rear= 15�� �ϰ� 1���� 14�̱� ������ 0 ĭ �տ� ���� �� �¾�.
			while ((temp_Rear -1 )!= Rear[pri_loc]) { //��� task�� �� 1ĭ �տ��� push �ϸ� ���������� Rear�� 1��ŭ�� �����Ѵ�.
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