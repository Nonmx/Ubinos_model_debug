#include "ubinos.h"
task_static_config task_static_info[NUM_OF_TASKS];
task_dynamic_stat task_dyn_info[NUM_OF_TASKS];
unsigned char task_state[NUM_OF_TASKS] = { 0 };
unsigned char Ceiling_Prio[2];



void initialize() {


	task_static_info[1].max_act_cnt = 1;
	task_static_info[1].prio = 1;


	task_static_info[2].max_act_cnt = 1;
	task_static_info[2].prio = 2;

	task_static_info[3].max_act_cnt = 1;
	task_static_info[3].prio = 3;

	task_static_info[4].max_act_cnt = 1;
	task_static_info[4].prio = 3;





	task_dyn_info[1].dyn_prio = task_static_info[1].prio;
	task_dyn_info[1].act_cnt = 0;
	task_dyn_info[2].dyn_prio = task_static_info[2].prio;
	task_dyn_info[2].act_cnt = 0;
	task_dyn_info[3].dyn_prio = task_static_info[3].prio;
	task_dyn_info[3].act_cnt = 0;
	task_dyn_info[4].dyn_prio = task_static_info[4].prio;
	task_dyn_info[4].act_cnt = 0;

}
