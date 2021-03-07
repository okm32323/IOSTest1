#include "hdgd_can.h"

// init can
// par char *canPort: "can0", int *can_fd: fd socket, int read_id: can read id
// result: no return
int can_init(char *canPort, int *can_fd, int read_id)
{
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_filter rfilter = {0};
    int fd;

    fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);		// 创建套接字
    strncpy(ifr.ifr_name, canPort, strlen(canPort));
    ioctl(fd, SIOCGIFINDEX, &ifr);				// 指定can0设备
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(fd, (struct sockaddr *)&addr, sizeof(addr));	// 将套接字与can0绑定

    rfilter.can_id = read_id;
    rfilter.can_mask = CAN_SFF_MASK;
    // 禁用过滤规则
   // setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
    // 设置recv过滤规则
    setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    *can_fd = fd;

    return 0;
}

// can write data
// par int can_fd: fd can socket, int can_id: write filter id, char *buf: write data
// result: success: nbytes, failed: -1
int can_write(int can_fd, int can_id, char *buf)
{
    struct can_frame frame = {0};
    int nbytes = 0;

    // 生成报文
    frame.can_id = can_id;
    frame.can_dlc = 1;
    memset(&frame, 0, sizeof(frame));
    strncpy((char *)frame.data, buf, strlen(buf));

    nbytes = write(can_fd, &frame, sizeof(frame));	// 发送frame
    if(nbytes != sizeof(frame))
    {
        printf("send Error frame\n");
        return -1;
    }

    return nbytes;
}

// can read data
// par int can_fd: fd can socket, int can_id: can recv filter id, *buf: recv data
// result: success: nbytes, failed: -1
int can_read(int can_fd, int can_id, char *buf)
{
    struct can_frame frame = {0};
    int nbytes = 0;

    memset(&frame, 0, sizeof(frame));
    nbytes = read(can_fd, &frame, sizeof(frame));	// 接收报文
    // 显示报文
    if(nbytes > 0)
    {
        memcpy(buf, frame.data, nbytes);
    //    printf("ID=0x%X DLC=%d data[0]=0x%X\n", frame.can_id, frame.can_dlc, frame.data);
        return nbytes;
    }

    return -1;
}
