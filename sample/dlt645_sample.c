/*************************************************
 Copyright (c) 2019
 All rights reserved.
 File name:     sample.c
 Description:   DLT645 软件包使用样例
 History:
 1. Version:    
    Date:       2019-09-23
    Author:     wangjunjie
    Modify:     
*************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "dlt645.h"

#define DEVICE_NAME "/dev/ttyUSB0"
//dlt645 采集测试标识符 （A相电压）
#define DLT645_2007_READ_TEST_CODE 0x00010000
#define DLT645_1997_READ_TEST_CODE 0xB611
uint8_t test_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static dlt645_t *dlt645 = NULL;

/**
 * Name:    dlt645_read_test
 * Brief:   dlt645协议采集测试程序
 * Input:   None
 * Output:  None
 */
static void dlt645_read_test(void)
{
    uint8_t read_buf[4];
    memset(read_buf, 0, 4);

    //设置从机地址
    dlt645_set_addr(dlt645, test_addr);

    //if(dlt645_read_data(&dlt645,DLT645_1997_READ_TEST_CODE,read_buf,DLT645_1997) > 0) //1997采集测试
    if (dlt645_read_data(dlt645, DLT645_2007_READ_TEST_CODE, read_buf, DLT645_2007) > 0) //2007采集测试
    {
        printf("Read success,data: %.2f \r\n", *(float *)read_buf);
    }
    else
    {
        printf("读取失败\r\n");
    }
}

static void stop(int param)
{
    dlt645_close(dlt645);
    exit(0);
}

/**
 * Name:    main
 * Brief:   主函数
 * Input:   None
 * Output:  None
 */
int main(void)
{
    signal(SIGINT, stop);
    //dlt645 硬件层初始化
    dlt645 = dlt645_new_ctx(DEVICE_NAME, 9600, 'N', 8, 1, 0);
    dlt645_set_response_timeout(dlt645, 1500);
    if (dlt645 == NULL)
    {
        return -1;
    }
    while (1)
    {
        //采集测试
        dlt645_read_test();
        usleep(1000 * 1000);
    }
}
