#ifndef HDGD_CAN_H
#define HDGD_CAN_H

#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

int can_init(char *canPort, int *can_fd, int read_id);
int can_write(int can_fd, int can_id, char *buf);
int can_read(int can_fd, int can_id, char *buf);


#endif // HDGD_CAN_H
