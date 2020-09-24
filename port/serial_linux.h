#ifndef __DLT645_PORT_H
#define __DLT645_PORT_H
#include "dlt645.h"

//波特率
typedef enum _serial_baud
{
    SERIAL_B1200,
    SERIAL_B2400,
    SERIAL_B4800,
    SERIAL_B9600,
    SERIAL_B19200,
    SERIAL_B38400,
    SERIAL_B57600,
    SERIAL_B115200,
} serial_baud_t;
//校验位
typedef enum _serial_parity
{
    PARITY_NONE,
    PARITY_ODD,
    PARITY_EVEN
} serial_parity_t;
//数据位
typedef enum _serial_data_bits
{
    DATA_BITS_5,
    DATA_BITS_6,
    DATA_BITS_7,
    DATA_BITS_8
} serial_data_bits_t;
//停止位
typedef enum _serial_stop_bits
{
    STOP_BITS_1,
    STOP_BITS_2
} serial_stop_bits_t;
//模式
typedef enum _serial_mode
{
    RS485,
    RS232
} serial_mode_t;

//645 硬件接口参数
typedef struct _serial_linux_param
{
    char *device_name;
    serial_baud_t baud;           //波特率
    serial_parity_t parity;       //校验位
    serial_data_bits_t data_bits; //数据位
    serial_stop_bits_t stop_bits; //停止位
    serial_mode_t mode;           //串口模式
} serial_linux_param_t;

dlt645_t *dlt645_new_serial_linux(serial_linux_param_t *params);

#endif
