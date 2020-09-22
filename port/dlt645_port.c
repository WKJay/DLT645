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
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include "dlt645.h"

//DLT645采集使用的串口名
#define DLT645_SERIAL_NAME "/dev/ttyUSB0"

//DL/T 645硬件拓展结构体
typedef struct
{
    int fd;
} dlt645_port_t;

static dlt645_port_t dlt645_port = {
    .fd = 0};

//dlt645 环境结构体
dlt645_t dlt645;

static int dlt645_port_select(dlt645_t *ctx)
{
    int s_rc;
    fd_set rfds;
    struct timeval tv;
    dlt645_port_t *port = ctx->port_data;

    FD_ZERO(&rfds);
    FD_SET(port->fd, &rfds);
    tv.tv_sec = ctx->response_timeout / 1000;
    tv.tv_usec = (ctx->response_timeout % 1000) * 1000;

    s_rc = select(port->fd + 1, &rfds, NULL, NULL, &tv);
    if (s_rc < 0)
    {
        return -1;
    }
    else
    {
        return s_rc;
    }
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
static int dlt645_hw_read(dlt645_t *ctx, uint8_t *msg, uint16_t len)
{
    //实际接收长度
    int read_len = 0;

    dlt645_port_t *port = ctx->port_data;
    read_len = read(port->fd, msg, len);
    printf("read %d size\r\n", read_len);
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
    dlt645_port_t *port = ctx->port_data;
    int write_len = 0;

    write_len = write(port->fd, buf, len);
    return write_len;
}

void port_exit(int signo)
{
    dlt645_port_t *port = dlt645.port_data;
    if (port->fd > 0)
        close(port->fd);
    printf("\r\nclose %s\r\n", DLT645_SERIAL_NAME);
    _exit(0);
}

/**
 * Name:    dlt645_port_init
 * Brief:   645采集硬件层初始化
 * Input:   None
 * Output:  None
 */
int dlt645_port_init(void)
{
    dlt645_port_t *port = dlt645.port_data;
    signal(SIGINT, port_exit);
    //串口初始化
    port->fd = open(DLT645_SERIAL_NAME, O_RDWR | O_NOCTTY);
    if (port->fd < 0)
    {
        printf("cannot open device %s \r\n", DLT645_SERIAL_NAME);
        return -1;
    }
    else
    {
        struct termios opt;
        //清空串口接收缓冲区
        tcflush(port->fd, TCIOFLUSH);
        // 获取串口参数opt
        tcgetattr(port->fd, &opt);

        // 设置串口输出波特率
        cfsetospeed(&opt, B9600);
        //设置串口输入波特率
        cfsetispeed(&opt, B9600);
        //设置数据位数
        opt.c_cflag &= ~CSIZE;
        opt.c_cflag |= CS8;
        //校验位
        opt.c_cflag &= ~PARENB;
        opt.c_iflag &= ~INPCK;
        //设置停止位
        opt.c_cflag &= ~CSTOPB;

        opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        printf("Device %s on fd %d is set to 9600bps,8N1\n", DLT645_SERIAL_NAME, port->fd);

        //更新配置
        tcsetattr(port->fd, TCSANOW, &opt);
    }

    return 0;
}

const dlt645_backend_t dlt645_backend = {
    dlt645_port_select,
    dlt645_hw_write,
    dlt645_hw_read};

//645结构体注册
dlt645_t dlt645 = {
    {0},
    0,
    1500,
    (void *)&dlt645_port,
    &dlt645_backend};
