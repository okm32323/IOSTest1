#include "hdgd_msg.h"
#include "hdgd_uart.h"

struct hdgd_msg {
    long int mtype;
    char mtext[DATA_LEN];
};
struct hdgd_msg readmsg, writemsg;


// init msg queue
// par int *msg_id: msg id
// result: success: 0, failed: -1
int msg_init(int *msg_id, char *key_name)
{
    int id;
    key_t key;

    // create msg
   key = ftok(key_name, 66);

   id = msgget(key, 0666 | IPC_CREAT);
   if(id == -1)
   {
       printf("open msg error!\n");

       return -1;
   }
   *msg_id = id;

   return 0;
}


// read data from msg
// par int msg_id:msg id, long int msg_type: recv msg type, char *buf: buf for recv
// result: success: nbytes failed: -1
int msg_read(int msg_id, long int msg_type, char *buf)
{
    int nbytes = 0;
    int idx = 0;

    memset(readmsg.mtext, 0, sizeof(readmsg));
    readmsg.mtype = msg_type;

    for(idx = 0; idx < 3; idx++)
    {
        nbytes = msgrcv(msg_id, (void *)&readmsg, DATA_LEN, 1, 0);
        // recv msg data, success:data num, failed: -1
        if(nbytes > 0)
        {
            break;
        }
    }

    if(nbytes <= 0)
    {
        return -1;
    }

    strncpy(buf, readmsg.mtext, nbytes);
  //  printf("data:%s", buf);

    return nbytes;
}

// write analyze data to msg
// par int msg_id: msg id, char *buf: write data, int msg_type: write msg type
// result: success: 0, failed: -1
int msg_write(int msg_id, long int msg_type, char *buf)
{
    // judge write data number
    if(strlen(buf) <= 0)
    {
        printf("no data to send!\n");
        return -1;
    }

    memset(&writemsg, 0, sizeof(writemsg));
    // deploy msg parameters
    writemsg.mtype = msg_type;
    memset(writemsg.mtext, 0, DATA_LEN);
    strncpy(writemsg.mtext, buf, strlen(buf));

    // msg send data ,if success 0, failed -1
    if(msgsnd(msg_id, (void *)&writemsg, DATA_LEN, 0) < 0)
    {
        printf("send msg error!\n");
        return -1;
    }

    return 0;
}

int msg_delete(int msg_id)
{
    if(msgctl(msg_id, IPC_RMID, NULL) < 0)
    {
        printf("del msg error!\n");

        return -1;
    }

    return 0;
}
