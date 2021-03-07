#include "hdgd_uart.h"
#include "hdgd_analyze.h"


// init uart com
// par int tty_id: uart comId, char *dev: "/dev/ttS*" dev char
// result success: 0, failed: -1
int uart_init(int *uart_id, char *dev)
{
    struct termios options;
    int ret = 0;
    int tty_id;

    // open "/dev/ttyS*"
    tty_id = open(dev, O_RDWR|O_NOCTTY|O_NDELAY);
    if(tty_id < 0)
    {
        printf("open tty failed:%s\n", strerror(errno));
        return -1;
    }

    printf("open devices sucessful!\n");

    // deploy usart par
    memset(&options, 0, sizeof(options));
    ret = tcgetattr(tty_id, &options);
    if(ret != 0)
    {
        printf("tcgetattr() failed:%s\n", strerror(errno));
        return -1;
    }

    options.c_cflag |= (CLOCAL|CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;

    cfsetspeed(&options, B115200);

    options.c_cflag &= ~CSTOPB;

    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 0;
    tcflush(tty_id, TCIFLUSH);

    if((tcsetattr(tty_id, TCSANOW, &options)) != 0)
    {
        printf("tcsetattr failed:%s\n", strerror(errno));
        return -1;
    }

    *uart_id = tty_id;

    return 0;
}

// read data from uart
// par int tty_id: uart comId buf:data for buf
// result success: nbytes failed: -1
// buf must lager than DATA_LEN
int uart_read(int tty_id, char *buf)
{
    int nbytes = 0;

    // judge buf pointer is avalible
    if(buf == NULL)
    {
        printf("recv buf is wrong !\n");
        return -1;
    }

    memset(buf, 0, DATA_LEN);
    nbytes = read(tty_id, buf, DATA_LEN);
    if(nbytes < 0)
    {
        printf("Read() error:%s\n", strerror(errno));
        return -1;
    }

  //  printf("Read from tty:%s\n", buf);

    return nbytes;
}

// write data to uart
// par int tty_id: uart comId, char *buf: buf for wirte
int uart_write(int tty_id, char *buf)
{
    int nbytes = 0;

    // judge write number is avalible
    if(strlen(buf)  <= 0)
    {
        printf("no data!\n");
        return -1;
    }

    nbytes = write(tty_id, buf, strlen(buf));
    if(nbytes < 0)
    {
        printf("write() error:%s\n", strerror(errno));
        return -1;
    }

    return nbytes;
}

// close uart comId
// par int tty_id: uart comId
void uart_close(int tty_id)
{
    close(tty_id);
}








