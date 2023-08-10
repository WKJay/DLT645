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

//DL/T 645硬件拓展结构体
typedef struct
{
    rt_sem_t dlt645_sem;   //用于串口接收的信号量
    uint32_t byte_timeout; //字节间的超时时间
} dlt645_port_t;

static dlt645_port_t dlt645_port = {
    .dlt645_sem = RT_NULL,
    .byte_timeout = 10, //接收字节间超时时间
};
//dlt645 采集设备句柄
static rt_device_t dlt645_device = RT_NULL;
//dlt645 采集接收信号量
static struct rt_semaphore dlt645_receive_sem;
//串口配置参数
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
//dlt645 环境结构体
dlt645_t dlt645;

//串口接收数据回调函数
rt_err_t uart_handler(rt_device_t dev, rt_size_t size)
{
    //接收到一个数据释放信号量
    rt_sem_release(&dlt645_receive_sem);
    return RT_EOK;
}

/**
 * Name:    dlt645_hw_read
 * Brief:   dlt645 硬件层接收数据
 * Input:
 *  @ctx:   645运行环境
 *  @msg:   接收数据存放地址
 *  @len:   数据最大接收长度 
 * Output:  读取数据的长度
 */
static int dlt645_hw_read(dlt645_t *ctx, uint8_t *msg ,uint16_t len)
{
    //实际接收长度
    int read_len = 0;
    //清缓存变量
    uint8_t buf = 0;
    
    //清空缓存
    while(rt_device_read(dlt645_device,0,&buf,1));
    //等待串口接收到数据
    if(rt_sem_take(&dlt645_receive_sem, 1000) == -RT_ETIMEOUT)
    {
        return 0;
    }
    //每次读取一个字节的数据
    while (rt_device_read(dlt645_device, 0, msg + read_len, 1) == 1)
    {
        if(read_len > len)
        {
            return 0;
        }
        else
        {
            read_len ++;
        }
        //读取超时标志一帧数据读取完成
        if (rt_sem_take(&dlt645_receive_sem, ((dlt645_port_t *)(ctx->port_data))->byte_timeout) == -RT_ETIMEOUT)
        {
            break;
        }
    }
    return read_len;
}

/**
 * Name:    dlt645_hw_write
 * Brief:   dlt645 硬件层发送数据
 * Input:
 *  @ctx:   645运行环境
 *  @buf:   待发送数据
 *  @len:   发送长度
 * Output:  实际发送的字节数，错误返回-1
 */
static int dlt645_hw_write(dlt645_t *ctx, uint8_t *buf, uint16_t len)
{
    //串口发送数据
    rt_pin_write(GET_PIN(A,15), PIN_HIGH);
    int ret = rt_device_write(dlt645_device,0,buf,len);
    rt_pin_write(GET_PIN(A,15), PIN_LOW);
    return ret;
}


/**
 * Name:    dlt645_port_init
 * Brief:   645采集硬件层初始化
 * Input:   None
 * Output:  None
 */
int dlt645_port_init(void)
{
    //串口初始化
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

    //信号量初始化
    if (rt_sem_init(&dlt645_receive_sem, "receive_sem", 0, RT_IPC_FLAG_FIFO) == RT_EOK)
    {
        dlt645_port.dlt645_sem = &dlt645_receive_sem;
    }
    else
    {
        return -RT_ERROR;
    }

    //设置串口接收回调函数
    rt_device_set_rx_indicate(dlt645_device, uart_handler);
    //485控制引脚初始化
    rt_pin_mode(GET_PIN(A,15),PIN_MODE_OUTPUT);
    return  RT_EOK;
}

//645结构体注册
dlt645_t dlt645 = {
    {0},
    0,
    dlt645_hw_write,
    dlt645_hw_read,
    (void *)&dlt645_port};
