#include "hdgd_uart.h"
#include "hdgd_analyze.h"

#define UART_DATA_LEN           21

// init uart com
// par int tty_id: uart comId, char *dev: "/dev/ttS*" dev char
// result success: 0, failed: -1
int uart_init(int *uart_id, char *dev)
{
    struct termios options;
    int ret = 0;
    int tty_id;
    printf("%s\n", dev);
    // open "/dev/ttyS*"
    tty_id = open(dev, O_RDWR|O_NOCTTY|O_NDELAY);
    if(tty_id < 0)
    {
        printf("open tty failed:%s\n", strerror(errno));
        return -1;
    }
    mkfifo(dev, O_CREAT|O_RDWR|0666);

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
    struct timeval tv;
    int idx = 0;
    char recvbuf[DATA_LEN/4] = {0};
    static char tmpbuf[DATA_LEN/2] = {0};
    static int data_on = 0, data_flag = 0,
            data_idx = 0, data_time_idx = 0,
            old_data_time_idx = 0;


    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    // judge buf pointer is avalible
    if(buf == NULL)
    {
        printf("recv buf is wrong !\n");
        return -1;
    }

    memset(buf, 0, DATA_LEN);
    idx = 0;

    while(1)
    {
        memset(recvbuf, 0, DATA_LEN/4);
        nbytes = read(tty_id, recvbuf, DATA_LEN);

        if(nbytes > 0)
        {
        //    printf("%02x ", (*(char *)recvbuf & 0xFF));
            if((*(char *)recvbuf & 0xFF) == 0xaa)
            {
                data_flag = 1;
                old_data_time_idx = data_time_idx;
           //     printf("0xaa\n");
            }

            if(data_flag)
            {
                if(old_data_time_idx == (data_time_idx - 1))
                {
                    if((*(char *)recvbuf & 0xFF) == 0x55)
                    {
                    //    printf("0x55\n");
                        data_on = 1;
                        data_idx = 1;
                    }
                    else
                    {
                        data_on = 0;
                        data_flag = 0;
                    }
                }
            }

            if(data_on)
            {
                *(char *)(tmpbuf + data_idx) = (*(char *)recvbuf & 0xFF);
                data_idx++;
            }

            if(data_idx == UART_DATA_LEN)
            {
                *(char *)tmpbuf = 0xaa;
                memcpy(buf, tmpbuf, UART_DATA_LEN);
#if 0
                printf("\nbuf:");
                for(idx = 0; idx < UART_DATA_LEN; idx++)
                {
                    printf("%02x ", (*(char *)(buf + idx) & 0xFF));
                }
//                printf("\nogn:");
#endif
                data_idx = 0;

                return UART_DATA_LEN;
            }

            data_time_idx++;

            if(data_time_idx >= 0x1000000)
            {
                data_time_idx = 0;
            }
#if 0
            printf("nbytes:%d\n", nbytes);
            for(idx = 0; idx < nbytes; idx++)
            {
                printf("%02x ", (*(char *)(buf + idx) & 0xFF));
            }

       //     printf("\n");
#endif
        //    return nbytes;
        }

#if 0
        if(nbytes > 0)
        {
            printf("nbytes:%d\n", nbytes);
            memcpy(tmpbuf + data_idx, recvbuf, nbytes);
            data_idx += nbytes;

            if(data_idx >= UART_DATA_LEN)
            {
                for(i = 0; i < data_idx; i++)
                {
                    if((tmpbuf[i] == 0x55) && (tmpbuf[i - 1] == 0xaa))
                    {
                        memmove(tmpbuf, &tmpbuf[i - 1], data_idx - (i - 1));
                        memset((tmpbuf + (data_idx - (i - 1))), 0, (i - 1));

                        data_num = strlen(tmpbuf);
                        if(data_num >= UART_DATA_LEN)
                        {
                            strncpy(buf, tmpbuf, UART_DATA_LEN);
                            data_idx -= UART_DATA_LEN;

                            memmove(tmpbuf, &tmpbuf[UART_DATA_LEN], data_idx - UART_DATA_LEN);
                            memset(tmpbuf+data_idx - UART_DATA_LEN, 0, UART_DATA_LEN);

                            for(idx = 0; idx <= UART_DATA_LEN; idx++)
                            {
                                printf("%x", buf + idx);
                            }
                            printf("\n");

                            return UART_DATA_LEN;
                        }
                        else
                        {
                            data_idx = data_idx - i + 1;
                        }
                    }
                }
            }
        }

        time_idx++;
        if(time_idx >= 100)
        {
            printf("no data!\n");
            return -1;
        }
#endif
        select(0, NULL, NULL, NULL, &tv);
    }
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








