#include "os.h"
#include <stdio.h>
#include "kernel.h"
#include "Mutex_WaitQ.h"
#include <assert.h>


//extern int get_mutex_task_from_WQ(unsigned char* tid, unsigned char* prio, mutex_pt msid, Msgq* mutex);

Mutex* mutex;

int loc;
void main()
{
	unsigned char tid;
	unsigned char prio;
	mutex = (Mutex*)malloc(2 * sizeof(Mutex*));
	push_mutex_task_into_WQ(Task1, 1, 1, mutex);
	push_mutex_task_into_WQ(Task2, 2, 1, mutex);
	push_mutex_task_into_WQ(Task3, 3, 1, mutex);
	push_mutex_task_into_WQ(Task4, 4, 1, mutex);

	get_mutex_task_from_WQ(&tid, &prio, 1, mutex);
	assert(tid == 4, prio = 4);
	get_mutex_task_from_WQ(&tid, &prio, 1, mutex);
	assert(tid == 3, prio = 3);
	get_mutex_task_from_WQ(&tid, &prio, 1, mutex);
	assert(tid == 2, prio = 2);
	get_mutex_task_from_WQ(&tid, &prio, 1, mutex);
	assert(tid == 1, prio = 1);



	//Find_mutex_Btask(&loc,1,1,mutex);
	//mutex_prio_change(1, 5, 1, mutex, loc);
	//Find_mutex_Btask(&loc, 1, 1, mutex);
	//get_mutex_task_from_WQ_position(&tid,&prio,1,mutex,loc);

	//assert(tid == 1);
	
	//assert(task_dyn_info[1].dyn_prio == 5);

	//push_mutex_task_into_WQ(Task1, 1, 1, mutex);
}
void running()
{
}