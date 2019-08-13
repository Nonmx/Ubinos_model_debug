#include "os.h"
#include <stdio.h>
#include "kernel.h"
#include "Msgq_WaitQ.h"
#include <assert.h>


//extern int get_msgq_task_from_WQ(unsigned char* tid, unsigned char* prio, msgq_pt msid, Msgq* msgq);

Msgq* msgq;

int loc;
void main()
{
	unsigned char tid;
	unsigned char prio;
	msgq = (Msgq*)malloc(2 * sizeof(Msgq*));
	push_msgq_task_into_WQ(Task1, 1, 1, msgq);
	push_msgq_task_into_WQ(Task2, 2, 1, msgq);
	push_msgq_task_into_WQ(Task3, 3, 1, msgq);
	push_msgq_task_into_WQ(Task4, 4, 1, msgq);

	Find_msgq_Btask(&loc, 1, 1, msgq);
	msgq_prio_change(1, 5, 1, msgq, loc);
	Find_msgq_Btask(&loc, 1, 1, msgq);
	get_msgq_task_from_WQ_position(&tid, &prio, 1, msgq, loc);

	assert(tid == 1);
	/*get_msgq_task_from_WQ(&tid, &prio, 1, msgq);
	assert(tid == 4, prio = 4);
	get_msgq_task_from_WQ(&tid, &prio, 1, msgq);
	assert(tid == 3, prio = 3);
	get_msgq_task_from_WQ(&tid, &prio, 1, msgq);
	assert(tid == 2, prio = 2);
	get_msgq_task_from_WQ(&tid, &prio, 1, msgq);
	assert(tid == 1, prio = 1);*/

	assert(task_dyn_info[1].dyn_prio == 5);

	//push_mutex_task_into_WQ(Task1, 1, 1, mutex);
}
void running()
{
}