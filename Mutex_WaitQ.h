#include "ubinos.h"
#include "config.h"

#ifndef WAITINGQ_H_

#define WAITING_H_

int Find_mutex_Btask(int* task_loc, int,mutex_pt, Mutex *);
int MUTEX_WQ_FULL(mutex_pt,Mutex*);
int MUTEX_WQ_EMPTY(mutex_pt, Mutex*);
int push_mutex_task_into_WQ(unsigned char, unsigned char,mutex_pt,Mutex*);
int get_mutex_task_from_WQ(unsigned char*, unsigned char*,mutex_pt, Mutex*);
void get_mutex_task_from_WQ_position(unsigned char* tid, unsigned char* prio, mutex_pt mutex,Mutex*, int task_loc); //use for lock timed
int mutex_prio_change(unsigned char tid, unsigned char chan_prio, mutex_pt mid, Mutex* mutex, int loc);
#endif