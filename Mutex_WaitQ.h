#include "ubinos.h"
#include "config.h"

#ifndef WAITINGQ_H_

#define WAITING_H_

int Find_mutex_Btask(int* pri_loc, int* task_loc, mutex_pt* mutex);
int MUTEX_WQ_FULL(mutex_pt* mutex, unsigned char p);
int MUTEX_WQ_EMPTY(mutex_pt* mutex);
void push_mutex_task_into_WQ(unsigned char, unsigned char,mutex_pt*);
void get_mutex_task_from_WQ(unsigned char*, unsigned char*,mutex_pt*);
void get_mutex_task_from_WQ_position(unsigned char* tid, unsigned char* prio, mutex_pt* mutex, int pri_loc, int task_loc); //use for lock timed

#endif