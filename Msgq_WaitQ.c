#include "Msgq_WaitQ.h"
//#include "ubinos.h"
#include <stdio.h>
#include <assert.h>


extern Msgq* msgq_list;

int MSGQ_WQ_FULL(msgq_pt msid)
{
	if ((msgq_list[msid].Rear + 1) % (NUM_OF_TASKS+1)  == msgq_list[msid].Front)
		return 1;
	else
		return 0;

}

int MSGQ_WQ_EMPTY(msgq_pt msid)
{
	if (msgq_list[msid].Front == msgq_list[msid].Rear)
		return 1;
	else
		return 0;
}

/*int Find_msgq_Btask(int* task_loc, int tid, msgq_pt msid) //use for mutex lock_timed 
{
	int j = 0;

	for (j = 0; j < NUM_OF_TASKS; j++)
	{
		if (msgq_list[msid].msgqQ[j].tid == tid)
		{
			*task_loc = j;
			return 0;//finded
		}
	}

	return -1;//not find
}*/

void msgqQ_sort(msgq_pt msid)
{
	unsigned char temp_tid;
	//'unsigned char temp_prio;
	int i = 0;
	int j = 0;
	for (i = 0; i < (msgq_list[msid].Rear - msgq_list[msid].Front + (NUM_OF_TASKS+1))%(NUM_OF_TASKS+1); i++)
	{
		int tp_front = msgq_list[msid].Front;
		for (j = 0; j < ((msgq_list[msid].Rear- msgq_list[msid].Front+(NUM_OF_TASKS+1))%(NUM_OF_TASKS+1)) - i; j++)
		{
			if (task_dyn_info[msgq_list[msid].msgqQ[tp_front].tid].dyn_prio < task_dyn_info[msgq_list[msid].msgqQ[(tp_front+1) % (NUM_OF_TASKS+1)].tid].dyn_prio)
			{
				temp_tid = msgq_list[msid].msgqQ[tp_front].tid;
			//	temp_prio = msgq[msid].msgqQ[tp_front + 1].prio;

				msgq_list[msid].msgqQ[tp_front].tid = msgq_list[msid].msgqQ[(tp_front+1) % (NUM_OF_TASKS+1)].tid;
				//msgq[msid].msgqQ[tp_front].prio = msgq[msid].msgqQ[tp_front+1].prio;

				msgq_list[msid].msgqQ[(tp_front+1) % (NUM_OF_TASKS+1)].tid = temp_tid;
				//msgq[msid].msgqQ[tp_front+1].tid = temp_tid;
			}
			tp_front = (tp_front + 1) % (NUM_OF_TASKS+1);
		}
	}
}

int push_msgq_task_into_WQ(unsigned char tid, unsigned char p, msgq_pt msid)
{
	task_state[tid] = Blocked;
	if (MSGQ_WQ_FULL(msid))
	{
		printf("sem_waittingQ is full!\n");
		return -1;
	}
	else
	{
		//printf("enQ -> rear: %d\n\n", Rear);
		task_state[tid] = Blocked;
		//printf("task_state[tid][act_counter[tid]] = %d \n", task_state[tid]);
		msgq_list[msid].msgqQ[msgq_list[msid].Rear].tid = tid;
		task_dyn_info[msgq_list[msid].msgqQ[msgq_list[msid].Rear].tid].dyn_prio = p;

		msgq_list[msid].Rear = ( 1 + msgq_list[msid].Rear) % (NUM_OF_TASKS+1);
		if ((msgq_list[msid].Rear- msgq_list[msid].Front+(NUM_OF_TASKS+1))%(NUM_OF_TASKS+1) > 1)//More than one element, sorting
		{
			msgqQ_sort(msid);
		}

		return 0;

	}


}

int temp_Rear;


int get_msgq_task_from_WQ(unsigned char* tid, unsigned char* prio, msgq_pt msid)
{
	if (MSGQ_WQ_EMPTY(msid))
	{
		printf("sem_waitQ is empty\n");
		return -1;
	}

	*tid = msgq_list[msid].msgqQ[msgq_list[msid].Front].tid;
	*prio = task_dyn_info[*tid].dyn_prio;

	msgq_list[msid].msgqQ[msgq_list[msid].Front].tid = 0;
	task_dyn_info[msgq_list[msid].msgqQ[msgq_list[msid].Front].tid].dyn_prio = 0;

	msgq_list[msid].Front = (msgq_list[msid].Front + 1) % (NUM_OF_TASKS+1);
	return 0;

}

int msgq_prio_change(unsigned char tid, unsigned char chan_prio, msgq_pt msid, int loc)
{
	if (msgq_list[msid].msgqQ[loc].tid == tid) {
		
		task_dyn_info[msgq_list[msid].msgqQ[loc].tid].dyn_prio = chan_prio;

		msgqQ_sort(msid);
		return 0;
	}
	else
		return -1;
}

int get_msgq_task_from_WQ_position(unsigned char* tid, unsigned char* prio, msgq_pt msid, int task_loc) //use for lock timed
{
	if (MSGQ_WQ_EMPTY(msid))
	{
		printf("waitingQ is empty\n");
		//current_tid = -1;
		return -1;
	}
	else {
		//printf("deQ -> get_task_from_WQ ->front : %d\n\n", Front);
		*tid = msgq_list[msid].msgqQ[task_loc].tid;
		*prio = task_dyn_info[*tid].dyn_prio;



		msgq_list[msid].msgqQ[task_loc].tid = 0; //
		task_dyn_info[msgq_list[msid].msgqQ[task_loc].tid].dyn_prio = 0;
		//

		if (msgq_list[msid].Front == task_loc)
		{
			msgq_list[msid].Front = (msgq_list[msid].Front + 1) % (NUM_OF_TASKS+1);
		}
		else if ((msgq_list[msid].Rear+(NUM_OF_TASKS+1))%(NUM_OF_TASKS+1) == 0)
		{
			msgqQ_sort(msid);
			msgq_list[msid].Rear = NUM_OF_TASKS;
		}
		else
		{
			msgqQ_sort(msid);
			msgq_list[msid].Rear--;
		}

	}

}



