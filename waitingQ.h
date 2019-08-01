#include "ubinos.h"
#include "config.h"

#define QSIZE 15

typedef struct
{
	unsigned char tid;
	unsigned char prio;
}WQ;

//int Front = 0;
//int Rear = 0;

int Front[MAX_PRIORITY + 1];
int Rear[MAX_PRIORITY + 1];


unsigned char TID[1];
unsigned char PRI[1];


int SIZE[MAX_PRIORITY + 1];
int WHOLESIZE;
#define full(p) (SIZE[(p)] == QSIZE-1)
#define empty() (WHOLESIZE == 0)
int PRIORITY;

WQ waitingQ[MAX_PRIORITY + 1][QSIZE];

void push_task_into_WQ(unsigned char, unsigned char);
void get_task_from_WQ(unsigned char*, unsigned char*);


