#include "ubinos.h"



extern int REAR;
extern int FRONT;

int push_sleep_task_into_WQ(unsigned char,unsigned char,int);
int get_sleep_task_from_WQ(unsigned char*, unsigned char*);
int get_sleep_task_from_WQ_position(unsigned char* tid, unsigned char* prio, int loc);
int sleep_prio_change(unsigned char tid, unsigned char chan_prio, int );

int Sleep_Empty();
int Sleep_Full();

