#ifndef HDGD_CAN_H
#define HDGD_CAN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int can_init(char *canPort, int *can_fd, int read_id);
int can_write(int can_fd, int can_id, char *buf);
int can_read(int can_fd, int can_id, char *buf);


#endif // HDGD_CAN_H
