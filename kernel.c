/*
kernel.c includes implementation of system services(APIs) of Ubinos.

Author: YangSong
Date: 08/01/2019
*/
#include <stdio.h>
#include "ubinos.h"
#include "kernel.h"
#include "readyQ.h"
#include "mylib.h"
#include "waitingQ.h"
#include "messageQ.h"

//part of task_sleep


extern void eventually(int);

extern unsigned char max_activation_order[NUM_OF_TASKS + 1];
extern unsigned char cur_activation_order[NUM_OF_TASKS + 1];
API api;
extern signed char current_tid;
unsigned char current_prio;
//unsigned char e_code;
extern int act_counter[NUM_OF_TASKS + 1];
int task_create(unsigned char reftask)
{
	api = API_task_create;

	if (task_static_info[reftask].max_act_cnt == 0)
		initialize();

	//check whether max activation count has been reached
	if (reftask < 0 || reftask > NUM_OF_TASKS)
	{
		e_code = E_OS_ID;
		return 0;
	}
	else if (task_dyn_info[reftask].act_cnt < task_static_info[reftask].max_act_cnt)
	{
		task_dyn_info[reftask].act_cnt++;
		push_task_into_readyQ(reftask, task_static_info[reftask].prio, 0, 0);
	}
	else
	{
		e_code = E_OS_LIMIT;

		return 0;
	}
	/* background task of Erika? */
	if (current_tid == -1) {
		current_tid = reftask;
		return reschedule(API_task_create, current_tid);
	}
	return reschedule(API_task_create, current_tid);
}

int TerminateTask()
{
	api = API_TerminateTask;
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
	else
	{
		task_state[current_tid] = Suspended;
	}
	min_activation_order[current_tid] = cur_activation_order[current_tid] + 1;
	return reschedule(API_TerminateTask, current_tid);
}

int task_suspend(unsigned char reftask)// implement later
{
	return 0;
}




unsigned char task_tid;
//double task_time;
int task_sleep(double time)
{

	if (task_state[current_tid] == Running)
	{
		api = API_task_sleep;
		task_state[current_tid] = Blocked;
		//printf("task_state[%d] = %d", current_tid, task_state[current_tid]);
		return(reschedule(API_task_sleep, current_tid));

		//return current_tid;
	}
	else if (task_state[current_tid] == Suspended)
	{
		return 0; //
	}

}




int time_checker(unsigned char tid)//Wake up the task
{
	if (task_state[tid] == Blocked)
	{

		task_state[tid] = Ready;
		push_task_into_readyQ(tid, task_static_info[tid].prio, 0, 0);


		//If this task has a higher priority than the current running task
		//push_task_into_readyQ(current_tid, task_static_info[current_tid].prio, 0, PREEMPT);

		if ((reschedule(API_task_create, current_tid)) != 0)//priority based
		{
			return 1;

		}
		else if (Round_Robin_Schedule())//round robin
		{
			return 1;
		}
		else
		{
			return 0; //Not up to the appointed time
		}
	}
	else
	{
		return 0;
	}
}


int mutex_create(mutex_pt* mutex)
{
	mutex->flag = 0; // mutex 생성
	mutex->owner = 0;
	mutex->lock_counter = 0;
	mutex->tra_flag = 0;

	return 1;
}

unsigned char Before_temp;
unsigned char After_temp;
unsigned char temp_tid;
unsigned char temp_prio;


int mutex_lock(mutex_pt* mutex)
{
	mutex->lock_call[current_tid] = 1;
	if (mutex->flag == 0 && mutex->owner == 0)//first time 
	{
		mutex->flag = 1;
		mutex->owner = current_tid;
		mutex->lock_counter = 1;
		//mutex[0].call[current_tid]++;

		return 1;
	}
	else if (mutex->lock_counter >= 1 && mutex->owner == current_tid)// mutex 여려번 잠금할 수 있다.
	{
		mutex->lock_counter++;
		return 1;
	}
	else
	{
		if (task_static_info[mutex->owner].prio < task_static_info[current_tid].prio)
		{
			mutex->tra_flag = 1;
			After_temp = current_prio;
			Before_temp = task_static_info[mutex->owner].prio; // unlock

			task_state[current_tid] = Blocked;
			push_task_into_WQ(current_tid, current_prio); // 수행중인 task가 lock 될 수 없으면 waitQ로 push 한다
			int loc = 0;
			for (loc = front[task_dyn_info[mutex->owner].dyn_prio]; loc < rear[task_dyn_info[mutex->owner].dyn_prio]; loc++)
			{
				//mutex 가지고 있는 task 꺼낼 때 까지 
				get_task_from_readyQ_position(&temp_tid, &temp_prio, mutex);
				if (temp_tid == mutex->owner)
				{
					task_dyn_info[temp_tid].dyn_prio = After_temp; // priorit inherit
					push_task_into_readyQ(temp_tid, task_dyn_info[temp_tid].dyn_prio, current_pc[temp_tid], PREEMPT); // mutex가지고 있는 낮은 priority인 task가 먼저 readyQ에서 꺼내서 priority 상속한 다음에 readyQ로 다시 push 한다
					break;
				}
				else
				{
					push_task_into_readyQ(temp_tid, task_dyn_info[temp_tid].dyn_prio, current_pc[temp_tid], PREEMPT);
				}
			}
			return reschedule(API_mutex_lock, current_tid);


		}
		else
		{
			task_state[current_tid] = Blocked;
			push_task_into_WQ(current_tid, current_prio);
			return reschedule(API_mutex_lock, current_prio);
		}
	}
}


int mutex_unlock(mutex_pt* mutex)
{
	if (mutex->flag == 1 && mutex->lock_counter == 1 && mutex->lock_call[current_tid] > 0)
	{
		if (mutex->tra_flag > 0) // priority 북구
		{
			mutex->tra_flag = 0;
			task_dyn_info[current_tid].dyn_prio = task_static_info[current_tid].prio;
			push_task_into_readyQ(current_tid, task_dyn_info[current_tid].dyn_prio, current_pc[current_tid], PREEMPT);


		}
		mutex->flag = 0;
		mutex->owner = 0;
		mutex->lock_counter = 0;
		mutex->lock_call[current_tid] = 0;
		get_task_from_WQ(&temp_tid, &temp_prio); // block중인 tid 흭득
		if (is_sleeping() && mutex->lock_call[temp_tid] > 0) // 지금 waitQ에서(block) task가 있으면 그리고 해당task 이미 mutex_lock 호출하면 
		{
			//block된 task 수행될 때 mutex 가지고 있는 상태로 resume


			task_state[temp_tid] = Ready;
			mutex->flag = 1;
			mutex->owner = temp_tid;
			mutex->lock_counter = 1;
			push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_tid], PREEMPT);//readyQ로 추가해서 바로 실행하지 않는다.
			return reschedule(API_mutex_unlock, current_tid); // 높은 priority 인 TASK가 있으면 preempt 해야 한다.

		}

		return 1;
	}
	else if (mutex->lock_counter > 1)
	{
		mutex->lock_counter--;
		return 1;
	}
	else
	{
		return 0;
	}
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
	return 1;
}




int mutex_time_checker(mutex_pt* mutex, unsigned char tid)//
{

	if (mutex->lock_call[tid] == 99 && mutex->flag == 0 && mutex->owner == 0)
	{
		mutex->flag = 1;
		mutex->owner = tid;
		mutex->lock_counter = 1;
		return 1;
	}
	else if (mutex->lock_call[tid] == 99 && mutex->flag >= 1 && mutex->owner == tid)
	{
		mutex->lock_counter++;
		return 1;
	}
	else if (mutex->lock_call[tid] == 99 && !(mutex->owner == tid))
	{
		return 0;
	}
}


int mutex_lock_timed(mutex_pt* mutex, unsigned int time)
{
	mutex->lock_call[current_tid] = 99;

	return 1;
}




int sem_create(sem_pt* sem)
{
	sem->counter = 0;
	return 1;
}

int sem_delete(sem_pt* sem)
{
	sem->counter = -1;
	return 1;
}

int sem_give(sem_pt* sem)
{
	if (!(empty()))
	{
		get_task_from_WQ(&temp_tid, &temp_prio);
		push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_tid], PREEMPT);
		return reschedule(API_sem_give, current_tid);
		//	return 1;// 높은 priority task 있으면 바로 수행
	//	else return 0;//높은 priority 없으면 Round robin 발생가능성이 있다.
	}
	else
	{
		sem->counter = sem->counter + 1;
		return 1;
	}
}


int sem_take(sem_pt* sem)
{
	if (sem->counter > 0)
	{
		sem->counter = sem->counter - 1;
		return 1;

	}
	else
	{
		push_task_into_WQ(current_tid, current_prio);
		return(reschedule(API_sem_take, current_tid));
	}
}

int sem_take_timed(sem_pt* sem, unsigned int timed)
{
	sem->lock_call[current_tid] = 99; // 어느 태스크가 sem_task_time API 호출하는지 기록
}

int sem_time_checker(sem_pt* sem, unsigned char tid)
{
	if (sem->lock_call[tid] == 99)
	{
		if (sem->counter > 0)
		{
			sem->counter = sem->counter - 1;
			return 1;
		}
		else
		{
			push_task_into_WQ(tid, task_dyn_info[tid].dyn_prio);
			return (reschedule(API_sem_take, tid));
		}
	}
}


//message Q
int msgq_create(msgq_pt* msgq_p, unsigned int msgsize, unsigned int maxcount) //for noting
{
	//message static적으로 구현
	msgq_p->flag = 1; //created scuessful
	return 1;
}



int msgq_send(msgq_pt* msgq_p, unsigned char* message)
{
	if (!(empty()) && msgq_p->flag == 1)
	{
		get_task_from_WQ(&temp_tid, &temp_prio);
		push_task_into_readyQ(temp_tid, temp_prio, current_pc[temp_prio], PREEMPT);
		return reschedule(API_msgq_send, current_tid);
	}
	else if (msgq_p->flag == 1)
	{
		push_message_into_MQ(msgq_p, message);
		return 1;

	}
	else return 0;
}

int msgq_receive(msgq_pt* msgq_p, unsigned char* message)
{
	if (!(MQ_empty()) && msgq_p->flag == 1)
	{
		get_message_from_MQ(msgq_p, message);
		return 1;
	}
	else if (msgq_p->flag == 1)
	{
		push_task_into_WQ(current_tid, current_prio);
		return(reschedule(API_msgq_receive, current_tid));
	}
	else return 0;
}

int os_on;
const int ON = 1;
const int OFF = 0;

void ubik_comp_start()
{
	if (os_on == OFF)
	{
		os_on = ON;
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
