#include "ubinos.h"
#include "config.h"

#ifndef SEM_WAITQ_H_

#define SEM_WAITQ_H_

int SEM_WQ_FULL(sem_pt* sem, unsigned char p);
int SEM_WQ_EMPTY(sem_pt* sem);
void push_sem_task_into_WQ(unsigned char, unsigned char, sem_pt*);
void get_sem_task_from_WQ(unsigned char*, unsigned char*, sem_pt*);

#endif