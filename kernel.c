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



int MID = 1;
int SID = 1;
int MSID = 1;

extern unsigned char max_activation_order[NUM_OF_TASKS + 1];
extern unsigned char cur_activation_order[NUM_OF_TASKS + 1];

unsigned char sleep_info[NUM_OF_TASKS + 1] = {-1};

unsigned int creation_times = 0;

API api;
API api_suporter;
extern signed char current_tid;
unsigned char current_prio;
//unsigned char e_code;
extern int act_counter[NUM_OF_TASKS + 1];


Msgq* msgq_list = NULL;
Mutex* mutex_list = NULL;
Sem* sem_list = NULL;

void API_Call_Suporter(API api);

int task_create(unsigned char reftask)
{
	api = API_task_create;
	API_Call_Suporter(API_task_create);

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

void API_Call_Suporter(API api)
{
	api_suporter = api;

}

unsigned char mutex_owner = 0;
int TerminateTask()
{
	
	api = API_TerminateTask;
	API_Call_Suporter(API_TerminateTask);

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
	for (int i = 0; i < WAITQ_SIZE; i++)
	{
		Sleep_Q[i].tid = -1;
		Sleep_Q[i].time = -1;
	}
}

int task_sleep(unsigned int time)
{
	
	if (task_state[current_tid] == Running)
	{
		API_Call_Suporter(API_task_sleep);
		api = API_task_sleep;
		API_Call_Suporter(API_task_sleep);
		//sleep_info[current_tid] = time+1; //시간 tick 초기화는 -1이기 때문에 
		task_state[current_tid] = Blocked;
		//sleepQ_init();
		push_sleep_task_into_WQ(current_tid, current_prio,time);
		return 0;

		//return current_tid;
	}
	else if (task_state[current_tid] == Suspended || task_state[current_tid]==Blocked)
	{
		return -1; //
	}

}


int sleep_timer()//Wake up the task
{

	for (int i = 0; i < NUM_OF_TASKS; i++)
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
	*mutex = MID;
	creation_times++;
	if (mutex_list == NULL)
	{
		mutex_list = (Mutex*)malloc((MID*2)*sizeof(Mutex));
	}
	else
	{
		mutex_list = (Mutex*)realloc(mutex_list,(MID*2)*sizeof(Mutex));

	}
	if (mutex_list == NULL)
	{
		assert(0);
	}
	
		mutex_list[*mutex].flag = 0;
		mutex_list[*mutex].Front = 0;
		mutex_list[*mutex].Rear = 0;
		mutex_list[*mutex].tra_flag = 0;
		mutex_list[*mutex].block_flag = 0;
		mutex_list[*mutex].owner = 0;
		mutex_list[*mutex].lock_counter = 0;
		mutex_list[*mutex].timed_flag = -1;
		mutex_list[*mutex].form_readyQ = -1;
		mutex_list[*mutex].form_mutexQ = -1;
		mutex_list[*mutex].form_sleepQ = -1;
		mutex_list[*mutex].form_semQ = -1;
		mutex_list[*mutex].form_msgqQ = -1;
		mutex_list[*mutex].mutex_flag = -1;


		for (int i = 0; i < NUM_OF_TASKS; i++)
		{
			mutex_list[*mutex].mutex_timed_info[i] = -1;
		}

		MID++;
		return 0;
}





int loc;//task location
int mutex_lock(mutex_pt mid)
{
	API_Call_Suporter(API_mutex_lock);

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
		if (task_dyn_info[mutex_list[mid].owner].dyn_prio < task_dyn_info[current_tid].dyn_prio)
		{
			unsigned char temp_tid = 0;
			unsigned char temp_prio = 0;
			unsigned int temp_mutex_obj_num;
			unsigned int temp_sem_obj_num;
			unsigned int temp_msgq_obj_num;
			mutex_list[mid].tra_flag = 1;

			int sleep_loc;
			int mutex_loc;
			int sem_loc;
			int msgq_loc;

			task_state[current_tid] = Blocked;
			mutex_list[mid].block_flag = 1;
			push_mutex_task_into_WQ(current_tid, current_prio,mid,mutex_list); // 수행중인 task가 lock 될 수 없으면 waitQ로 push 한다

			loc = find_task_readyQ(mutex_list[mid].owner,task_dyn_info[mutex_list[mid].owner].dyn_prio);
			if (loc != -1) //If task with mutex is in readyQ 
			{
				mutex_list[mid].form_readyQ = 1;
				get_task_from_readyQ_position(&temp_tid, &temp_prio, mid, mutex_list, loc);
				//task_dyn_info[temp_tid].dyn_prio = ; 
				push_task_into_readyQ(temp_tid, task_dyn_info[current_tid].dyn_prio, current_pc[temp_tid]);// priority inheritance
				return 0;
			}
			else
			{
				sleep_loc = find_task_sleepQ(mutex_list[mid].owner);
				if (sleep_loc != -1)
				{
					mutex_list[mid].form_sleepQ = 1;
					//task_dyn_info[temp_tid].dyn_prio = task_dyn_info[current_tid].dyn_prio;
					sleep_prio_change(mutex_list[mid].owner, task_dyn_info[current_tid].dyn_prio, sleep_loc);
					return 0;
				}
				else
				{
					mutex_loc = find_task_mutexQ(mutex_list[mid].owner, mutex_list, &temp_mutex_obj_num);
					if (mutex_loc != -1)
					{
						mutex_list[mid].form_mutexQ = 1;
						//task_dyn_info[temp_tid].dyn_prio = task_dyn_info[current_tid].dyn_prio;
						mutex_prio_change(mutex_list[mid].owner, task_dyn_info[current_tid].dyn_prio, temp_mutex_obj_num, mutex_list, mutex_loc);

						return 0;
					}
					else
					{
						sem_loc = find_task_semQ(mutex_list[mid].owner, sem_list, &temp_sem_obj_num);
						if (sem_loc != -1)
						{
							mutex_list[mid].form_semQ = 1;
							//task_dyn_info[temp_tid].dyn_prio = task_dyn_info[current_tid].dyn_prio;
							sem_prio_change(mutex_list[mid].owner, task_dyn_info[current_tid].dyn_prio, temp_sem_obj_num, sem_list, sem_loc);
							return 0;
						}
						else
						{
							msgq_loc = find_task_msgqWQ(mutex_list[mid].owner, msgq_list, &temp_msgq_obj_num);
							if (msgq_loc != -1)
							{
								mutex_list[mid].form_msgqQ++;
								//task_dyn_info[temp_tid].dyn_prio = task_dyn_info[current_tid].dyn_prio;
								msgq_prio_change(mutex_list[mid].owner, task_dyn_info[current_tid].dyn_prio, temp_msgq_obj_num, msgq_list, msgq_loc);
								return 0;
							}
							else
								return -1;
						}
					}

				}
			}

		}
		else
		{
			mutex_list[mid].block_flag = 1;
			task_state[current_tid] = Blocked;
			push_mutex_task_into_WQ(current_tid, current_prio,mid,mutex_list);
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

		int sleep_loc;
		int mutex_loc;
		int sem_loc;
		int msgq_loc;

		unsigned int temp_mutex_obj_num;
		unsigned int temp_sem_obj_num;
		unsigned int temp_msgq_obj_num;


		if (mutex_list[mid].tra_flag >0 ) // priority 북구
		{
			if (current_tid != mutex_list[mid].owner && task_state[mutex_list[mid].owner] == Ready )
			{
				loc = find_task_readyQ(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio);
				get_task_from_readyQ_position(&temp_tid, &temp_prio, mid, mutex_list, loc);
				assert(temp_tid == mutex_list[mid].owner);
				task_dyn_info[temp_tid].dyn_prio = task_static_info[mutex_list[mid].owner].prio;
				push_task_into_readyQ(temp_tid, task_dyn_info[temp_tid].dyn_prio, current_pc[temp_tid]);
				return 2;
			}
			else if (current_tid == mutex_list[mid].owner)
			{
				if (mutex_list[mid].form_readyQ > 0)
				{
					mutex_list[mid].form_readyQ--;
					mutex_list[mid].tra_flag--;

					task_dyn_info[mutex_list[mid].owner].dyn_prio = task_static_info[mutex_list[mid].owner].prio;
					push_task_into_readyQ(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio, current_pc[mutex_list[mid].owner]);
				}
			}
			else{
				
				if (mutex_list[mid].form_mutexQ > 0 || mutex_list[mid].form_semQ > 0 || mutex_list[mid].form_sleepQ > 0 || mutex_list[mid].form_msgqQ) //mutexQ
				{
					mutex_loc = find_task_mutexQ(mutex_list[mid].owner, mutex_list, &temp_mutex_obj_num);
					if (mutex_loc != -1)
					{
						mutex_list[mid].form_mutexQ--;
						mutex_list[mid].tra_flag--;
						task_dyn_info[mutex_list[mid].owner].dyn_prio = task_static_info[mutex_list[mid].owner].prio;
						mutex_prio_change(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio, temp_mutex_obj_num, mutex_list, mutex_loc);
					}
					else
					{
						sleep_loc = find_task_sleepQ(mutex_list[mid].owner);
						if (sleep_loc != -1)
						{
							mutex_list[mid].form_sleepQ--;
							mutex_list[mid].tra_flag--;
							task_dyn_info[mutex_list[mid].owner].dyn_prio = task_static_info[mutex_list[mid].owner].prio;
							sleep_prio_change(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio, sleep_loc);
						}
						else
						{
							sem_loc = find_task_semQ(mutex_list[mid].owner, sem_list, &temp_sem_obj_num);
							if (sem_loc != -1)
							{
								mutex_list[mid].form_semQ--;
								mutex_list[mid].tra_flag--;
								task_dyn_info[mutex_list[mid].owner].dyn_prio = task_static_info[mutex_list[mid].owner].prio;
								sem_prio_change(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio, temp_sem_obj_num, sem_list, sem_loc);
							}
							else
							{
								msgq_loc = find_task_msgqWQ(mutex_list[mid].owner, msgq_list, &temp_msgq_obj_num);
								if (msgq_loc != -1)
								{
									mutex_list[mid].form_msgqQ++;
									task_dyn_info[mutex_list[mid].owner].dyn_prio = task_dyn_info[current_tid].dyn_prio;
									msgq_prio_change(mutex_list[mid].owner, task_dyn_info[mutex_list[mid].owner].dyn_prio, temp_msgq_obj_num, msgq_list, msgq_loc);
								}
								else
									return -1;
							}
						}
					}
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


			get_mutex_task_from_WQ(&temp_tid, &temp_prio,mid,mutex_list); // block중인 tid 흭득
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

	mutex_list[*mid].flag = -1;
	mutex_list[*mid].Front = -1;
	mutex_list[*mid].tra_flag = -1;
	mutex_list[*mid].block_flag = -1;
	mutex_list[*mid].owner = -1;
	mutex_list[*mid].lock_counter = -1;
	mutex_list[*mid].Rear = -1;
	mutex_list[*mid].timed_flag = -1;
	mutex_list[*mid].form_readyQ = -1;
	mutex_list[*mid].form_mutexQ = -1;
	mutex_list[*mid].form_sleepQ = -1;
	mutex_list[*mid].form_semQ = -1;
	mutex_list[*mid].mutex_flag = -1;

	
	return 0;
}

		

int mutex_flag;
void mutex_timer()
{
	int ret_val = 0;
	if(mutex_list!= NULL)
	{
		for (int j = 1; j < MID; j++) //mutex ID starting from 1
		{
			if (mutex_list[j].mutex_flag == 0)//If the lock fails, the associated mutex id is found and the timer starts
			{
				for (int i = 1; i < NUM_OF_TASKS; i++) //Tid starting form 1
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
						unsigned char temp_tid;
						unsigned char temp_prio;
						int task_loc;
						if (Find_mutex_Btask(&task_loc, i, j, mutex_list) == -1)//find related task

						{
							assert(0); //not find task, bug!
						}
						get_mutex_task_from_WQ_position(&temp_tid,&temp_prio,j,mutex_list,task_loc);//get fromt wq
						push_task_into_readyQ(temp_tid,temp_prio,current_pc[temp_tid]);//push
						//ret_val++;
					}

				}
			}
		}
	}
}




int mutex_lock_timed(mutex_pt mid, unsigned int time)
{
	API_Call_Suporter(API_mutex_lock_timed);
	mutex_list[mid].mutex_timed_info[current_tid] = time+1;
	mutex_list[mid].mutex_flag = mutex_lock(mid);
	return mutex_list[mid].mutex_flag;
}




int sem_create(sem_pt *sid)
{
	*sid = SID;
	
	if (sem_list == NULL)
	{
		sem_list = (Sem*)malloc((SID * 2)*sizeof(Sem));
	}
	else //fix later
	{
		sem_list = (Sem*)realloc(sem_list, (SID * 2)*sizeof(Sem));
	}
	if (sem_list == NULL)
		return -1;
	else
	{
		sem_list[*sid].Front = 0;
		sem_list[*sid].Rear = 0;
		sem_list[*size].counter = 0;
		sem_list[*size].sem_flag = -1;
		
		for (int i = 0; i < NUM_OF_TASKS; i++)
		{
			sem_list[*sid].sem_timed_info[i] = -1;
		}

		SID++;
		return 0;
	}

	
}


int sem_delete(sem_pt *sid)
{
	//4294967295
	sem_list[*sid].Front = -1;
	sem_list[*sid].Rear = -1;
	sem_list[*sid].counter = -1;
	//sem_list[*size].sem_mutex_owner = -1;
	sem_list[*sid].sem_flag = -1;


	
	return 0;
	
}

int sem_give(sem_pt sid)
{
	unsigned char temp_tid;
	unsigned char temp_prio;



	if (SEM_WQ_EMPTY(sid,sem_list))
	{
		sem_list[sid].counter++;
		return 2;//no scheduling
	}
	else
	{
		get_sem_task_from_WQ(&temp_tid, &temp_prio, sid, sem_list);
		push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_tid]);
		return 0;
	}
}




int sem_take(sem_pt sid)
{
	
	API_Call_Suporter(API_sem_take);
	if (sem_list[sid].counter > 0)
	{
		sem_list[sid].counter = sem_list[sid].counter - 1;
		return 2;//no scheduling

	}
	else
	{

		push_sem_task_into_WQ(current_tid, current_prio,sid,sem_list);
		return 0;
	}
}

void sem_timer()
{
	int ret_val = 0;
	if (sem_list != NULL)
	{
		for (int j = 1; j < SID; j++) //sem ID starting from 1
		{
			if (sem_list[j].sem_flag == 0)//If the take fails, the associated sem id is found and the timer starts
			{
				for (int i = 1; i < NUM_OF_TASKS; i++) //Tid starting form 1
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
						int task_loc;
						if (Find_sem_Btask(&task_loc, i, j, sem_list) == -1)//find related task
						{
							assert(0); //not find task, bug!
						}
						get_sem_task_from_WQ_position(&temp_tid, &temp_prio, j, sem_list, task_loc);//get from wq
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
	API_Call_Suporter(API_sem_take_timed);
	sem_list[sid].sem_timed_info[current_tid] = time+1;
	sem_list[sid].sem_flag = sem_take(sid);
	return sem_list[sid].sem_flag;
}


void multi_time_checker()
{
	sleep_timer();
	mutex_timer();
	sem_timer();
}


//message Q



int msgq_create(msgq_pt *msid, unsigned int msgsize,unsigned int maxcount) 
{

	*msid = MSID;
	if (msgq_list == NULL)
	{
		msgq_list = (Msgq*)malloc((MSID * 2)* sizeof(Msgq));
	}
	else
	{
		msgq_list = (Msgq*)realloc(msgq_list, MSID * 2*sizeof(Msgq));
	}
	if (msgq_list == NULL)
	{
		return -1;
	}
	else
	{

		msgq_list[*msid].flag = -1;
		msgq_list[*msid].Front = 0;
		msgq_list[*msid].Rear = 0;
		msgq_list[*msid].counter = 0;


		msgq_list[*msid].Message_Queue = (MQ*)calloc(maxcount, sizeof(MQ));
		if (msgq_list[*msid].Message_Queue == NULL)
		{
			return -1;
		}
		else
		{
			msgq_list[*msid].maxcounter = maxcount;
			for (int i = 0; i < maxcount; i++)
			{
				
				msgq_list[*msid].Message_Queue[i].message = (char*)calloc(msgsize,sizeof(char));
				if (msgq_list[*msid].Message_Queue[i].message == NULL)
				{
					return -1;
				}
			}
			MSID++;
			return 0;
				
		}		
		
	}
}



int msgq_send(msgq_pt msid, static unsigned char *message)
{

	//API_Call_Suporter(API_msgq_send);
	int r;
	unsigned char* temp_message_buf;
	unsigned char temp_tid;
	unsigned char temp_prio;

	if (msgq_list[msid].counter > msgq_list[msid].maxcounter)
		return -1;//overflow
	else
	{
		
		if (!(MSGQ_WQ_EMPTY(msid, msgq_list)))
		{
			get_msgq_task_from_WQ(&temp_tid, &temp_prio,msid,msgq_list);
			//push_message_into_MQ(msid, message);
			//msgq_list[msid].counter++;
			push_task_into_readyQ(temp_tid, temp_prio, current_pc[current_tid]);
			memcpy(msgq_list[msid].owner[temp_tid].buf, message, sizeof(message)/sizeof(char));
			return 0;
		}
		else
		{
			push_message_into_MQ(msid,message,msgq_list);
			msgq_list[msid].counter++;
			return 2;
		}
	}
}



int msgq_receive(msgq_pt msid, static unsigned char* message)
{
	API_Call_Suporter(API_msgq_receive);
	if (msgq_list[msid].counter > 0)
	{
		get_message_from_MQ(msid, message,msgq_list);
		msgq_list[msid].counter --;
		return 2;
	}
	else
	{
		//messgae_write(message, msid,current_tid);//task->wtask_p->msg
		msgq_list[msid].owner[current_tid].buf = message;
		push_msgq_task_into_WQ(current_tid, current_prio, msid,msgq_list);
		return 0;
	}
}

int msgq_delete(msgq_pt *msid)
{
	msgq_list[*msid].flag = -1;
	msgq_list[*msid].Front = -1;
	msgq_list[*msid].Rear = -1;
	
	free(msgq_list[*msid].Message_Queue);

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
