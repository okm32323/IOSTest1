#ifndef HDGD_ANALYZEDATA_H
#define HDGD_ANALYZEDATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>

int msg_init(int *msg_id, char *key_name);
int msg_read(int msg_id, long int msg_type, char *buf);
int msg_write(int msg_id, long int msg_type, char *buf);
int msg_delete(int msg_id);

#endif // HDGD_ANALYZEDATA_H
