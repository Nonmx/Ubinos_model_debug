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


extern void eventually(int);

extern unsigned char max_activation_order[NUM_OF_TASKS + 1];
extern unsigned char cur_activation_order[NUM_OF_TASKS + 1];
extern unsigned char sleep_info[NUM_OF_TASKS + 1] = {-1};
extern unsigned char mutex_timed_info[NUM_OF_TASKS+1] = { -1 };
extern unsigned char sem_timed_info[NUM_OF_TASKS+1] = { -1 };
API api;
API api_suporter;
extern signed char current_tid;
unsigned char current_prio;
//unsigned char e_code;
extern int act_counter[NUM_OF_TASKS + 1];

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

int task_sleep(double time)
{

	if (task_state[current_tid] == Running)
	{
		api = API_task_sleep;
		API_Call_Suporter(API_task_sleep);
		sleep_info[current_tid] = time+1; //시간 tick 초기화는 -1이기 때문에 
		task_state[current_tid] = Blocked;
		push_sleep_task_into_WQ(current_tid, current_prio);
		return 0;

		//return current_tid;
	}
	else if (task_state[current_tid] == Suspended || task_state[current_tid]==Blocked)
	{
		return -1; //
	}

}


int multi_time_checker()//Wake up the task
{
	for (int i = 0; i < NUM_OF_TASKS; i++)
	{
		if (sleep_info[i] > 0)
		{
			sleep_info[i]--; //1 tick 
			return -1;//keep going
		}
		if (sleep_info[i] == 0)
		{
			return i;//tid
		}

		if (mutex_timed_info[i] > 0)
		{
			sleep_info[i]--;
			return -1;//keep going
		}
		if (mutex_timed_info[i] == 0)
		{
			//check task state
		}

		if (sem_timed_info[i] > 0)
		{
			sleep_info[i]--;
			return -1;
		}
		if (sem_timed_info[i] == 0)
		{
			
		}



	}
	
}


void mutex_mem_alloc(mutex_pt* mutex)
{
	mutex = (mutex_pt*)malloc(sizeof(mutex_pt));
}

int mutex_create(mutex_pt* mutex)
{
	
	mutex_mem_alloc(mutex);
	if (mutex == NULL)
	{
		return -1;
	}
	else
	{
		mutex->flag = 0; // mutex 생성
		mutex->owner = 0;
		mutex->lock_counter = 0;
		mutex->tra_flag = 0;
		mutex->signal = 0;
		mutex->block_flag = 0;
		return 0;
	}
	
}

unsigned char Before_temp;
unsigned char After_temp;
unsigned char temp_tid;
unsigned char temp_prio;


int loc;//task location
int mutex_lock(mutex_pt* mutex)
{
	API_Call_Suporter(API_mutex_lock);

	mutex->lock_call[current_tid] = 1;
	if (mutex->flag == 0 && mutex->owner == 0)//first time 
	{
		mutex->flag = 1;
		mutex->owner = current_tid;
		mutex_owner = mutex->owner;
		mutex->lock_counter = 1;
		//mutex[0].call[current_tid]++;

		return 2;//succeed, no scheduling
	}
	else if (mutex->lock_counter >= 1 && mutex->owner == current_tid)// mutex 여려번 잠금할 수 있다.
	{
		mutex->lock_counter++;
		return 2;//succeed, no scheduling
	}
	else
	{
		if (task_static_info[mutex->owner].prio < task_static_info[current_tid].prio)
		{
			mutex->tra_flag++;
			After_temp = current_prio;//after inheritance
			Before_temp = task_static_info[mutex->owner].prio; // before inheritance


			task_state[current_tid] = Blocked;
			mutex->block_flag++;
			push_mutex_task_into_WQ(current_tid, current_prio,mutex); // 수행중인 task가 lock 될 수 없으면 waitQ로 push 한다
			loc = find_task(mutex->owner,task_static_info[mutex->owner].prio);
			//get the task of mutex locking
			if (loc == 0) //If task with mutex is not in readyQ 
			{

				get_mutex_task_from_WQ(&temp_tid, &temp_prio, mutex); // block중인 tid 흭득
				assert(mutex->owner == temp_tid);
				task_dyn_info[temp_tid].dyn_prio = task_static_info[current_tid].prio; //priority inheritance
				push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_tid]);
				return 0; // schueduling or non-scheduleing User decision
			}
			get_task_from_readyQ_position(&temp_tid, &temp_prio, mutex,loc);
			task_dyn_info[temp_tid].dyn_prio = task_static_info[current_tid].prio;
			push_task_into_readyQ(temp_tid, task_dyn_info[temp_tid].dyn_prio, current_pc[temp_tid]);
			assert(mutex->owner == temp_tid);
			return 0;
		}
		else
		{
			mutex->block_flag++;
			task_state[current_tid] = Blocked;
			push_mutex_task_into_WQ(current_tid, current_prio,Mutex);
			return 0;
		}
	}
}


int mutex_unlock(mutex_pt* mutex)
{
	API_Call_Suporter(API_mutex_unlock);
	if (mutex->flag == 1 && mutex->lock_counter == 1 && mutex->lock_call[current_tid] > 0)
	{
		if (mutex->tra_flag >0) // priority 북구
		{
			mutex->tra_flag --;
			task_dyn_info[current_tid].dyn_prio = task_static_info[current_tid].prio;

		}
		mutex->flag = 0;
		mutex->owner = 0;
		mutex->lock_counter = 0;
		mutex->lock_call[current_tid] = 0;
		mutex_owner = mutex->owner;
		
		if (mutex->block_flag > 0) // 지금 waitQ에서(block) task가 있으면 그리고 해당task 이미 mutex_lock 호출하면 
		{
			//block된 task 수행될 때 mutex 가지고 있는 상태로 resume


			get_mutex_task_from_WQ(&temp_tid, &temp_prio,mutex); // block중인 tid 흭득
			mutex->block_flag--;
			task_state[temp_tid] = Ready;
			mutex->flag = 1;
			mutex->owner = temp_tid;
			mutex_owner = mutex->owner;
			mutex->lock_counter++;
			push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_tid]);//readyQ로 추가해서 바로 실행하지 않는다.
			return 0; 

		}

		return 2;//non scheduling
	}
	else if (mutex->lock_counter > 1)
	{
		mutex->lock_counter--;
		return 2;//no schuedling
	}
	else
		return -1;
}

int mutex_islocked(mutex_pt* mutex)
{
	if (mutex->flag == 1)
		return 1;//locked
	else
		return 0;//unlocked
}

int mutex_delete(mutex_pt* mutex)
{
	mutex->flag = -1;
	mutex->lock_counter = -1;
	mutex->tra_flag = -1;
	mutex->owner = -1;
	mutex->signal = -1;
	mutex->block_flag = -1;

	mutex = NULL;
	free(mutex);
	return 0;
}





int mutex_lock_timed(mutex_pt* mutex, unsigned int time)
{
	mutex_timed_info[current_tid] = time;
	return mutex_lock(mutex);

}


void sem_mem_alloc(sem_pt* sem)
{
	sem = (sem_pt*)malloc(sizeof(sem_pt));
}

int sem_create(sem_pt* sem)
{
	sem_mem_alloc(sem);
	sem->counter = 0;

	return 0;
}


int sem_delete(sem_pt* sem)
{
	sem->counter = -1;
	sem = NULL;
	free(sem);
	return 0;
}

int sem_give(sem_pt* sem)
{
	API_Call_Suporter(API_sem_give);
	if (!(SEM_WQ_EMPTY(sem)))
	{
		get_sem_task_from_WQ(&temp_tid, &temp_prio,sem);
		push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_tid]);
		return 0;
	}
	else
	{
		sem->counter = sem->counter + 1;
		return 2;//no scheduling
	}
}


int sem_take(sem_pt* sem)
{
	API_Call_Suporter(API_sem_take);
	if (sem->counter > 0)
	{
		sem->counter = sem->counter - 1;
		return 2;//no scheduling

	}
	else
	{
		push_sem_task_into_WQ(current_tid, current_prio,sem);
		return 0;
	}
}



int sem_take_timed(sem_pt* sem, unsigned int timed)
{
	sem_timed_info[current_tid] = timed;
	return sem_take(sem);
}



//message Q

void msgq_mem_alloc(msgq_pt* msgq_p) {
	msgq_p = (msgq_pt*)malloc(sizeof(msgq_pt));

}

int msgq_create(msgq_pt* msgq_p) //for noting
{
	//message static적으로 구현
	msgq_mem_alloc(msgq_p); 
	return 0;
}


int msgq_send(msgq_pt* msgq_p, unsigned char* message)
{
	if (())
	{
		get_msgq_task_from_WQ(&temp_tid, &temp_prio,msgq_p);
		push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_prio]); 
		return 0;
	}
	else
	{
		push_message_into_MQ(msgq_p, message);
		return 2;//no scheduling

	}

}

int msgq_receive(msgq_pt* msgq_p, unsigned char* message)
{
	if (!(MQ_empty()) && msgq_p->flag == 1)
	{
		get_message_from_MQ(msgq_p, message);
		return 2;//no schuedling
	}
	else if (msgq_p->flag == 1)
	{
		push_task_into_WQ(current_tid, current_prio);
		return(reschedule(API_msgq_receive, current_tid));
	}
	else return -1;//msgq create하지 않는다
}

int msgq_delete(msgq_pt* msgq_p)
{
	msgq_p->flag = -1;
	msgq_p = NULL;
	free(msgq_p);
	
	return 1;
}

int os_on;
const int ON = 1;
const int OFF = 0;

void ubik_comp_start()
{
	if (os_on == OFF)
	{
		os_on = ON;
		scheduler();
		running(); //call tasks here
	}
}

void ShutDownOS()
{
	if (os_on == ON)
	{
		os_on = OFF;
	}
}
