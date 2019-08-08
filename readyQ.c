/*
readyQ.c implements functions related to ready queue.
We assume that ready queue is a circular queue, with queue size 10.
MAX QUEUE LENGTH can be changed.
Author: Yoohee
Date: 08/09/2016
*/
//#include <assert.h>
#include <stdio.h>
#include "readyQ.h"
#include "ubinos.h"
#include "mylib.h"
#include <stdlib.h>

unsigned char idx;
unsigned char k = 0;

extern int P;

int front[MAX_PRIORITY + 1];
int rear[MAX_PRIORITY + 1];
unsigned char max_prio = 0;
Queue readyQ[MAX_PRIORITY + 1][MAX_QUEUE_LENGTH]; //this queue keeps the ready list in the order of prority and arrival time
int size[MAX_PRIORITY + 1];
int wholesize;


unsigned char min_activation_order[NUM_OF_TASKS + 1] = { 1,1,1 };
unsigned char max_activation_order[NUM_OF_TASKS + 1];
unsigned char cur_activation_order[NUM_OF_TASKS + 1];

int act_counter[NUM_OF_TASKS + 1];
int num_of_act_task = 0;
extern void eventually(int);


void push_task_into_readyQ(unsigned char t, unsigned char p, int pc)
{
	if (is_full(p)) //queue  is full
	{
		printf("Queue is full!\n");
	}
	else
	{
		task_state[t] = Ready;
		k = rear[p];
		readyQ[p][k].tid = t;
		readyQ[p][k].pc = pc;
		rear[p] = (MAX_QUEUE_LENGTH + k + 1) % MAX_QUEUE_LENGTH;
		readyQ[p][k].activation_order = ++max_activation_order[t];

		size[p]++; //increase size to check out whether it is full or empty.
		wholesize++;

		//reassign max_prio if current prio is bigger.
		if (p > max_prio)
		{
			max_prio = p;
		}
	}
}

void get_task_from_readyQ(unsigned char* t, unsigned char* p)
{
	int i = 0;
	if (is_empty() && !(is_sleeping()))
	{
		printf("Queue is empty\n");
		current_tid = -1;
	}
	else if (is_empty() && is_sleeping())
	{
		*t = 0;
		*p = 0;
	}
	else if (!is_empty())
	{

		*t = readyQ[max_prio][front[max_prio]].tid;
		*p = max_prio;
		cur_activation_order[*t] = readyQ[max_prio][front[max_prio]].activation_order;
		//current_pc[*t] = readyQ[max_prio][front[max_prio]].pc;
		//truncate popped index
		readyQ[max_prio][front[max_prio]].tid = -1;
		readyQ[max_prio][front[max_prio]].pc = -1;
		//redefine front variable
		front[max_prio] = (front[max_prio] + 1) % MAX_QUEUE_LENGTH;
		size[*p]--;
		wholesize--;

		//re-calculate max_prio.
		max_prio = *p;
		while (!size[max_prio] && max_prio != 0)
		{
			max_prio--;
		}

		//printf("max_prio : %d\n\n", max_prio);
		task_state[*t] = Running;
		//printf("task_state[*t] -> %d\n\n", task_state[*t]);
		//if (task_state[*t][act_counter[*t]] == Running)
			//act_counter[*t]++;
	}

}

int temp_rear;

void get_task_from_readyQ_position(unsigned char* t, unsigned char* p, mutex_pt *mutex,int loc)
{
	int i = 0;
	if (is_empty() && !(is_sleeping()))
	{
		printf("Queue is empty\n");
		current_tid = -1;
	}
	else if (!is_empty())
	{

		*t = readyQ[task_dyn_info[mutex->owner].dyn_prio][loc].tid;
		*p = task_dyn_info[mutex->owner].dyn_prio;
		cur_activation_order[*t] = readyQ[max_prio][front[max_prio]].activation_order;
		//current_pc[*t] = readyQ[max_prio][front[max_prio]].pc;
		//truncate popped index
		readyQ[task_dyn_info[mutex->owner].dyn_prio][loc].tid = -1;
		readyQ[task_dyn_info[mutex->owner].dyn_prio][loc].pc = -1;


		temp_rear = rear[mutex->owner];

		if (front[mutex->owner] == loc)
		{
			front[mutex->owner] = (front[mutex->owner] + 1) % MAX_QUEUE_LENGTH;
		}
		else if (loc < temp_rear)//task가 waitQ의 중간에 꺼내면 이 task가 뒤에 있는 task들이 다 앞으로 다시 push 해야 한다.
		{
			if (temp_rear == 0)
				temp_rear = MAX_QUEUE_LENGTH; //temp_Rear는 0이면 1를 빼면 lowerbound 가능성이 있으니까 temp_Rear= 15로 하고 1빼면 14이기 때문에 0 칸 앞에 것은 딱 맞아.
			while ((temp_rear - 1) != rear[mutex->owner]) { //모든 task가 다 1칸 앞에서 push 하면 최종적으로 Rear는 1만큼을 감소한다.
				rear[mutex->owner] = loc;
				loc++;
				push_task_into_readyQ(readyQ[mutex->owner][loc].tid, task_dyn_info[readyQ[mutex->owner][loc].tid].dyn_prio,current_pc[readyQ[mutex->owner][loc].tid]);
			}
		}

		//redefine front variable
		//front[task_dyn_info[mutex->owner].dyn_prio] = (front[task_dyn_info[mutex->owner].dyn_prio] + 1) % MAX_QUEUE_LENGTH;
		size[*p]--;
		wholesize--;
	}

}


void push_task_into_readyQ_position(unsigned char t, unsigned char p, int pc,mutex_pt* mutex,int loc)
{
	if (is_full(p)) //queue  is full
	{
		printf("Queue is full!\n");
	}
	else
	{
		readyQ[task_dyn_info[mutex->owner].dyn_prio][loc].tid = t;
		readyQ[task_dyn_info[mutex->owner].dyn_prio][loc].pc = pc;
		//rear[p] = (MAX_QUEUE_LENGTH + k + 1) % MAX_QUEUE_LENGTH;
		//readyQ[p][k].activation_order = ++max_activation_order[t];

		size[p]++; //increase size to check out whether it is full or empty.
		wholesize++;

		//reassign max_prio if current prio is bigger.
		if (p > max_prio)
		{
			max_prio = p;
		}
	}
}

extern signed char current_tid;
extern unsigned char current_prio;

int scheduler() {


	if (is_idle() || api_suporter == API_TerminateTask || api_suporter == API_mutex_lock || api_suporter == API_sem_take)
	{
		api_suporter = -1;
		get_task_from_readyQ(&current_tid, &current_prio);
		if (current_tid == -1)
			return -1;
		else
			return 0;
	}
	else
	{
		if (task_dyn_info[current_tid].dyn_prio < max_prio)
		{
			push_task_into_readyQ(current_tid, task_dyn_info[current_tid].dyn_prio, current_pc[current_tid]);
			get_task_from_readyQ(&current_tid, &current_prio);
			if (current_tid == -1)
				return -1;
			else
				return 0;
		}

		else
		{
			if (Hava_to_RR())
			{
				push_task_into_readyQ(current_tid, task_dyn_info[current_tid].dyn_prio, current_pc[current_tid]);
				get_task_from_readyQ(&current_tid, &current_prio);
				if (current_tid == -1)
					return -1;
				else
					return 0;
			}
			else
				return -1;
		}
	}
}



/*int Round_Robin_Schedule()
{
	if (Hava_to_RR())
	{
		push_task_into_readyQ(current_tid, task_dyn_info[current_tid].dyn_prio, current_pc[current_tid], PREEMPT);
		get_task_from_readyQ(&current_tid, &current_prio);
		if (current_tid == -1)
			return 0;
		else
			return 1;
	}
	else
		return 0;
}*/

