#include "ubinos.h"
#include "config.h"

#ifndef MSGQ_WAITQ_H_

#define MSGQ_WAITQ_H_

int MSGQ_WQ_FULL(msgq_pt* msgq, unsigned char p);
int MSGQ_WQ_EMPTY(msgq_pt* msgq);
void push_msgq_task_into_WQ(unsigned char, unsigned char, msgq_pt*);
void get_msgq_task_from_WQ(unsigned char*, unsigned char*, msgq_pt*);

#endif