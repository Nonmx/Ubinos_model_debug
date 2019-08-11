#include "ubinos.h"

extern int R;
extern int F;

int push_message_into_MQ(msgq_pt, unsigned char*);
int get_message_from_MQ(msgq_pt, unsigned char*);

int MQ_empty();
int MQ_full();

