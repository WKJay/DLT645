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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include "dlt645.h"

//DL/T 645硬件拓展结构体
typedef struct
{
    int fd;
    uint8_t is_rs485;
    struct termios old_tios;
} dlt645_port_t;

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

void dlt645_port_close(void *port_data)
{
    dlt645_port_t *port = port_data;
    if (port_data)
    {
        tcsetattr(port->fd,TCSANOW,&port->old_tios);
        if (port->fd)
        {
            close(port->fd);
        }
        dlt645_free(port_data);
    }
}

const dlt645_backend_t dlt645_backend = {
    dlt645_port_select,
    dlt645_hw_write,
    dlt645_hw_read};

/**
 * Name:    dlt645_port_init
 * Brief:   645采集硬件层初始化
 * Input:   None
 * Output:  None
 */
int dlt645_port_init(dlt645_t *ctx, const char *device,
                     int baud, char parity, uint8_t data_bit, uint8_t stop_bit, uint8_t is_rs485)
{
    if (ctx == NULL)
    {
        printf("no ctx available\r\n");
        return -1;
    }
    ctx->backend = &dlt645_backend;
    dlt645_port_t *port = dlt645_malloc(sizeof(dlt645_port_t));
    if (port == NULL)
    {
        printf("no memory for dlt645_port_t\r\n");
        return -1;
    }
    memset(port, 0, sizeof(dlt645_port_t));

    //串口初始化
    port->fd = open(device, O_RDWR | O_NOCTTY);
    if (port->fd < 0)
    {
        printf("cannot open device %s \r\n", device);
        return -1;
    }
    else
    {
        struct termios opt;
        int baudrate = 0;
        tcgetattr(port->fd, &port->old_tios);
        memset(&opt, 0, sizeof(struct termios));

        //清空串口接收缓冲区
        tcflush(port->fd, TCIOFLUSH);

        // 设置串口输出波特率
        switch (baud)
        {
        case 1200:
            baudrate = B1200;
            break;
        case 2400:
            baudrate = B2400;
            break;
        case 4800:
            baudrate = B4800;
            break;
        case 9600:
            baudrate = B9600;
            break;
        case 19200:
            baudrate = B19200;
            break;
        case 38400:
            baudrate = B38400;
            break;
        case 57600:
            baudrate = B57600;
            break;
        case 115200:
            baudrate = B115200;
            break;
        default:
            printf("unknown baud rate :%d, use default 9600\r\n", baud);
            baudrate = B9600;
            break;
        }

        if ((cfsetospeed(&opt, baudrate) < 0) ||
            (cfsetispeed(&opt, baudrate) < 0))
        {
            close(port->fd);
            dlt645_free(port);
            return -1;
        }

        //设置数据位数
        opt.c_cflag &= ~CSIZE;
        switch (data_bit)
        {
        case 5:
            opt.c_cflag |= CS5;
            break;
        case 6:
            opt.c_cflag |= CS6;
            break;
        case 7:
            opt.c_cflag |= CS7;
            break;
        case 8:
        default:
            opt.c_cflag |= CS8;
            break;
        }

        //校验位
        if (parity == 'N')
        {
            /* None */
            opt.c_cflag &= ~PARENB;
        }
        else if (parity == 'E')
        {
            /* Even */
            opt.c_cflag |= PARENB;
            opt.c_cflag &= ~PARODD;
        }
        else
        {
            /* Odd */
            opt.c_cflag |= PARENB;
            opt.c_cflag |= PARODD;
        }

        if (parity == 'N')
        {
            opt.c_iflag &= ~INPCK;
        }
        else
        {
            opt.c_iflag |= INPCK;
        }

        //设置停止位
        if (stop_bit == 1)
        {
            opt.c_cflag &= ~CSTOPB;
        }
        else
        {
            opt.c_cflag |= CSTOPB;
        }
        /* Software flow control is disabled */
        opt.c_iflag &= ~(IXON | IXOFF | IXANY);
        /* Raw ouput */
        opt.c_oflag &= ~OPOST;
        opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        printf("Device %s on fd %d is set to 9600bps,8N1\n", device, port->fd);

        //更新配置
        if (tcsetattr(port->fd, TCSANOW, &opt) < 0)
        {
            close(port->fd);
            dlt645_free(port);
            return -1;
        }
    }
    ctx->port_data = port;
    return 0;
}
