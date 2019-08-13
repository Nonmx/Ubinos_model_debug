#include "ubinos.h"



int push_message_into_MQ(msgq_pt, unsigned char*,Msgq*);
int get_message_from_MQ(msgq_pt, unsigned char*,Msgq*);

int MQ_empty(msgq_pt, Msgq*);
int MQ_full(msgq_pt, Msgq*);

