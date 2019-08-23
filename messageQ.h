#include "ubinos.h"



int push_message_into_MQ(msgq_pt, unsigned char*);
int get_message_from_MQ(msgq_pt, unsigned char*);

int MQ_empty(msgq_pt);
int MQ_full(msgq_pt);

