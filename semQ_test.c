#include "os.h"
#include <stdio.h>
#include "kernel.h"
#include "Sem_WaitQ.h"
#include <assert.h>


//extern int get_msgq_task_from_WQ(unsigned char* tid, unsigned char* prio, msgq_pt msid, Msgq* msgq);

Sem* sem;

int loc;
void main()
{
	unsigned char tid;
	unsigned char prio;
	sem = (Sem*)malloc(2 * sizeof(Sem*));
	push_sem_task_into_WQ(Task1, 1, 1, sem);
	push_sem_task_into_WQ(Task2, 2, 1, sem);
	push_sem_task_into_WQ(Task3, 3, 1, sem);
	push_sem_task_into_WQ(Task4, 4, 1, sem);

	//Find_sem_Btask(&loc, 1, 1, sem);
	//sem_prio_change(1, 5, 1, sem, loc);
	//Find_sem_Btask(&loc, 1, 1, sem);
	//get_sem_task_from_WQ_position(&tid, &prio, 1, sem, loc);

	//assert(tid == 1);
	get_sem_task_from_WQ(&tid, &prio,1, sem);
	assert(tid == 4, prio = 4);
	get_sem_task_from_WQ(&tid, &prio, 1, sem);
	assert(tid == 3, prio = 3);
	get_sem_task_from_WQ(&tid, &prio, 1, sem);
	assert(tid == 2, prio = 2);
	get_sem_task_from_WQ(&tid, &prio, 1, sem);
	assert(tid == 1, prio = 1);

	//assert(task_dyn_info[1].dyn_prio == 5);

	//push_mutex_task_into_WQ(Task1, 1, 1, mutex);
}
void running()
{
}