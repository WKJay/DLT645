/*************************************************
 Copyright (c) 2019
 All rights reserved.
 File name:     dlt645_data.c
 Description:   DLT645 数据处理源文件
 History:
 1. Version:    
    Date:       2019-09-20
    Author:     wangjunjie
    Modify:     
*************************************************/
#include "dlt645_private.h"
#include <math.h>

//字节位置枚举类型
typedef enum
{
    BYTE_RESET = 0,   //重置
    BYTE_LOW_ADDRESS, //低位
    BYTE_HIGH_ADDRESS //高位
} byte_part;

/**
 * Name:    _crc
 * Brief:   crc和校验
 * Input:
 *  @msg:   校验数据包
 *  @len:   数据包长度
 * Output:  校验值
 */
int _crc(uint8_t *msg, int len)
{
    uint8_t crc = 0;
    while (len--)
    {
        crc += *msg++;
    }
    return crc;
}

/**
 * Name:    dlt645_common_check
 * Brief:   645协议接收数据公共部分校验
 * Input:
 *  @msg:   校验数据包
 *  @len:   数据包长度
 *  @addr:  从站地址
 * Output:  校验成功：0，校验失败：-1
 */
int dlt645_common_check(uint8_t *msg, int len, uint32_t addr)
{
    //数据包长度校验
    if (len < 7)
    {
        return -1;
    }
    //数据帧标志校验
    if (msg[0] != DL645_START_CODE ||
        msg[DL645_ADDR_LEN + 1] != DL645_START_CODE ||
        msg[len - 1] != DL645_STOP_CODE)
    {
        DLT645_LOG("check code error!\n");
        return -1;
    }
    //CRC校验
    uint8_t crc = _crc(msg, len - 2);
    if (crc != msg[len - 2])
    {
        DLT645_LOG("check crc error!\n");
        return -1;
    }
    //控制码主从校验
    if ((msg[DL645_CONTROL_POS] & C_TD_MASK) == (C_TD_MASTER << C_TD_POS))
    {
        DLT645_LOG("check control direction error!\n");
        return -1;
    }
    //控制码应答校验
    if ((msg[DL645_CONTROL_POS] & C_ACK_MASK) == (C_ACK_ERR << C_ACK_POS))
    {
        DLT645_LOG("check ACK error!\n");
        return msg[len - 3];
    }
    //从站地址校验
    if (*((uint32_t *)(msg + 1)) != addr)
    {
        return -1;
    }

    return 0;
}

/**
 * Name:    dec2bcd
 * Brief:   十进制转BCD码
 * Input:
 *  @val:   十进制值
 * Output:  BCD码值
 */
uint32_t dec2bcd(uint32_t val)
{
    uint32_t data;
    if (val < 100)
    {
        uint8_t byte0 = val % 10;
        uint8_t byte1 = val / 10;
        data = (byte1 << 4) + byte0;
    }
    else if (val < 10000)
    {
        uint8_t byte0 = val % 10;
        uint8_t byte1 = (val / 10) % 10;
        uint8_t byte2 = (val / 100) % 10;
        uint8_t byte3 = (val / 1000) % 10;
        data = (byte3 << 12) +
               (byte2 << 8) +
               (byte1 << 4) + byte0;
    }
    else if (val < 1000000)
    {
        uint8_t byte0 = val % 10;
        uint8_t byte1 = (val / 10) % 10;
        uint8_t byte2 = (val / 100) % 10;
        uint8_t byte3 = (val / 1000) % 10;
        uint8_t byte4 = (val / 10000) % 10;
        uint8_t byte5 = (val / 100000) % 10;
        data = (byte5 << 20) +
               (byte4 << 16) +
               (byte3 << 12) +
               (byte2 << 8) +
               (byte1 << 4) + byte0;
    }
    else if (val < 100000000)
    {
        uint8_t byte0 = val % 10;
        uint8_t byte1 = (val / 10) % 10;
        uint8_t byte2 = (val / 100) % 10;
        uint8_t byte3 = (val / 1000) % 10;
        uint8_t byte4 = (val / 10000) % 10;
        uint8_t byte5 = (val / 100000) % 10;
        uint8_t byte6 = (val / 1000000) % 10;
        uint8_t byte7 = (val / 10000000) % 10;
        data = (byte7 << 28) +
               (byte6 << 24) +
               (byte5 << 20) +
               (byte4 << 16) +
               (byte3 << 12) +
               (byte2 << 8) +
               (byte1 << 4) + byte0;
    }
    return data;
}

/**
 * Name:    data_package_translate_to_int
 * Brief:   将接收到的dlt645数据包中的数据转化为整数
 * Input:
 *  @read_data: 数据首地址
 *  @len:       数据长度
 * Output:  转化后的整数
 */
int data_package_translate_to_int(uint8_t *read_data, uint16_t len)
{
    //权值
    uint8_t number_weight = 0;
    //当前数组下标索引
    uint8_t current_index = 0;
    //当前解析字节位
    uint8_t current_byte_part = BYTE_RESET;
    //当前整数值
    int i_value = 0;

    while (len--)
    {
        current_byte_part = BYTE_LOW_ADDRESS;
        do
        {
            switch (current_byte_part)
            {
            //当前处理字节低位
            case BYTE_LOW_ADDRESS:
                i_value += ((read_data[current_index] - 0x33) & 0x0f) * pow(10, number_weight);
                number_weight++;
                current_byte_part = BYTE_HIGH_ADDRESS;
                break;
            //当前处理字节高位
            case BYTE_HIGH_ADDRESS:
                i_value += ((read_data[current_index] - 0x33) >> 4) * pow(10, number_weight);
                number_weight++;
                current_byte_part = BYTE_RESET;
                break;
            }
        } while (current_byte_part != BYTE_RESET);
        current_index ++;
    }
    return i_value;
}
