#define _CRT_SECURE_NO_WARNINGS

/*
kernel.c includes implementation of system services(APIs) of Ubinos.

Author: YangSong
Date: 08/01/2019
*/
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "ubinos.h"
#include "kernel.h"
#include "readyQ.h"
#include "mylib.h"
#include "messageQ.h"
#include "Mutex_WaitQ.h"
#include "Sem_WaitQ.h"
#include "Msgq_WaitQ.h"
#include "Sleep_WaitQ.h"
#include <string.h>
#include <malloc.h>



unsigned int MID = 0;
unsigned int SID = 0;
unsigned int MSID = 0;

unsigned mutex_list_size;
unsigned sem_list_size;
unsigned msgq_list_size;

extern unsigned char max_activation_order[NUM_OF_TASKS + 1];
extern unsigned char cur_activation_order[NUM_OF_TASKS + 1];

unsigned char sleep_info[NUM_OF_TASKS + 1] = {-1};

unsigned int creation_times = 0;

API api;
API api_name;
extern signed char current_tid;
unsigned char current_prio;
//unsigned char e_code;
extern int act_counter[NUM_OF_TASKS + 1];


Msgq* msgq_list = NULL;
Mutex* mutex_list = NULL;
Sem* sem_list = NULL;


int task_create(unsigned char reftask)
{
	api = API_task_create;
	//API_Call_Suporter(API_task_create);

	if (task_static_info[reftask].max_act_cnt == 0)
		initialize();

	//check whether max activation count has been reached
	if (reftask < 0 || reftask > NUM_OF_TASKS)
	{
		e_code = E_OS_ID;
		return -1;
	}
	else if (task_dyn_info[reftask].act_cnt < task_static_info[reftask].max_act_cnt)
	{
		task_dyn_info[reftask].act_cnt++;
		push_task_into_readyQ(reftask, task_static_info[reftask].prio, 0);
	}
	else
	{
		e_code = E_OS_LIMIT;

		return -1;
	}
	/* background task of Erika? */
	if (current_tid == -1) {
		current_tid = reftask;
		return 0;
	}
	return 0;
}



unsigned char mutex_owner = 0;
int TerminateTask()
{
	api_name = API_TerminateTask;

	/*if (has_resource(current_tid))
	{
		e_code = E_OS_RESOURCE;

	}*/
	if (task_dyn_info[current_tid].act_cnt > 0)
		task_dyn_info[current_tid].act_cnt--;
	if (task_dyn_info[current_tid].act_cnt > 0)
	{
		task_state[current_tid] = Ready;
		//eventually(current_tid);
	}
	else if (mutex_owner == current_tid)
	{
		printf("please unlocking the mutex before you terminate task%d \n", current_tid);
		return -1;
	}
	else
	{
		task_state[current_tid] = Suspended;
	}
	min_activation_order[current_tid] = cur_activation_order[current_tid] + 1;
	return 0;
}


unsigned char task_tid;

void sleepQ_init()
{
	for (int i = 0; i < NUM_OF_TASKS+1; i++)
	{
		Sleep_Q[i].tid = -1;
		Sleep_Q[i].time = -1;
	}
}

int task_sleep(unsigned int time)
{
	
	if (task_state[current_tid] == Running)
	{
		api_name = API_task_sleep;

		for (unsigned int i = 1; i < MID; i++)
		{
			if (current_tid == mutex_list[i].owner)
			{
				task_dyn_info[current_tid].In_SleepQ = 1;
			}
		}

		task_state[current_tid] = Blocked;
		push_sleep_task_into_WQ(current_tid, current_prio,time);
		return 0;

		//return current_tid;
	}
	else if (task_state[current_tid] == Suspended || task_state[current_tid]==Blocked)
	{
		return -1; //
	}

}


void sleep_timer()//Wake up the task
{

	for (int i = 0; i < NUM_OF_TASKS+1; i++)
	{
		if (Sleep_Q[i].time > 0)
		{
			Sleep_Q[i].time--;
		}
		else if(Sleep_Q[i].time == 0)
		{
			unsigned char temp_tid;
			unsigned char temp_prio;
			get_sleep_task_from_WQ(&temp_tid, &temp_prio);
			push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_tid]);
		}
	}
}






int mutex_create(mutex_pt* mutex)
{
	
	MID++;
	*mutex = MID;
	
	if (mutex_list == NULL)
	{
		mutex_list = (Mutex*)malloc((MID*2)*sizeof(Mutex));
		mutex_list_size = 2;
	}
	else if(MID+1 > mutex_list_size)
	{
		mutex_list = (Mutex*)realloc(mutex_list,(mutex_list_size*2)*sizeof(Mutex));
		mutex_list_size = mutex_list_size*2;

	}
	if (mutex_list == NULL)
	{
		assert(0);
	}
	
		mutex_list[MID].flag = 0;
		mutex_list[MID].Front = 0;
		mutex_list[MID].Rear = 0;
		mutex_list[MID].prio_inheri = 0;// priority inheritance 발생하는 지 기록
		mutex_list[MID].block_flag = 0;//Blocked인 task가 있는지 기록
		mutex_list[MID].owner = 0;
		mutex_list[MID].lock_counter = 0;
		mutex_list[MID].timed_flag = -1;

		for (int i = 0; i < NUM_OF_TASKS+1; i++)
		{
			mutex_list[MID].mutex_timed_info[i] = -1;
			mutex_list[MID].mutex_timed_flag[i] = -1;
			mutex_list[MID].mutexQ[i].tid = 0;
			
		}

		return 0;
}






int mutex_lock(mutex_pt mid)
{
	
	mutex_list[mid].lock_call[current_tid] = 1;


	if (mutex_list[mid].flag == 0 && mutex_list[mid].owner == 0)//first time 
	{
		mutex_list[mid].flag = 1;
		mutex_list[mid].owner = current_tid;
		mutex_owner = mutex_list[mid].owner;
		mutex_list[mid].lock_counter = 1;
		//mutex[0].call[current_tid]++;

		return 2;//succeed, no scheduling
	}
	else if (mutex_list[mid].lock_counter >= 1 && mutex_list[mid].owner == current_tid)// mutex 여려번 잠금할 수 있다.
	{
		mutex_list[mid].lock_counter++;
		return 2;//succeed, no scheduling
	}
	else
	{
		api_name = API_mutex_lock;
		if (task_dyn_info[mutex_list[mid].owner].dyn_prio < task_dyn_info[current_tid].dyn_prio)
		{
			unsigned int loc;//task location
			unsigned char temp_tid = 0;
			unsigned char temp_prio = 0;
			mutex_list[mid].prio_inheri++;

			task_state[current_tid] = Blocked;

			for (unsigned int i = 1; i <= MID; i++)
			{
				if (current_tid == mutex_list[i].owner)
				{
					task_dyn_info[current_tid].In_MutexQ = 1;
					task_dyn_info[current_tid].Msgq_Id = mid;
				}
			}
			

			mutex_list[mid].block_flag = 1;
			push_mutex_task_into_WQ(current_tid, current_prio,mid); // 수행중인 task가 lock 될 수 없으면 waitQ로 push 한다


			if (task_dyn_info[mutex_list[mid].owner].In_ReadyQ > 0) //If task with mutex is in readyQ 
			{
				//task_dyn_info[current_tid].In_ReadyQ = 0;
				loc = find_task_readyQ(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio);
				get_task_from_readyQ_position(&temp_tid, &temp_prio, mid, mutex_list, loc);
				task_dyn_info[temp_tid].dyn_prio = task_dyn_info[current_tid].dyn_prio;
				push_task_into_readyQ(temp_tid, task_dyn_info[current_tid].dyn_prio, current_pc[temp_tid]);// priority inheritance
				return 0;
			}
			else if (task_dyn_info[mutex_list[mid].owner].In_MutexQ > 0)
			{
				task_dyn_info[mutex_list[mid].owner].dyn_prio = task_dyn_info[current_tid].dyn_prio;
				loc = find_task_mutexQ(mutex_list[mid].owner, mutex_list, task_dyn_info[current_tid].Mutex_Id);
				mutex_prio_change(mutex_list[mid].owner, task_dyn_info[current_tid].dyn_prio, task_dyn_info[current_tid].Mutex_Id, loc);
				return 0;
			}
			else if (task_dyn_info[mutex_list[mid].owner].In_SleepQ > 0)
			{

				//task_dyn_info[current_tid].In_SleepQ = 0;
				task_dyn_info[mutex_list[mid].owner].dyn_prio = task_dyn_info[current_tid].dyn_prio;
				loc = find_task_sleepQ(mutex_list[mid].owner);
				sleep_prio_change(mutex_list[mid].owner, task_dyn_info[current_tid].dyn_prio, loc);
				return 0;

			}
			else if (task_dyn_info[mutex_list[mid].owner].In_MsgqQ > 0)
			{
				//task_dyn_info[current_tid].In_MsgqQ = 0;
				task_dyn_info[mutex_list[mid].owner].dyn_prio = task_dyn_info[current_tid].dyn_prio;
				loc = find_task_msgqWQ(mutex_list[mid].owner, msgq_list, task_dyn_info[mutex_list[mid].owner].Msgq_Id);
				msgq_prio_change(mutex_list[mid].owner, task_dyn_info[current_tid].dyn_prio, task_dyn_info[mutex_list[mid].owner].Msgq_Id, loc);
				return 0;

			}
			else if (task_dyn_info[mutex_list[mid].owner].In_SemQ > 0)
			{
				//task_dyn_info[current_tid].In_SemQ == 0;
				task_dyn_info[mutex_list[mid].owner].dyn_prio = task_dyn_info[current_tid].dyn_prio;
				loc = find_task_semQ(mutex_list[mid].owner, sem_list, task_dyn_info[current_tid].Sem_Id);
				sem_prio_change(mutex_list[mid].owner, task_dyn_info[current_tid].dyn_prio, task_dyn_info[current_tid].Sem_Id, loc);
				return 0;
			}
			else
				return -1;
			

		}
		else
		{
			mutex_list[mid].block_flag = 1;
			task_state[current_tid] = Blocked;
			push_mutex_task_into_WQ(current_tid, current_prio,mid);
			return 0;
		}
	}
}


int mutex_unlock(mutex_pt mid)
{
	//API_Call_Suporter(API_mutex_unlock);
	if (mutex_list[mid].flag == 1 && mutex_list[mid].lock_counter == 1 && mutex_list[mid].lock_call[current_tid] > 0)
	{
		unsigned char temp_tid;
		unsigned char temp_prio;
		unsigned int loc;



		if (mutex_list[mid].prio_inheri > 0) // priority 북구
		{
			if (current_tid != mutex_list[mid].owner && task_state[mutex_list[mid].owner] == Ready)
			{
				loc = find_task_readyQ(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio);
				get_task_from_readyQ_position(&temp_tid, &temp_prio, mid, mutex_list, loc);
				assert(temp_tid == mutex_list[mid].owner);
				task_dyn_info[temp_tid].dyn_prio = task_static_info[mutex_list[mid].owner].prio;
				push_task_into_readyQ(temp_tid, task_dyn_info[temp_tid].dyn_prio, current_pc[temp_tid]);
			}
			else if (current_tid == mutex_list[mid].owner)
			{
				if (task_dyn_info[current_tid].In_ReadyQ > 0)
				{
					task_dyn_info[current_tid].In_ReadyQ --;
					mutex_list[mid].prio_inheri--;

					task_dyn_info[mutex_list[mid].owner].dyn_prio = task_static_info[mutex_list[mid].owner].prio;
					push_task_into_readyQ(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio, current_pc[mutex_list[mid].owner]);
				}
			}
			else {

				if (task_dyn_info[mutex_list[mid].owner].In_MsgqQ > 0) //mutexQ
				{

					task_dyn_info[mutex_list[mid].owner].In_MsgqQ --;
					mutex_list[mid].prio_inheri--;
					loc = find_task_msgqWQ(mutex_list[mid].owner, msgq_list, task_dyn_info[mutex_list[mid].owner].Msgq_Id);
					task_dyn_info[mutex_list[mid].owner].dyn_prio = task_static_info[mutex_list[mid].owner].prio;
					msgq_prio_change(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio, task_dyn_info[mutex_list[mid].owner].Msgq_Id,loc);

				}
				else if (task_dyn_info[mutex_list[mid].owner].In_SleepQ > 0)
				{

					task_dyn_info[mutex_list[mid].owner].In_SleepQ --;
					mutex_list[mid].prio_inheri--;
					task_dyn_info[mutex_list[mid].owner].dyn_prio = task_static_info[mutex_list[mid].owner].prio;
					loc = find_task_sleepQ(mutex_list[mid].owner);
					sleep_prio_change(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio, loc);
				}
				else if (task_dyn_info[mutex_list[mid].owner].In_SemQ > 0)
				{


					task_dyn_info[mutex_list[mid].owner].In_SemQ --;
					mutex_list[mid].prio_inheri--;
					task_dyn_info[mutex_list[mid].owner].dyn_prio = task_static_info[mutex_list[mid].owner].prio;
					loc = find_task_semQ(mutex_list[mid].owner, sem_list, task_dyn_info[mutex_list[mid].owner].Sem_Id);
					sem_prio_change(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio, task_dyn_info[mutex_list[mid].owner].Sem_Id,loc);

				}
				else if(task_dyn_info[mutex_list[mid].owner].In_MutexQ > 0)
				{
					task_dyn_info[mutex_list[mid].owner].In_MutexQ--;
					mutex_list[mid].prio_inheri--;
					task_dyn_info[mutex_list[mid].owner].dyn_prio = task_static_info[mutex_list[mid].owner].prio;
					loc = find_task_mutexQ(mutex_list[mid].owner, mutex_list, task_dyn_info[mutex_list[mid].owner].Mutex_Id);
					mutex_prio_change(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio, task_dyn_info[mutex_list[mid].owner].Mutex_Id, loc);
				}
		
				
			}

		}
		mutex_list[mid].flag = 0;
		mutex_list[mid].owner = 0;
		mutex_list[mid].lock_counter = 0;
		mutex_list[mid].lock_call[current_tid] = 0;
		mutex_owner = mutex_list[mid].owner;
		
		if (mutex_list[mid].block_flag > 0) // 지금 waitQ에서(block) task가 있으면 그리고 해당task 이미 mutex_lock 호출하면 
		{
			//block된 task 수행될 때 mutex 가지고 있는 상태로 resume


			get_mutex_task_from_WQ(&temp_tid, &temp_prio,mid); // block중인 tid 흭득
			mutex_list[mid].block_flag--;
			task_state[temp_tid] = Ready;
			mutex_list[mid].flag = 1;
			mutex_list[mid].owner = temp_tid;
			mutex_owner = mutex_list[mid].owner;
			mutex_list[mid].lock_counter++;
			push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_tid]);//readyQ로 추가해서 바로 실행하지 않는다.
			return 0; 

		}

		return 2;//non scheduling
	}
	else if (mutex_list[mid].lock_counter > 1)
	{
		mutex_list[mid].lock_counter--;
		return 2;//no schuedling
	}
	else
		return -1;
}


int mutex_delete(mutex_pt *mid)
{

	mutex_list[MID].flag = -1;
	mutex_list[MID].Front = -1;
	mutex_list[MID].prio_inheri = -1;
	mutex_list[MID].block_flag = -1;
	mutex_list[MID].owner = -1;
	mutex_list[MID].lock_counter = -1;
	mutex_list[MID].Rear = -1;


	*mid = -1;
	return 0;
}

		

int mutex_flag;
void mutex_timer()
{

	unsigned int loc;
	unsigned char temp_tid;
	unsigned char temp_prio;
	if(mutex_list!= NULL)
	{
		for (unsigned int j = 1; j <= MID; j++) //mutex ID starting from 1
		{
			for (int i = 1; i < NUM_OF_TASKS+1; i++) //Tid starting form 1
			{
				if (mutex_list[j].mutex_timed_flag[i] == 0)//If the lock fails, the associated mutex id is found and the timer starts
				{
					if (task_state[mutex_list[j].mutex_timed_info[i]] == Ready)
					{
						mutex_list[j].mutex_timed_info[i] = -1; //The unlock API has been called
						//printf("Task[%d] is put into readyQ via unblockAPI", i);
					}
					if (mutex_list[j].mutex_timed_info[i] > 0)
					{
						mutex_list[j].mutex_timed_info[i]--;
						//ret_val++; //keep going
					}
					else if (mutex_list[j].mutex_timed_info[i] == 0)
					{

						mutex_list[j].mutex_timed_info[i] = -1;
						mutex_list[j].mutex_timed_flag[i] = -1;
						loc = find_task_mutexQ(i,mutex_list,j);
						get_mutex_task_from_WQ_position(&temp_tid,&temp_prio,j,loc);
						push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_tid]);

						if (mutex_list[j].prio_inheri > 0)
						{
							if (current_tid != mutex_list[j].owner && task_state[mutex_list[j].owner] == Ready)
							{
								loc = find_task_readyQ(mutex_list[j].owner, task_dyn_info[mutex_list[j].owner].dyn_prio);
								get_task_from_readyQ_position(&temp_tid, &temp_prio, j, mutex_list, loc);
								assert(temp_tid == mutex_list[j].owner);
								task_dyn_info[temp_tid].dyn_prio = task_static_info[mutex_list[j].owner].prio;
								push_task_into_readyQ(temp_tid, task_dyn_info[temp_tid].dyn_prio, current_pc[temp_tid]);
							}
							else if (current_tid == mutex_list[j].owner)
							{
								if (task_dyn_info[current_tid].In_ReadyQ > 0)
								{
									task_dyn_info[current_tid].In_ReadyQ --;
									mutex_list[j].prio_inheri--;

									task_dyn_info[mutex_list[j].owner].dyn_prio = task_static_info[mutex_list[j].owner].prio;
									push_task_into_readyQ(mutex_list[j].owner, task_dyn_info[mutex_list[j].owner].dyn_prio, current_pc[mutex_list[j].owner]);
								}
							}
							else {

								if (task_dyn_info[mutex_list[j].owner].In_MsgqQ > 0) //mutexQ
								{

									task_dyn_info[mutex_list[j].owner].In_MsgqQ--;
									mutex_list[j].prio_inheri--;
									loc = find_task_msgqWQ(mutex_list[j].owner, msgq_list, task_dyn_info[mutex_list[j].owner].Msgq_Id);
									task_dyn_info[mutex_list[j].owner].dyn_prio = task_static_info[mutex_list[j].owner].prio;
									msgq_prio_change(mutex_list[j].owner, task_dyn_info[mutex_list[j].owner].dyn_prio, task_dyn_info[mutex_list[j].owner].Msgq_Id, loc);

								}
								else if (task_dyn_info[mutex_list[j].owner].In_SleepQ > 0)
								{

									task_dyn_info[mutex_list[j].owner].In_SleepQ--;
									mutex_list[j].prio_inheri--;
									task_dyn_info[mutex_list[j].owner].dyn_prio = task_static_info[mutex_list[j].owner].prio;
									loc = find_task_sleepQ(mutex_list[j].owner);
									sleep_prio_change(mutex_list[j].owner, task_dyn_info[mutex_list[j].owner].dyn_prio, loc);
								}
								else if (task_dyn_info[mutex_list[j].owner].In_SemQ > 0)
								{


									task_dyn_info[mutex_list[j].owner].In_SemQ--;
									mutex_list[j].prio_inheri--;
									task_dyn_info[mutex_list[j].owner].dyn_prio = task_static_info[mutex_list[j].owner].prio;
									loc = find_task_semQ(mutex_list[j].owner, sem_list, task_dyn_info[mutex_list[j].owner].Sem_Id);
									sem_prio_change(mutex_list[j].owner, task_dyn_info[mutex_list[j].owner].dyn_prio, task_dyn_info[mutex_list[j].owner].Sem_Id, loc);

								}
								else if (task_dyn_info[mutex_list[j].owner].In_MutexQ > 0)
								{
									task_dyn_info[mutex_list[j].owner].In_MutexQ--;
									mutex_list[j].prio_inheri--;
									task_dyn_info[mutex_list[j].owner].dyn_prio = task_static_info[mutex_list[j].owner].prio;
									loc = find_task_mutexQ(mutex_list[j].owner, mutex_list, task_dyn_info[mutex_list[j].owner].Mutex_Id);
									mutex_prio_change(mutex_list[j].owner, task_dyn_info[mutex_list[j].owner].dyn_prio, task_dyn_info[mutex_list[j].owner].Mutex_Id, loc);
								}
							}
						}
					}
				}
			}
		}
	}
}




int mutex_lock_timed(mutex_pt mid, unsigned int time)
{
	//API_Call_Suporter(API_mutex_lock_timed);
	
	mutex_list[mid].mutex_timed_flag[current_tid] = mutex_lock(mid);
	if (mutex_list[mid].mutex_timed_flag[current_tid] == 0)
	{
		mutex_list[mid].mutex_timed_info[current_tid] = time;
		return 0;//current_tid  == Blocked
	}
	else if (mutex_list[mid].mutex_timed_flag[current_tid] == -1)
		return -1;//error
	else
		return 2;//non scheduling
	

}




int sem_create(sem_pt *sid)
{
	SID++;//
	*sid = SID;
	
	if (sem_list == NULL)
	{
		sem_list = (Sem*)malloc((SID * 2)*sizeof(Sem));
		sem_list_size = 2;
	}
	else if(SID+1 > sem_list_size)
	{
		sem_list = (Sem*)realloc(sem_list, (sem_list_size*2)*sizeof(Sem));
		sem_list_size = sem_list_size * 2;
	}
	if (sem_list == NULL)
		return -1;
	else
	{
		sem_list[SID].Front = 0;
		sem_list[SID].Rear = 0;
		sem_list[SID].counter = 0;
		//sem_list[*size].sem_flag = -1;
		
		for (int i = 0; i < NUM_OF_TASKS+1; i++)
		{
			sem_list[SID].sem_timed_info[i] = -1;
			sem_list[SID].sem_timed_flag[i] = -1;
			sem_list[SID].semQ[i].tid = 0;
			
		}

		
		return 0;
	}

	
}


int sem_delete(sem_pt *sid)
{
	//4294967295
	sem_list[SID].Front = -1;
	sem_list[SID].Rear = -1;
	sem_list[SID].counter = -1;
	//sem_list[*size].sem_mutex_owner = -1;
	//sem_list[*sid].sem_flag = -1;

	*sid = -1;
	
	return 0;
	
}

int sem_give(sem_pt sid)
{
	unsigned char temp_tid;
	unsigned char temp_prio;

	if (SEM_WQ_EMPTY(sid))
	{
		sem_list[sid].counter++;
		return 2;//no scheduling
	}
	else
	{
		get_sem_task_from_WQ(&temp_tid, &temp_prio, sid);
		push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_tid]);
		return 0;
	}
}




int sem_take(sem_pt sid)
{
	
	
	if (sem_list[sid].counter > 0)
	{
		sem_list[sid].counter = sem_list[sid].counter - 1;
		return 2;//no scheduling

	}
	else
	{
		api_name = API_sem_take;

		for (unsigned int i = 1; i <= MID; i++)
		{
			if (current_tid == mutex_list[i].owner)
			{
				task_dyn_info[current_tid].In_SemQ = 1;
				task_dyn_info[current_tid].Sem_Id = sid;
			}
		}
		push_sem_task_into_WQ(current_tid, current_prio,sid);
		
		//printf("%d", sem_list[1].semQ[0].tid);
		return 0;
	}
}

void sem_timer()
{
	int ret_val = 0;
	if (sem_list != NULL)
	{
		for (unsigned int j = 1; j <= SID; j++) //sem ID starting from 1
		{
			
				for (int i = 1; i < NUM_OF_TASKS+1; i++) //Tid starting form 1
				{
					if (sem_list[j].sem_timed_flag[i] == 0)//If the take fails, the associated sem id is found and the timer starts
					{
						if (task_state[sem_list[j].sem_timed_info[i]] == Ready)
						{
							sem_list[j].sem_timed_info[i] = -1; //The give API has been called
							//printf("Task[%d] is put into readyQ via Semgive API", i);
						}
						if (sem_list[j].sem_timed_info[i] > 0)
						{
							sem_list[j].sem_timed_info[i]--;
							//ret_val++; //keep going
						}
						else if (sem_list[j].sem_timed_info[i] == 0)
						{
							unsigned char temp_tid;
							unsigned char temp_prio;
							unsigned int loc;
							
							sem_list[j].sem_timed_flag[i] = -1;
							sem_list[j].sem_timed_info[i] = -1;
							loc = find_task_semQ(i, sem_list, j);
							get_sem_task_from_WQ_position(&temp_tid, &temp_prio, j, loc);//get from wq
							push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_tid]);//push
							//ret_val++;
						}
					}
				}
			
		}
	}
	
	//return ret_val > 0 ? 0 : 2;
}

int sem_take_timed(sem_pt sid, int time)
{
	//API_Call_Suporter(API_sem_take_timed);
	sem_list[sid].sem_timed_flag[current_tid] = sem_take(sid);
	if (sem_list[sid].sem_timed_info[current_tid] == 0)
	{
		sem_list[sid].sem_timed_info[current_tid] = time;
		return 0;
	}
	else if(sem_list[sid].sem_timed_info[current_tid] == -1)
		return -1;
	else
		return 2;//non-scheduling
}


void multi_time_checker()
{
	sleep_timer();
	mutex_timer();
	sem_timer();
}


//message Q



int msgq_create(msgq_pt *msid, unsigned int msgsize,int maxcount) 
{
	MSID++;
	*msid = MSID;
	if (msgq_list == NULL)
	{
		msgq_list = (Msgq*)malloc((MSID * 2)* sizeof(Msgq));
		msgq_list_size = 2;
	}
	else if(MSID+1 > msgq_list_size)
	{
		msgq_list = (Msgq*)realloc(msgq_list, (msgq_list_size*2)*sizeof(Msgq));
		msgq_list_size = msgq_list_size * 2;
	}
	if (msgq_list == NULL)
	{
		return -1;
	}
	else
	{
		msgq_list[MSID].Front = 0;
		msgq_list[MSID].Rear = 0;
		msgq_list[MSID].counter = 0;
		msgq_list[MSID].maxcounter = 0;
		msgq_list[MSID].R = 0;
		msgq_list[MSID].F = 0;

		for (int i = 0; i< NUM_OF_TASKS+1;i++)
		{
			msgq_list[MSID].msgqQ[i].tid = 0;
		}

		msgq_list[MSID].Message_Queue = (MQ*)calloc(maxcount, sizeof(MQ));
		//assert(!(msgq_list[MSID].Message_Queue == NULL));
		if (msgq_list[MSID].Message_Queue == NULL)
		{
			return -1;
		}
		else {
			msgq_list[MSID].maxcounter = maxcount;
			for (int i = 0; i < maxcount; i++)
			{
				msgq_list[MSID].Message_Queue[i].message = (unsigned char*)calloc(msgsize , sizeof(unsigned char));
				//assert(!(msgq_list[MSID].Message_Queue[i].message == NULL));
			}

			return 0;
		}
				
			
		
	}
}



int msgq_send(msgq_pt msid, unsigned char *message)
{

	//API_Call_Suporter(API_msgq_send);
	unsigned char temp_tid;
	unsigned char temp_prio;

	if (msgq_list[msid].counter > msgq_list[msid].maxcounter)
		return -1;//overflow
	else
	{
		
		if (!(MSGQ_WQ_EMPTY(msid)))
		{
			get_msgq_task_from_WQ(&temp_tid, &temp_prio,msid);
			push_task_into_readyQ(temp_tid, temp_prio, current_pc[current_tid]);
			//strcpy(task_dyn_info[current_tid].task_message[current_tid].buf, message);
			strcpy(task_dyn_info[temp_tid].task_message[temp_tid].buf, message);
			return 0;
		}
		else
		{
			push_message_into_MQ(msid,message);
			msgq_list[msid].counter++;
			return 2;
		}
	}
}



int msgq_receive(msgq_pt msid,unsigned char* message)
{
	
	if (msgq_list[msid].counter > 0)
	{
		get_message_from_MQ(msid, message);
		msgq_list[msid].counter --;
		return 2;
	}
	else
	{
		api_name = API_msgq_receive;
		for (unsigned int i = 1; i <= MID; i++)
		{
			if (current_tid == mutex_list[i].owner)
			{
				task_dyn_info[current_tid].In_MsgqQ  = 1;
				task_dyn_info[current_tid].Msgq_Id = msid;
			}
		}
		//task_dyn_info[current_tid].task_message[current_tid].buf = message;
		task_dyn_info[current_tid].task_message[current_tid].buf = message;
		push_msgq_task_into_WQ(current_tid, current_prio, msid);
		return 0;
	}
}

int msgq_delete(msgq_pt *msid)
{

	msgq_list[MSID].Front = -1;
	msgq_list[MSID].Rear = -1;
	msgq_list[MSID].counter = -1;
	msgq_list[MSID].maxcounter = -1;
	msgq_list[MSID].R = -1;
	msgq_list[MSID].F = -1;
	
	free(msgq_list[MSID].Message_Queue);
	*msid = -1;
	return 0;
}



int os_on;
const int ON = 1;
const int OFF = 0;

void ubik_comp_start()
{
	if (os_on == OFF)
	{
		os_on = ON;
		sleepQ_init();
		scheduler();
		running(); //call tasks here
	}
}

void ShutDownOS()
{
	if (os_on == ON)
	{
		os_on = OFF;

		if (mutex_list != NULL)
		{
			free(mutex_list);
		}
		if (sem_list != NULL)
		{
			free(sem_list);
		}
		if (msgq_list != NULL)
		{
			free(msgq_list);
		}

	}
}
