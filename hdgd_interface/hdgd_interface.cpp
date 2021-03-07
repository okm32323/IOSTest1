#include "hdgd_analyze.h"
#include "hdgd_can.h"
#include "hdgd_msg.h"
#include "hdgd_uart.h"

#define CANPORT                 "can0"
#define UARTDEV                 "/dev/ttyS3"
#define MSGKEY                  "/root/"
#define MSGTYPE                 1

int can_fd = 0, can_id = 0;
int uart_id = 0;
int msg_id = 0;

void *recvFunc(void *);
void *sendFunc(void *);



int hdgd_dev_init(void)
{
 //   int can_fd = 0, can_id = 0;
 //   int uart_id = 0;
 //   int msg_id = 0;
    int ret = 0;

    // can init
    ret = can_init(CANPORT, &can_fd, can_id);
    if(ret < 0)
    {
        printf("can init failed!\n");
        return -1;
    }
    // uart init
    ret = uart_init(&uart_id, UARTDEV);
    if(ret < 0)
    {
        printf("uart init failed!\n");
        return -1;
    }

    // msg init
    ret = msg_init(&msg_id, MSGKEY);
    if(ret < 0)
    {
        printf("msg init failed!\n");
        return -1;
    }

    return 0;
}

int hdgd_data_run(void)
{
    pthread_t rcvId;
    int ret = 0;

    ret = hdgd_dev_init();
    if(ret < 0)
    {
        printf("dev init failed!\n");
        return -1;
    }

    pthread_create(&rcvId, NULL, sendFunc, NULL);

    pthread_join(rcvId, NULL);

    ret = msg_delete(msg_id);
    if(ret < 0)
    {
        printf("msg delete failed!\n");
        return -1;
    }
    return 0;
}



// thread: recv uart data delay 1ms and send to msg queue
void *sendFunc(void *)
{
    char buf[DATA_LEN] = {0};
    char outbuf[DATA_LEN] = {0};
    int ret = 0;

    while(1)
    {
        memset(buf, 0, DATA_LEN);
        memset(outbuf, 0, DATA_LEN);

        // read from uart
        ret = uart_read(uart_id, buf);
        if(ret > 0)
        {
            // if have avalible data and analyze
            ret = analyze_Data(buf, outbuf);
            if(ret < 0)
            {
                printf("uart data error!\n");
                continue;
            }

            // analyze data write msg
            ret = msg_write(msg_id, MSGTYPE, outbuf);
            if(ret < 0)
            {
                printf("msg write uart data failed!\n");
                continue;
            }
        }

        // delay 1ms
        usleep(1000);
    }
}



void testCan(void)
{
    int can_fd = 0, can_id = 0;
    char buf[64] = {0};

    can_id = 0x11;
    can_init(CANPORT, &can_fd, can_id);
    strcpy(buf, "nihao,1014");
    can_write(can_fd, can_id, buf);
    memset(buf, 0, 64);
    can_read(can_fd, can_id, buf);
}


void testUart(void)
{
    int uart_id = 0;
    char read_buf[DATA_LEN] = {0};
    char out_buf[DATA_LEN] = {0};

    uart_init(&uart_id, UARTDEV);
    uart_read(uart_id, read_buf);

    analyze_Data(read_buf, out_buf);


}

void testMsg(void)
{
    int msg_id = 0;
    char read_buf[DATA_LEN] = {0};
    long int msg_type = 0x1;

    msg_init(&msg_id, MSGKEY);

    memset(read_buf, 0, DATA_LEN);
    msg_read(msg_id, msg_type, read_buf);
    msg_write(msg_id, msg_type, read_buf);
}

















