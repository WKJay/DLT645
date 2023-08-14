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
#include "dlt645.h"
#include "rtthread.h"
#include "stdio.h"
#include "dlt645_port.h"

// dlt645 采集测试标识符 （A相电压）
#define DLT645_2007_READ_TEST_CODE 0x02010100
#define DLT645_1997_READ_TEST_CODE 0xB611
uint8_t test_addr[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

/**
 * Name:    dlt645_read_test
 * Brief:   dlt645协议采集测试程序
 * Input:   None
 * Output:  None
 */
static void dlt645_read_test(void) {
    uint8_t read_buf[4];
    rt_memset(read_buf, 0, 4);

    // 设置从机地址
    dlt645_set_addr(&dlt645, test_addr);

    // 设置debug模式
    dlt645_set_debug(&dlt645, 0);

    // if(dlt645_read_data(&dlt645,DLT645_1997_READ_TEST_CODE,read_buf,DLT645_1997) > 0) //1997采集测试
    if (dlt645_read_data(&dlt645, DLT645_2007_READ_TEST_CODE, read_buf, DLT645_2007) > 0)  // 2007采集测试
    {
        printf("读取成功,A相电压值为: %.2f \r\n", *(float *)read_buf);
    } else {
        rt_kprintf("读取失败\r\n");
    }
}

/**
 * Name:    dlt645_entry
 * Brief:   dlt645协议测试线程
 * Input:   None
 * Output:  None
 */
void dlt645_entry(void *param) {
    // dlt645 硬件层初始化
    dlt645_port_init();
    while (1) {
        // 采集测试
        dlt645_read_test();
        rt_thread_mdelay(1000);
    }
}

int dlt645_test(void) {
    rt_thread_t tid;
    tid = rt_thread_create("dlt645", dlt645_entry, RT_NULL, 4096, 8, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
    return 0;
}
MSH_CMD_EXPORT(dlt645_test, dlt645 test);
