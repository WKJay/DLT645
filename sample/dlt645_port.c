/*************************************************
 Copyright (c) 2019
 All rights reserved.
 File name:     dlt645_port.c
 Description:   DLT645 移植&使用例程文件
 History:
 1. Version:    
    Date:       2019-09-19
    Author:     wangjunjie
    Modify:     
*************************************************/
#include "dlt645.h"
#include "rtthread.h"
#include "drv_gpio.h"

//DLT645采集使用的串口名
#define DLT645_SERIAL_NAME "uart4"
//dlt645 采集测试标识符 （A相电压）
#define DLT645_READ_TEST_CODE 0x2010100

//DL/T 645硬件拓展结构体
typedef struct
{
    rt_sem_t dlt645_sem;   //用于串口接收的信号量
    uint32_t byte_timeout; //字节间的超时时间
} dlt645_port_t;

static dlt645_port_t dlt645_port = {
    .dlt645_sem = RT_NULL,
    .byte_timeout = 20,
};
//dlt645 采集设备句柄
static rt_device_t dlt645_device = RT_NULL;
//dlt645 采集接收信号量
static struct rt_semaphore dlt645_receive_sem;
//dlt645 环境结构体
dlt645_t dlt645;
//串口配置参数
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

//串口接收数据回调函数
rt_err_t uart_handler(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&dlt645_receive_sem);
    return RT_EOK;
}

//硬件层接收函数
static int dlt645_hw_read(dlt645_t *ctx, uint8_t *msg)
{
    int read_len = 0;
    if(rt_sem_take(&dlt645_receive_sem, 1000) == -RT_ETIMEOUT)
    {
        return 0;
    }
    while (rt_device_read(dlt645_device, 0, msg + read_len, 1) == 1)
    {
        read_len ++;
        //读取超时标志一次读取完成
        if (rt_sem_take(&dlt645_receive_sem, ((dlt645_port_t *)(ctx->port_data))->byte_timeout) == -RT_ETIMEOUT)
        {
            break;
        }
    }

    return read_len;
}

//硬件层发送函数
static int dlt645_hw_write(dlt645_t *ctx, uint8_t *buf, uint16_t len)
{
    return rt_device_write(dlt645_device,0,buf,len);
}

static void dlt645_read_test(void)
{
    uint8_t read_buf[4];
    rt_memset(read_buf, 0, 4);

    if(dlt645_read_data(&dlt645,1,DLT645_READ_TEST_CODE,read_buf,DLT645_2007) > 0)
    {
        printf("读取成功,A相电压值为: %.1f \r\n",*(float *)read_buf);
    }
    else
    {
        rt_kprintf("读取失败\r\n");
    }
    
}
MSH_CMD_EXPORT(dlt645_read_test , dlt645 protocal read test);

int dlt645_port_init(void)
{
    dlt645_device = rt_device_find(DLT645_SERIAL_NAME);
    if (dlt645_device == RT_NULL)
    {
        rt_kprintf("cannot find device %s \r\n", DLT645_SERIAL_NAME);
        return -RT_ERROR;
    }
    if (rt_device_open(dlt645_device, RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("cannot open device %s \r\n", DLT645_SERIAL_NAME);
        return -RT_ERROR;
    }
    else
    {
        config.baud_rate = BAUD_RATE_9600;
        config.data_bits = DATA_BITS_8;
        config.stop_bits = STOP_BITS_1;
        config.parity = PARITY_NONE;
        /* 打开设备后才可修改串口配置参数 */
        rt_device_control(dlt645_device, RT_DEVICE_CTRL_CONFIG, &config);
        rt_kprintf("device %s open success \r\n", DLT645_SERIAL_NAME);
    }

    if (rt_sem_init(&dlt645_receive_sem, "receive_sem", 0, RT_IPC_FLAG_FIFO) == RT_EOK)
    {
        dlt645_port.dlt645_sem = &dlt645_receive_sem;
    }
    else
    {
        return -RT_ERROR;
    }

    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(dlt645_device, uart_handler);
    rt_pin_mode(GET_PIN(A,15),PIN_MODE_OUTPUT);
    return  RT_EOK;
}
INIT_APP_EXPORT(dlt645_port_init);

//645结构体注册
static dlt645_t dlt645 = {
    {0},
    0,
    dlt645_hw_write,
    dlt645_hw_read,
    (void *)&dlt645_port};
