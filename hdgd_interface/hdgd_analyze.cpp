#include "hdgd_analyze.h"

#define GETBITVALUE(x, idx)                 ((x >> idx) & 1)

// analyze uart data
// par char *inbuf: uart data, char *outbuf: analyze data
// result: success: 0, failed: -1
int analyze_Data(char *inbuf, char *outbuf)
{
    long long int CRC = 0;
    int idx = 0, buf_idx = 0,
            tmp_value = 0;
    static int key_Value = 0;

    // judge inbuf data number
    if(strlen(inbuf) <= 0)
    {
        printf("inbuf data number error!\n");
        return -1;
    }

    // judge head data 0xAA55
    if((inbuf[0] != 0xaa) || (inbuf[1] != 0x55))
    {
        printf("data head is not 0xAA55, ERROR!\n");
        return -1;
    }

    // judge CRC
    for(idx = 0; idx < 20; idx++)
    {
        CRC += inbuf[idx];
    }
    CRC &= 0x000000FF;

    if(inbuf[idx] != CRC)
    {
        printf("CRC is wrong!n");
        return -1;
    }

    buf_idx = 2;
    // transparent scatter data
    memcpy(&outbuf[buf_idx], &inbuf[5], 8);
    buf_idx += 8;

    // analyze key value
    idx = 0;
    for(idx = 0; idx < 25; idx++)
    {
        if(GETBITVALUE(*(int *)(inbuf + 2), idx) == 1)
        {
             tmp_value = idx + 1;
             if(tmp_value != key_Value)
             {
                 key_Value = tmp_value;
             }
             else
             {
                 key_Value = 0;
             }
        }
    }
    *(short *)outbuf = key_Value;

    return 0;
}















