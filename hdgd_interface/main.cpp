#include <stdio.h>
#include "hdgd_interface.h"

int main(int argc, char *argv[])
{
    printf("hello interface!\n");

   // hdgd_dev_init();

    // test uart /dev/ttyS3

  //  testUart();

    testCan();

  //  testMsg();

  //  hdgd_data_run();


    return 0;
}
