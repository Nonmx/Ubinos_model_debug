#include <stdio.h>
#include <stdlib.h>
#include "os.h"
#include <assert.h>
#include "readyQ.h"
#include "mylib.h"

extern sleepQ_init();
int main()
{
	/*mutex_pt m1;
	mutex_create(&m1);
	task_create(1);
	sleepQ_init();
	scheduler();
	mutex_lock(m1);
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);

	task_create(2);
	scheduler();
	mutex_lock_timed(m1,2);

	//sleepQ_init();
	scheduler();
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);
	assert(task_state[1] == Running);
	//task_sleep(10);
	//assert(task_state[1] == Blocked);

	scheduler();
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);
	scheduler();
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);
	scheduler();
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);
	scheduler();
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);
	scheduler();
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);
	scheduler();
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);
	//assert(task_state[2] == Running);
	scheduler();
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);
	scheduler();
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);
	scheduler();
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);
	scheduler();// 
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);
	scheduler();
	printf("current_tid %d and T2 prio %d , T1 prio %d \n", current_tid, task_dyn_info[current_tid].dyn_prio, task_dyn_info[1].dyn_prio);
	*/
	mutex_pt m1;
	mutex_create(&m1);

	

	push_task_into_readyQ(1,2,0);
	push_task_into_readyQ(2, 3, 0);
	push_task_into_readyQ(3, 4, 0);
	push_task_into_readyQ(4, 5, 0);

	unsigned char tid;
	unsigned char prio;
	int loc = find_task_readyQ(2, 3);
	//get_task_from_readyQ_position(&tid,&prio,);


	/*sem_pt s1;
	sem_create(&s1);
	task_create(1);
	sleepQ_init();
	scheduler();
	
	printf("current_tid %d\n", current_tid);

	task_create(2);
	scheduler();
	printf("current_tid %d\n", current_tid);
	sem_take_timed(s1, 2);
	scheduler();
	printf("current_tid %d\n", current_tid);
	//mutex_lock_timed(m1, 2);

	//sleepQ_init();
	scheduler();
	printf("current_tid %d\n", current_tid);
	//assert(task_state[1] == Running);
	//task_sleep(10);
	//assert(task_state[1] == Blocked);

	scheduler();
	printf("current_tid %d\n", current_tid);
	scheduler();
	printf("current_tid %d\n", current_tid);
	scheduler();
	printf("current_tid %d\n", current_tid);
	scheduler();
	printf("current_tid %d\n", current_tid);
	scheduler();
	printf("current_tid %d\n", current_tid);
	scheduler();
	printf("current_tid %d\n", current_tid);
	//assert(task_state[2] == Running);
	scheduler();
	printf("current_tid %d\n", current_tid);
	scheduler();
	printf("current_tid %d\n", current_tid);
	scheduler();
	printf("current_tid %d\n", current_tid);
	scheduler();// 
	printf("current_tid %d\n", current_tid);
	scheduler();
	printf("current_tid %d\n", current_tid);

	*/

	//assert(task_state[1] == Running);




}

void running()
{

}