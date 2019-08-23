/*
author: yangsong
*/


#ifndef UBINOS_H_
#define UBINOS_H_

/* get configuration from config.h file */
#include "config.h"
#include <stdio.h>
//#include <windows.h>
/* define ERROR codes -- from OSEK/VDX OS specification */
#define E_OK 0
#define E_OS_ACCESS 1
#define E_OS_CALLLEVEL 2
#define E_OS_ID 3
#define E_OS_LIMIT 4
#define E_OS_NOFUNC 5
#define E_OS_RESOURCE 6
#define E_OS_STATE 7
#define E_OS_VALUE 8
#define E_OS_DISABLEDINT 9

/* define ERROR code -- additional ones for managing ready queue. */
#define E_OVER_QUEUE_SIZE 21
#define E_QUEUE_MANAGE 22
#define TEST 23
//ubinos?????????

/* Task states */
typedef enum state {
	Suspended, Ready, Running, Blocked
}State;

typedef enum API {
	API_task_create,
	API_task_suspend,
	API_TerminateTask,
	API_task_resume,
	API_task_sleep,
	API_sem_take,
	API_msgq_receive,
	API_mutex_lock,
	API_mutex_lock_timed,
	API_sem_take_timed,
	API_msgq_send,
	API_sem_give,
	API_mutex_unlock
}API;



extern API api;
extern API api_name;

typedef enum Mess {
	PB_KEY_CHANGE_EVT,
	PB_BATT_EVT,
	PB_TIME_EVT,
	PB_BLE_EVT,
	PB_AFE_EVT,
	PB_POPUP_EVT,
	PB_ALARM_EVT,
	PB_PNIP_EVT,
	PB_ACTIVITY_EVT,
	PB_GLUCOSE_EVT,
	PB_PAAR_TAG_EVT,
	PB_PEDOMETER_EVT,
	PB_SMART_CART_TALK_EVT,
	PB_TEST_MSG_EVT,
	PB_QC_EVT,
	PB_BLE_CONTROL_EVT,
	PB_SMART_CAR_TALK_EVT,
	PB_LF_EVT,
	PB_UI_REFRESH_EVT,
	PB_WDT_REFRESH_EVT
}Mess;

Mess MESS;

/* Data structure that store initial configurations */
typedef struct
{
	unsigned char prio;
	unsigned char max_act_cnt;
	//unsigned char preemptable;
}task_static_config;

/* DS to store dynamic state information */


typedef struct {
	char* buf;
}Task_buf;


typedef struct {
	unsigned char act_cnt;
	unsigned char dyn_prio;
	unsigned char preemptable;
	unsigned int In_ReadyQ;
	unsigned int In_MutexQ;
	unsigned int Mutex_Id;
	unsigned int In_SemQ;
	unsigned int Sem_Id;
	unsigned int In_MsgqQ;
	unsigned int Msgq_Id;
	unsigned int In_SleepQ;

	Task_buf task_message[NUM_OF_TASKS];


}task_dynamic_stat;


typedef struct
{
	unsigned char tid;
}WQ;

typedef struct {
	int flag;// -1 = mutex 없다, 0 = unlocked, 1 = locked
	char owner;//locked 가지고 있는  task
	int lock_counter;
	int lock_call[NUM_OF_TASKS+1];
	int prio_inheri;
	int signal;
	int block_flag;
	int timed_flag;

	int mutex_timed_info[NUM_OF_TASKS+1];
	int mutex_timed_flag[NUM_OF_TASKS+1];

	//mutex waitQ
	int Front;
	int Rear;
	WQ mutexQ[NUM_OF_TASKS+1];
}Mutex;



typedef int mutex_pt;


typedef struct {
	int counter;
	//unsigned int lock_call[NUM_OF_TASKS];
	int sem_timed_info[NUM_OF_TASKS+1];
	//unsigned char Lock; //locked?? task ????
	int sem_timed_flag[NUM_OF_TASKS+1];
	//sem waitQ

	int Front;
	int Rear;
	WQ semQ[NUM_OF_TASKS+1];

}Sem;
typedef int sem_pt;


typedef struct {
	unsigned char *message;
}MQ;



typedef struct {
	int maxcounter;
	int counter;
	int msgsize;
	
	int Front;
	int Rear;
	WQ msgqQ[NUM_OF_TASKS+1];
	

	int F;
	int R;
	MQ *Message_Queue;
	
}Msgq;


typedef int msgq_pt;


typedef struct
{
	unsigned char tid;
	//unsigned char prio;
	int time;
}SLEEP;

SLEEP Sleep_Q[NUM_OF_TASKS+1];

/*typedef struct{
	unsigned char prio;
	unsigned char tid;

}Current;*/ //current information

extern unsigned char current_prio;
extern signed char current_tid;

//Current current[NUM_OF_TASKS];
extern int current_pc[NUM_OF_TASKS + 1];

extern unsigned char e_code;

/*
Arrays to save attributes.
They are actually declared in osek.h
*/
extern task_static_config task_static_info[NUM_OF_TASKS]; //static configuration of tasks
extern task_dynamic_stat task_dyn_info[NUM_OF_TASKS]; //dynamic state of tasks
extern unsigned char task_state[NUM_OF_TASKS]; //state of tasks(??)
extern int PC[NUM_OF_TASKS]; //Program Counter
/* inline functions to get current information of the model. (allocation, event set/clear) */


extern void initialize();
extern void running();
extern void startup_process(unsigned char min, unsigned char max);


//Time-dependent definitions



#endif
