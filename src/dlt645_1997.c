/*************************************************
 Copyright (c) 2019
 All rights reserved.
 File name:     dlt645_1997.c
 Description:   DLT645 1997版本
 History:
 1. Version:    
    Date:       2019-09-19
    Author:     wangjunjie
    Modify:     
*************************************************/
#include "dlt645_private.h"
#include "dlt645_1997_private.h"
#include "string.h"

/**
 * Name:    dlt645_1997_recv_check
 * Brief:   DLT645-1997 数据校验
 * Input:
 *  @msg:   校验数据包
 *  @len:   数据包长度
 *  @addr:  从站地址
 *  @code:  操作码
 * Output:  校验成功：0 ，失败 -1
 */
int dlt645_1997_recv_check(uint8_t *msg, int len, uint8_t* addr, uint32_t code)
{
    if (dlt645_common_check(msg, len, addr) < 0)
    {
        return -1;
    }
    if (msg[DL645_CONTROL_POS] == 0x84)
        return 0;

    uint8_t *code_buf = (uint8_t *)&code;

    for (uint8_t i = 0; i < 2; i++)
    {
        code_buf[i] += 0x33;
    }

    if (*((uint16_t *)(msg + DL645_DATA_POS)) != code)
        return -1;

    return 0;
}

/**
 * Name:    dlt645_1997_parsing_data
 * Brief:   DLT645-1997 数据包解析
 * Input:
 *  @code:          标识符
 *  @read_data:     数据包指针
 *  @len:           数据包长度
 *  @real_val:      数据存储地址
 * Output:  数据包长度
 */
static int dlt645_1997_parsing_data(uint32_t code, uint8_t *read_data, uint16_t len, uint8_t *real_val)
{
    switch (code)
    {
    case DIC_B611:
    case DIC_B612:
    case DIC_B613:
    case DIC_B691:
    case DIC_B692:
    case DIC_B693:
    {
        dlt645_data_parse_by_format_to_float(read_data, 2, "XXX", real_val);
        break;
    }
    case DIC_B621:
    case DIC_B622:
    case DIC_B623:
    {
        dlt645_data_parse_by_format_to_float(read_data, 2, "XX.XX", real_val);
        break;
    }
    case DIC_B630:
    case DIC_B631:
    case DIC_B632:
    case DIC_B633:
    {
        dlt645_data_parse_by_format_to_float(read_data, 3, "XX.XXXX", real_val);
        break;
    }
    default:
    {
        for (uint16_t i = 0; i < len; i++)
        {
            real_val[i] = ((read_data[i] - 0x33) & 0x0f) + ((read_data[i] - 0x33) >> 4) * 10;
        }
        break;
    }
    }
    return len;
}

/**
 * Name:    dlt645_1997_read_data
 * Brief:   DLT645-1997 数据读取
 * Input:
 *  @ctx:           645句柄
 *  @addr:          从站地址
 *  @code:          数据标识
 *  @read_data:     数据存储地址
 * Output:  成功返回数据长度，失败返回-1
 */
int dlt645_1997_read_data(dlt645_t *ctx,
                          uint32_t code,
                          uint8_t *read_data)
{
    uint8_t send_buf[DL645_1997_RD_CMD_LEN];
    uint8_t read_buf[DL645_RESP_LEN];

    memset(read_buf, 0, sizeof(read_buf));
    memset(send_buf, 0, sizeof(send_buf));

    memcpy(send_buf + 1, ctx->addr, DL645_ADDR_LEN);
    send_buf[DL645_CONTROL_POS] = C_1997_CODE_RD;
    send_buf[DL645_LEN_POS] = 2;

    uint8_t send_code[2] = {0};
    send_code[0] = (code & 0xff) + 0x33;
    send_code[1] = ((code >> 8) & 0xff) + 0x33;
    memcpy(send_buf + DL645_DATA_POS, send_code, 2);

    if (dlt645_send_msg(ctx, send_buf, DL645_1997_RD_CMD_LEN) < 0)
    {
        DLT645_LOG("send data error!\n");
        return -1;
    }

    if (dlt645_receive_msg(ctx, read_buf, DL645_RESP_LEN, code, DLT645_1997) < 0)
    {
        DLT645_LOG("receive msg error!\n");
        return -1;
    }

    return dlt645_1997_parsing_data(code, read_buf + DL645_DATA_POS + 2, read_buf[DL645_LEN_POS] - 2, read_data);
}
