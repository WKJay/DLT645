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
#include <string.h>
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
int dlt645_common_check(uint8_t *msg, int len, uint8_t *addr)
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
    if (memcmp(msg + 1, addr, 6) != 0)
    {
        return -1;
    }

    return 0;
}

/**
 * Name:    dec2bcd
* Brief:   十进制转BCD码（目前支持32位数字大小）
 * Input:
 *  @val:   十进制值
 * Output:  BCD码值
 */
uint32_t dec_to_bcd(uint32_t val)
{
    uint32_t data = 0;

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
 * Name:    str_to_bcd
 * Brief:   字符串转BCD形式
 * Input:
 *  @str:               要转换的字符串
 *  @bcd_store_address: 转换后的存储地址
 *  @bcd_len:           BCD码总长度
 * Output:  成功0，失败-1
 */
int str_to_bcd(char *str, uint8_t *bcd_store_address, uint16_t bcd_len)
{
    //字符串偏移
    int str_pos = bcd_len * 2 - strlen(str);
    //字符串比BCD码长度长
    if (str_pos < 0)
    {
        return -1;
    }
    memset(bcd_store_address, 0, bcd_len);

    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            bcd_store_address[(i + str_pos) / 2] |= (str[i] - '0') << (4 * ((i + 1 + (strlen(str) % 2)) % 2));
        }
        else
        {
            //当前字符不为数字，返回错误
            return -1;
        }
    }
    return 0;
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
        current_index++;
    }
    return i_value;
}

/**
 * Name:    dlt645_data_parse_by_format_to_float
 * Brief:   根据数据格式将645协议读取的数据转换为真实数据并存储
 *          ！真实数据为浮点数据，需要注意的是无论读取数据长度是多少，存储数据长度都应是4字节
 * Input:
 *  @read_data:     645协议读取的数据
 *  @read_len:      读取数据的长度
 *  @data_format:   转换的数据格式，如 XX.XX,XX.XXX
 * Output:  转换成功返回0，失败返回-1
 */
int dlt645_data_parse_by_format_to_float(uint8_t *read_data, uint16_t read_len, const char *data_format, uint8_t *store_address)
{
    //权值
    int num_weight = 0;
    int ival = data_package_translate_to_int(read_data, read_len);

    for (int i = 0; i < strlen(data_format); i++)
    {
        if (*(data_format + i) == '.')
        {
            num_weight = strlen(data_format) - i - 1;
            if (num_weight < 0)
            {
                return -1;
            }
            break;
        }
    }
    float fval = ival / pow(10, num_weight);
    memcpy(store_address, &fval, 4);
    return 0;
}
