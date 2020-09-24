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

#include "serial_linux.h"

//DL/T 645硬件拓展结构体
typedef struct
{
    int fd;
    uint8_t is_rs485;
    struct termios old_tios;
} serial_port_t;

static int serial_select(dlt645_t *ctx)
{
    int s_rc;
    fd_set rfds;
    struct timeval tv;
    serial_port_t *port = ctx->port_data;

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
 * Name:    serial_linux_hw_read
 * Brief:   dlt645 硬件层接收数据
 * Input:
 *  @ctx:   645运行环境
 *  @msg:   接收数据存放地址
 *  @len:   数据最大接收长度 
 * Output:  读取数据的长度 -1:error 0:timeout
 */
static int serial_linux_hw_read(dlt645_t *ctx, uint8_t *msg, uint16_t len)
{
    //实际接收长度
    int rc = 0;
    serial_port_t *port = ctx->port_data;

    rc = serial_select(ctx);
    if (rc < 0)
    {
        printf("select error on fd %d\r\n", port->fd);
        return -1;
    }
    else if (rc == 0)
    {
        printf("select timeout on fd %d\r\n", port->fd);
        return -0;
    }

    rc = read(port->fd, msg, len);
    return rc;
}

/**
 * Name:    serial_linux_hw_write
 * Brief:   dlt645 硬件层发送数据
 * Input:
 *  @ctx:   645运行环境
 *  @buf:   待发送数据
 *  @len:   发送长度
 * Output:  实际发送的字节数，错误返回-1
 */
static int serial_linux_hw_write(dlt645_t *ctx, uint8_t *buf, uint16_t len)
{
    serial_port_t *port = ctx->port_data;
    int write_len = 0;
    write_len = write(port->fd, buf, len);
    return write_len;
}

static void _port_close(serial_port_t *port)
{
    if (port)
    {
        tcsetattr(port->fd, TCSANOW, &port->old_tios);
        if (port->fd)
        {
            if(close(port->fd)==0)
            {
                printf("close fd:%d success\r\n", port->fd);
            }
            else
            {
                 printf("close fd:%d failed\r\n", port->fd);
            }
            
        }
    }
}

static void serial_linux_hw_close(dlt645_t *ctx)
{
    if (ctx == NULL)
    {
        return;
    }
    if (ctx->port_data)
    {
        _port_close(ctx->port_data);
        free(ctx->port_data);
        ctx->port_data = NULL;
    }
    free(ctx);
    ctx = NULL;
}

const dlt645_backend_t serial_linux_backend = {
    serial_linux_hw_close,
    serial_linux_hw_write,
    serial_linux_hw_read};

/**
 * Name:    serial_linux_port_init
 * Brief:   硬件层初始化
 * Input:   None
 * Output:  None
 */
static int serial_linux_port_init(dlt645_t *ctx, void *param)
{
    serial_linux_param_t *port_param = param;
    if (ctx == NULL)
    {
        printf("no ctx available\r\n");
        return -1;
    }
    serial_port_t *port = dlt645_malloc(sizeof(serial_port_t));
    if (port == NULL)
    {
        printf("no memory for serial_port_t\r\n");
        return -1;
    }
    memset(port, 0, sizeof(serial_port_t));

    //串口初始化
    port->fd = open(port_param->device_name, O_RDWR | O_NOCTTY);
    if (port->fd < 0)
    {
        printf("cannot open device %s \r\n", port_param->device_name);
        dlt645_free(port);
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
        switch (port_param->baud)
        {
        case SERIAL_B1200:
            baudrate = B1200;
            break;
        case SERIAL_B2400:
            baudrate = B2400;
            break;
        case SERIAL_B4800:
            baudrate = B4800;
            break;
        case SERIAL_B9600:
            baudrate = B9600;
            break;
        case SERIAL_B19200:
            baudrate = B19200;
            break;
        case SERIAL_B38400:
            baudrate = B38400;
            break;
        case SERIAL_B57600:
            baudrate = B57600;
            break;
        case SERIAL_B115200:
            baudrate = B115200;
            break;
        default:
            printf("unknown baud rate :%d, use default 9600\r\n", port_param->baud);
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
        switch (port_param->data_bits)
        {
        case DATA_BITS_5:
            opt.c_cflag |= CS5;
            break;
        case DATA_BITS_6:
            opt.c_cflag |= CS6;
            break;
        case DATA_BITS_7:
            opt.c_cflag |= CS7;
            break;
        case DATA_BITS_8:
        default:
            opt.c_cflag |= CS8;
            break;
        }

        //校验位
        switch (port_param->parity)
        {
        case PARITY_NONE:
            opt.c_cflag &= ~PARENB;
            opt.c_iflag &= ~INPCK;
            break;
        case PARITY_EVEN:
            opt.c_cflag |= PARENB;
            opt.c_cflag &= ~PARODD;
            opt.c_iflag |= INPCK;
            break;
        case PARITY_ODD:
            opt.c_cflag |= PARENB;
            opt.c_cflag |= PARODD;
            opt.c_iflag |= INPCK;
            break;
        default:
            break;
        }

        //设置停止位
        if (port_param->stop_bits == STOP_BITS_1)
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

        printf("Device %s on fd %d is set to 9600bps,8N1\n", port_param->device_name, port->fd);

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

dlt645_t *dlt645_new_serial_linux(serial_linux_param_t *params)
{
    //create dlt645 context
    dlt645_t *ctx = malloc(sizeof(dlt645_t));
    if (ctx == NULL)
    {
        printf("no memory for dlt645 ctx,malloc failed\r\n");
        return NULL;
    }
    //set default params
    dlt645_ctx_init_default(ctx);
    //set backend
    dlt645_set_backend(ctx, &serial_linux_backend);
    //hardware init
    if (serial_linux_port_init(ctx, params) < 0)
    {
        free(ctx);
        return NULL;
    }
    return ctx;
}
