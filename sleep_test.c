#include "os.h"
#include "Sleep_WaitQ.h"
//#include "kernel.c"
#include <assert.h>
unsigned char T_id[NUM_OF_TASKS] = { 1,2,3,4 };
extern unsigned char nondet_uchar();
extern unsigned int nondet_uint();
extern int nondet_int();


int tp_REAR;
int tp_FRONT;
int flag;
unsigned int checker;
int time;

unsigned char temp_tid;
unsigned char temp_prio;
extern void sleepQ_init();
int temp;
int main()
{
	//for (int i = 0; i < 4; i++)
	//{
		//task_static_info[T_id[i]].prio = nondet_uchar();
		//__CPROVER_assume(task_static_info[T_id[i]].prio >= 1 && task_static_info[T_id[i]].prio <= MAX_PRIORITY);

		//task_static_info[T_id[i]].max_act_cnt = 1;
		//task_dyn_info[T_id[i]].dyn_prio = task_static_info[T_id[i]].prio;
		//task_dyn_info[T_id[i]].act_cnt = 0;

		//tp_REAR = REAR;
		//tp_FRONT = FRONT;

		//time = nondet_int();
		//__CPROVER_assume(time >= 500 && time <= 10000);
		sleepQ_init();
		flag = push_sleep_task_into_WQ(Task1, task_dyn_info[Task1].dyn_prio, 1230);
		//flag = push_sleep_task_into_WQ(Task2, task_dyn_info[Task2].dyn_prio, 13100);
		//flag = push_sleep_task_into_WQ(Task3, task_dyn_info[Task3].dyn_prio, 50321);
		//flag = push_sleep_task_into_WQ(Task4, task_dyn_info[Task4].dyn_prio, 110);
		//flag = push_sleep_task_into_WQ(Task4, task_dyn_info[Task4].dyn_prio, 1321);
		//flag = push_sleep_task_into_WQ(Task4, task_dyn_info[Task4].dyn_prio, 1312321);

		sleep_prio_change(1, 10, 0);


		/*assert(flag == 0 ? REAR = tp_REAR + 1 : 1);
		assert(REAR > 1 ? Sleep_Q[FRONT].time <= Sleep_Q[FRONT + 1].time : 1);

		checker = nondet_uint();
		__CPROVER_assume(checker >= 0 && checker <= 1);

		if (checker)
		{
			get_sleep_task_from_WQ(&temp_tid, &temp_prio);
			assert(flag == 0 ? FRONT = tp_FRONT + 1 : 1);
			assert(REAR > 1 ? Sleep_Q[FRONT].time <= Sleep_Q[FRONT + 1].time : 1);
		}*/
	//}
}

void running()
{

}