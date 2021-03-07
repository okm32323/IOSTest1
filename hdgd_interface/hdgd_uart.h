#ifndef HDGD_UART_H
#define HDGD_UART_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>


#define DATA_LEN            512


int uart_init(int *uart_id, char *dev);
int uart_read(int tty_id, char *buf);
int uart_write(int tty_id, char *buf);
void uart_close(int tty_id);



#endif // HDGD_UART_H
