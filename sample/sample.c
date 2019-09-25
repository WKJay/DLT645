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
#include "dlt645_port.h"

//dlt645 采集测试标识符 （A相电压）
#define DLT645_2007_READ_TEST_CODE 0x02010100
#define DLT645_1997_READ_TEST_CODE 0xB611
uint8_t test_addr[6] = {0x00,0x00,0x00,0x00,0x00,0x01};

/**
 * Name:    dlt645_read_test
 * Brief:   dlt645协议采集测试程序
 * Input:   None
 * Output:  None
 */
static void dlt645_read_test(void)
{
    uint8_t read_buf[4];
    rt_memset(read_buf, 0, 4);
    
    //设置从机地址
    dlt645_set_addr(&dlt645,test_addr);
    
    //if(dlt645_read_data(&dlt645,DLT645_1997_READ_TEST_CODE,read_buf,DLT645_1997) > 0) //1997采集测试
    if(dlt645_read_data(&dlt645,DLT645_2007_READ_TEST_CODE,read_buf,DLT645_2007) > 0)  //2007采集测试
    {
        printf("读取成功,A相电压值为: %.2f \r\n",*(float *)read_buf);
    }
    else
    {
        rt_kprintf("读取失败\r\n");
    }
}

/**
 * Name:    main
 * Brief:   主函数
 * Input:   None
 * Output:  None
 */
int main(void)
{
    //dlt645 硬件层初始化
    dlt645_port_init();
    while(1)
    {
        //采集测试
        dlt645_read_test();
        rt_thread_mdelay(1000);
    }
}