/*************************************************
 Copyright (c) 2023
 All rights reserved.
 File name:     dlt645_rs485_port.c
 Description:   DLT645用RS485通信的移植&使用例程文件
 History:
 1. Version:    
    Date:       2023-08-11
    Author:     LiangZohar
    Modify:     
    Env:        RT-Thread v5.0.1
    Depend on:  CONFIG_PKG_USING_RS485=y
    Usage：     打开并更新RT-Thread的RS485软件包
*************************************************/

#include <rtthread.h>
#include <rtdevice.h>
#define DBG_TAG "dlt645_485"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "dlt645_port.h"
#include "dlt645.h"
#include "rs485.h"


#ifndef DLT645_SERIAL_NAME
#define DLT645_SERIAL_NAME          "uart5" //serial device name
#endif

#ifndef DLT645_SERIAL_BAUDRATE
#define DLT645_SERIAL_BAUDRATE      2400
#endif

#ifndef DLT645_SERIAL_DATA_BITS
#define DLT645_SERIAL_DATA_BITS     8 //0 -- data bits
#endif

#ifndef DLT645_SERIAL_STOP_BITS
#define DLT645_SERIAL_STOP_BITS     1 //0 -- stop bits
#endif

#ifndef DLT645_SERIAL_PARITY
#define DLT645_SERIAL_PARITY        2 //0 -- none parity
#endif

#ifndef DLT645_RS485_DE_PIN
#define DLT645_RS485_DE_PIN         8 //-1 -- nonuse rs485 mode control
#endif

#ifndef DLT645_RS485_TX_LVL
#define DLT645_RS485_TX_LVL         1 //1 -- rs485 tx level
#endif

//645结构体注册
dlt645_t dlt645 = {0};

/**
 * Name:    dlt645_hw_read
 * Brief:   dlt645 硬件层接收数据
 * Input:
 *  @ctx:   645运行环境
 *  @msg:   接收数据存放地址
 *  @len:   数据最大接收长度 
 * Output:  读取数据的长度
 */
int dlt645_hw_read(dlt645_t *ctx, uint8_t *msg ,uint16_t len)
{
    int dlen = rs485_recv((rs485_inst_t *)(ctx->port_data), (void *)msg, len);
    if(ctx->debug){
        ulog_hexdump("dlt645 recv: ", dlen, msg, dlen);
    }
    return dlen;
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
    if(ctx->debug){
        ulog_hexdump("dlt645 send: ", len, buf, len);
    }
    return rs485_send((rs485_inst_t *)(ctx->port_data), (void *)buf, len);
}

/**
 * Name:    dlt645_port_init
 * Brief:   645采集硬件层初始化
 * Input:   None
 * Output:  None
 */
int dlt645_port_init(void)
{
    rt_memset(&dlt645, 0, sizeof(dlt645_t));

    dlt645.read = dlt645_hw_read;
    dlt645.write = dlt645_hw_write;
    dlt645.port_data = (void*)rs485_create( DLT645_SERIAL_NAME, 
                                            DLT645_SERIAL_BAUDRATE, 
                                            DLT645_SERIAL_PARITY, 
                                            DLT645_RS485_DE_PIN, 
                                            DLT645_RS485_TX_LVL);

    rs485_set_recv_tmo((rs485_inst_t*)dlt645.port_data, 1000);
    if (rs485_connect((rs485_inst_t*)dlt645.port_data) != RT_EOK)
    {
        rs485_destory((rs485_inst_t*)dlt645.port_data);
        LOG_E("rs485 connect fail.");
        return RT_ERROR;
    }

    LOG_D("dlt645 use rs485 create success.");

    return  RT_EOK;
}

