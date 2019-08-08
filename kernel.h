/*
kernel.h 
Mapping from OSEK OS model

Date:05/20/2019
Author : Yang Song

*/

#include "config.h"







unsigned char ecode;
unsigned char error[NUM_OF_TASKS];

extern int scheduler();

int task_create(unsigned char);
//mapping from ActivateTASK

int task_suspend(unsigned char);
//mapping from TerminateTask();

int TerminateTask();

//part of task_sleep
int task_sleep(double);
//int task_wakeup(unsigned char, double);
int time_checker(unsigned char);


//void RR(unsigned char); //round robin scheduler

//part of mutex

int mutex_create(mutex_pt*);
int mutex_delete(mutex_pt*);
int mutex_lock(mutex_pt*);
int mutex_unlock(mutex_pt*);
int mutex_islocked(mutex_pt*);
int mutex_lock_timed(mutex_pt*, unsigned int);
int mutex_time_checker(mutex_pt*, unsigned char);

//part of semaphore
int sem_create(sem_pt*);
int sem_delete(sem_pt*);
int sem_take(sem_pt*);
int sem_give(sem_pt*);
int sem_take_timed(sem_pt*, unsigned int);

//part of messahe Q
int msgq_create(msgq_pt*, unsigned int, unsigned int);
int msgq_receive(msgq_pt*, unsigned char*);
int msgq_send(msgq_pt*, unsigned char*);
int msgq_delete(msgq_pt*);


extern int os_on;
extern const int ON;
extern const int OFF;
void ubik_comp_start();
void ShutDownOS();
void running();
void initialize();
//mapping from kernel.h
