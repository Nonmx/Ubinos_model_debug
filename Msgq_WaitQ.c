#include "Msgq_WaitQ.h"
//#include "ubinos.h"
#include <stdio.h>
#include <assert.h>


int MSGQ_WQ_FULL(msgq_pt msid, Msgq* msgq)
{
	if ((msgq[msid].Rear + 1) % (WAITQ_SIZE + 1) == msgq[msid].Front)
		return 1;
	else
		return 0;

}

int MSGQ_WQ_EMPTY(msgq_pt msid, Msgq* msgq)
{
	if (msgq[msid].Front == msgq[msid].Rear)
		return 1;
	else
		return 0;
}

int Find_msgq_Btask(int* task_loc, int tid, msgq_pt msid, Msgq* msgq) //use for mutex lock_timed 
{
	int j = 0;

	for (j = 0; j < WAITQ_SIZE; j++)
	{
		if (msgq[msid].msgqQ[j].tid == tid)
		{
			*task_loc = j;
			return 0;//finded
		}
	}

	return -1;//not find
}

void msgqQ_sort(msgq_pt msid, Msgq* msgq)
{
	unsigned char temp_tid;
	unsigned char temp_prio;
	int i = 0;
	int j = 0;
	for (i = msgq[msid].Front; i < msgq[msid].Rear; i++)
	{
		for (j = msgq[msid].Front; j < msgq[msid].Rear - i; j++)
		{
			if (task_dyn_info[msgq[msid].msgqQ[i].tid].dyn_prio < task_dyn_info[msgq[msid].msgqQ[i++].tid].dyn_prio)
			{
				temp_tid = msgq[msid].msgqQ[i].tid;
				temp_prio = msgq[msid].msgqQ[i].prio;

				msgq[msid].msgqQ[i].tid = msgq[msid].msgqQ[i++].tid;
				msgq[msid].msgqQ[i].prio = msgq[msid].msgqQ[i++].prio;

				msgq[msid].msgqQ[i++].tid = temp_tid;
				msgq[msid].msgqQ[i++].tid = temp_tid;
			}
		}
	}
}

int push_msgq_task_into_WQ(unsigned char tid, unsigned char p, msgq_pt msid, Msgq* msgq)
{
	task_state[tid] = Blocked;
	if (MSGQ_WQ_FULL(msid, msgq))
	{
		printf("sem_waittingQ is full!\n");
		return -1;
	}
	else
	{
		//printf("enQ -> rear: %d\n\n", Rear);
		task_state[tid] = Blocked;
		//printf("task_state[tid][act_counter[tid]] = %d \n", task_state[tid]);
		msgq[msid].msgqQ[msgq[msid].Rear].tid = tid;
		msgq[msid].msgqQ[msgq[msid].Rear].prio = p;

		msgq[msid].Rear = (WAITQ_SIZE + 1 + msgq[msid].Rear) % WAITQ_SIZE;
		if (msgq[msid].Rear > 1)//More than one element, sorting
		{
			msgqQ_sort(msid, msgq);
		}

		return 0;

	}


}

int temp_Rear;


int get_msgq_task_from_WQ(unsigned char* tid, unsigned char* prio, msgq_pt msid, Msgq* msgq)
{
	if (MSGQ_WQ_EMPTY(msid, msgq))
	{
		printf("sem_waitQ is empty\n");
		return -1;
	}

	*tid = msgq[msid].msgqQ[msgq[msid].Front].tid;
	*prio = msgq[msid].msgqQ[msgq[msid].Front].prio;

	msgq[msid].Front = (msgq[msid].Front + 1) % WAITQ_SIZE;
	return 0;

}

int msgq_prio_change(unsigned char tid, unsigned char chan_prio, msgq_pt msid, Msgq* msgq, int loc)
{
	if (msgq[msid].msgqQ[loc].tid == tid) {
		msgq[msid].msgqQ[loc].prio = chan_prio;
		msgqQ_sort(msid, msgq);
		return 0;
	}
	else
		return -1;
}

void get_msgq_task_from_WQ_position(unsigned char* tid, unsigned char* prio, msgq_pt msid, Msgq* msgq, int task_loc) //use for lock timed
{
	if (MSGQ_WQ_EMPTY(msid, msgq))
	{
		printf("waitingQ is empty\n");
		//current_tid = -1;
		//return 0;
	}
	else {
		//printf("deQ -> get_task_from_WQ ->front : %d\n\n", Front);
		*tid = msgq[msid].msgqQ[task_loc].tid;
		*prio = msgq[msid].msgqQ[task_loc].prio;



		msgq[msid].msgqQ[task_loc].tid = 0; //
		msgq[msid].msgqQ[task_loc].prio = 0;
		//

		if (msgq[msid].Front == task_loc)
		{
			msgq[msid].Front = (msgq[msid].Front + 1) % WAITQ_SIZE;
		}

		else if ((task_loc + 1 + WAITQ_SIZE) % WAITQ_SIZE == msgq[msid].Rear)//task가 waitQ의 중간에 꺼내면 이 task가 뒤에 있는 task들이 다 앞으로 다시 push 해야 한다.
		{
			msgq[msid].Rear--;
			if (msgq[msid].Rear < 0)
				msgq[msid].Rear = WAITQ_SIZE;
		}
		else if (msgq[msid].Rear < task_loc)
		{
			msgqQ_sort(msid, msgq);
			if (msgq[msid].Rear < 0)
				msgq[msid].Rear = WAITQ_SIZE;
		}
		else
		{
			msgqQ_sort(msid, msgq);
			msgq[msid].Rear--;
		}

	}

}



