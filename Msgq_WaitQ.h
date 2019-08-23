#include "ubinos.h"
#include "config.h"

#ifndef MSGQ_WAITQ_H_

#define MSGQ_WAITQ_H_

int MSGQ_WQ_FULL(msgq_pt msid);
int MSGQ_WQ_EMPTY(msgq_pt msid);
//int Find_msgq_Btask(int* task_loc, int tid, msgq_pt msid);
void msgqQ_sort(msgq_pt msid);
int push_msgq_task_into_WQ(unsigned char tid, unsigned char p, msgq_pt msid);
int get_msgq_task_from_WQ(unsigned char* tid, unsigned char* prio, msgq_pt msid);
int msgq_prio_change(unsigned char tid, unsigned char chan_prio, msgq_pt msid, int loc);
int get_msgq_task_from_WQ_position(unsigned char* tid, unsigned char* prio, msgq_pt msid, int task_loc);

#endif