#include "hdgd_can.h"

// init can
// par char *canPort: "can0", int *can_fd: fd socket, int read_id: can read id
// result: no return
int can_init(char *canPort, int *can_fd, int read_id)
{
    struct sockaddr_can addr;
    struct ifreq ifr;
    int master = 0;
    int fd = 0;
    int ret = 0;
    printf("canPort:%s\n", canPort);
    fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd < 0) {
        perror("socket PF_CAN failed");
        return 1;
    }

    strcpy(ifr.ifr_name, canPort);
    ret = ioctl(fd, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        perror("ioctl failed");
        return 1;
    }

    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        perror("bind failed");
        return 1;
    }
    if (1) {
        struct can_filter filter[2];
        filter[0].can_id = 0x200 | CAN_EFF_FLAG;
        filter[0].can_mask = 0xFFF;

        filter[1].can_id = 0x20F | CAN_EFF_FLAG;
        filter[1].can_mask = 0xFFF;

        ret = setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER,
                                &filter, sizeof(filter));
        if (ret < 0) {
            perror("setsockopt failed");
            return 1;
        }
    }

    *can_fd = fd;
    printf("can_fd:%d\n", fd);

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
