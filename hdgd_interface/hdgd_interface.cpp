#include "hdgd_analyze.h"
#include "hdgd_can.h"
#include "hdgd_msg.h"
#include "hdgd_uart.h"
#include "hdgd_interface.h"

#define CANPORT                 "can0"
#define UARTDEV                 "/dev/ttyS5"
#define MSGKEY                  "/root/"
#define MSGTYPE                 1

#ifndef AF_CAN
#define AF_CAN 29
#endif
#ifndef PF_CAN
#define PF_CAN AF_CAN
#endif


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
#if 0
    // can init
    ret = can_init(CANPORT, &can_fd, can_id);
    if(ret < 0)
    {
        printf("can init failed!\n");
        return -1;
    }
    printf("can init success!\n");
    // uart init
    ret = uart_init(&uart_id, UARTDEV);
    if(ret < 0)
    {
        printf("uart init failed!\n");
        return -1;
    }
    printf("uart init success!\n");
#endif
    // msg init
    ret = msg_init(&msg_id, MSGKEY);
    if(ret < 0)
    {
        printf("msg init failed!\n");
        return -1;
    }
    printf("msg init success!\n");

    return 0;
}

int hdgd_data_run(void)
{
    pthread_t rcvId, sndId;
    int ret = 0;

    ret = hdgd_dev_init();
    if(ret < 0)
    {
        printf("dev init failed!\n");
        return -1;
    }

    pthread_create(&rcvId, NULL, recvFunc, NULL);
    pthread_create(&sndId, NULL, sendFunc, NULL);

    pthread_join(rcvId, NULL);
    pthread_join(sndId, NULL);

    ret = msg_delete(msg_id);
    if(ret < 0)
    {
        printf("msg delete failed!\n");
        return -1;
    }
    return 0;
}

void *recvFunc(void *)
{
    char buf[DATA_LEN] = {0};
    int ret = 0;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    while(1)
    {
        // analyze data write msg
        ret = msg_read(msg_id, MSGTYPE, buf);
        if(ret < 0)
        {
            printf("msg write uart data failed!\n");
            continue;
        }
        printf("buf:%s\n", buf);

        select(0, NULL, NULL, NULL, &tv);
    }
}


// thread: recv uart data delay 1ms and send to msg queue
void *sendFunc(void *)
{
    char buf[DATA_LEN] = {0};
    int ret = 0;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    strcpy(buf, "aqing");

    while(1)
    {
#if 0
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
#endif
            // analyze data write msg
            ret = msg_write(msg_id, MSGTYPE, buf);
            if(ret < 0)
            {
                printf("msg write uart data failed!\n");
                continue;
            }
   //     }

        // delay 1ms
        select(0, NULL, NULL, NULL, &tv);
    }
}

void *can_recv(void *)
{
    char buf[DATA_LEN] = {0};
    int ret = 0;
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    printf("can recv!\n");
    while(1)
    {
        // analyze data write msg
        ret = can_read(can_fd, can_id, buf);
        if(ret < 0)
        {
            printf("can recv data failed!\n");
            continue;
        }
        printf("buf:%s\n", buf);

        select(0, NULL, NULL, NULL, &tv);
    }
}

void *can_send(void *)
{
    char buf[DATA_LEN] = {0};
    int ret = 0;
    struct timeval tv;
    tv.tv_sec = 20;
    tv.tv_usec = 0;

    strcpy(buf, "aqing");

    while(1)
    {
        // analyze data write msg
        ret = can_write(can_fd, can_id, buf);
        if(ret < 0)
        {
            printf("can write data failed!\n");
            continue;
        }

        // delay 1ms
        select(0, NULL, NULL, NULL, &tv);
    }
}

static void print_frame(struct can_frame *fr)
{
    int i;
    printf("%08x\n", fr->can_id & CAN_EFF_MASK);
    //printf("%08x\n", fr->can_id);
    printf("dlc = %d\n", fr->can_dlc);
    printf("data = ");
    for (i = 0; i < fr->can_dlc; i++)
        printf("%02x ", fr->data[i]);
    printf("\n");
}

#define errout(_s)	fprintf(stderr, "error class: %s\n", (_s))
#define errcode(_d) fprintf(stderr, "error code: %02x\n", (_d))

static void handle_err_frame(const struct can_frame *fr)
{
    if (fr->can_id & CAN_ERR_TX_TIMEOUT) {
        errout("CAN_ERR_TX_TIMEOUT");
    }
    if (fr->can_id & CAN_ERR_LOSTARB) {
        errout("CAN_ERR_LOSTARB");
        errcode(fr->data[0]);
    }
    if (fr->can_id & CAN_ERR_CRTL) {
        errout("CAN_ERR_CRTL");
        errcode(fr->data[1]);
    }
    if (fr->can_id & CAN_ERR_PROT) {
        errout("CAN_ERR_PROT");
        errcode(fr->data[2]);
        errcode(fr->data[3]);
    }
    if (fr->can_id & CAN_ERR_TRX) {
        errout("CAN_ERR_TRX");
        errcode(fr->data[4]);
    }
    if (fr->can_id & CAN_ERR_ACK) {
        errout("CAN_ERR_ACK");
    }
    if (fr->can_id & CAN_ERR_BUSOFF) {
        errout("CAN_ERR_BUSOFF");
    }
    if (fr->can_id & CAN_ERR_BUSERROR) {
        errout("CAN_ERR_BUSERROR");
    }
    if (fr->can_id & CAN_ERR_RESTARTED) {
        errout("CAN_ERR_RESTARTED");
    }
}
#define myerr(str)	fprintf(stderr, "%s, %s, %d: %s\n", __FILE__, __func__, __LINE__, str)

void *can_Stest(void *)
{
    int ret;
    struct can_frame frdup;
    printf("can_Stest:%d\n", can_fd);
    struct timeval tv;
    fd_set rset;
    int idx = 0;

 //   frdup.can_id = 0x11;

    while (1) {
        tv.tv_sec = 1;
        tv.tv_usec = 100000;
#if 0
        FD_ZERO(&rset);
        FD_SET(can_fd, &rset);
        printf("=====\n");
/*
        ret = select(can_fd+1, &rset, NULL, NULL, NULL);
        if (ret == 0) {
            myerr("select time out");
            return -1;
        }
*/
        printf("------------------------ \n");
        ret = read(can_fd, &frdup, sizeof(frdup));
        if (ret < sizeof(frdup)) {
            myerr("read failed");
  //          return -1;
        }

        if (frdup.can_id & CAN_ERR_FLAG) { /* 出错设备错误 */
            handle_err_frame(&frdup);
            myerr("CAN device error");
            continue;
        }
        print_frame(&frdup);
#endif
        ret = write(can_fd, &frdup, sizeof(frdup));
        if (ret < 0) {
            myerr("write failed");
      //      return -1;
        }
        else
        {
            printf("ret:%d ", ret);
        }

     //   ret = read(can_fd, &frdup, sizeof(frdup));
     //   printf("data:%s\n", frdup);
        select(0, NULL, NULL, NULL, &tv);
    }

    return 0;
}


void *can_Rtest(void *)
{
    int ret;
    struct can_frame frdup;
    printf("can_Rtest:%d\n", can_fd);
    struct timeval tv;
    fd_set rset;
    int idx = 0;

 //   frdup.can_id = 0x11;

    while (1) {
        tv.tv_sec = 1;
        tv.tv_usec = 10000;

        ret = read(can_fd, &frdup, sizeof(frdup));
        if (ret < 0) {
            myerr("read failed");
      //      return -1;
        }
        else
        {
            for(idx = 0; idx < 8; idx++)
            {
                printf("data:%02x\n", *(char *)(frdup.data + idx));
            }

        }

        select(0, NULL, NULL, NULL, &tv);
    }

    return 0;
}


void testCan(void)
{
    char buf[64] = {0};
    pthread_t canSID, canRID;
    int master = 0;

    can_id = 0x11;
    can_init(CANPORT, &can_fd, can_id);
    printf("can init!\n");

  //  pthread_create(&canSID, NULL, can_Stest, NULL);
    pthread_create(&canRID, NULL, can_Rtest, NULL);

 //   pthread_join(canSID, NULL);
    pthread_join(canRID, NULL);

    close(can_fd);
}


void testUart(void)
{
    int uart_id = 0;
    char read_buf[DATA_LEN] = {0};
    char out_buf[DATA_LEN] = {0};
    struct timeval tv;
    int ret = 0, data_num = 0;

    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    uart_init(&uart_id, UARTDEV);

    while(1)
    {
        memset(read_buf, 0, DATA_LEN);
        ret = uart_read(uart_id, read_buf);
        if(ret == 21)
        {
      //      break;
            memset(out_buf, 0, DATA_LEN);
            ret = analyze_Data(read_buf, out_buf);
            if(ret == 0)
            {
                for(data_num = 0; data_num < strlen(out_buf); data_num++)
                {
                    printf("%02d ", *(char *)(out_buf + data_num));
                }
                printf("\n");
            }
        }



        select(0, NULL, NULL, NULL, &tv);
    }




}

void testMsg(void)
{
    int msg_id = 0;
    pthread_t rcvId, sndId;
    char read_buf[DATA_LEN] = {0};
    long int msg_type = 0x1;
    int ret = 0;

    msg_init(&msg_id, MSGKEY);


    pthread_create(&rcvId, NULL, recvFunc, NULL);
    pthread_create(&sndId, NULL, sendFunc, NULL);

    pthread_join(rcvId, NULL);
    pthread_join(sndId, NULL);

    ret = msg_delete(msg_id);
    if(ret < 0)
    {
        printf("msg delete failed!\n");
     //   return -1;
    }
}

















