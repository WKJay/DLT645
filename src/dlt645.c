/*************************************************
 Copyright (c) 2019
 All rights reserved.
 File name:     dlt645.c
 Description:   
 History:
 1. Version:    
    Date:       2019-09-20
    Author:     wangjunjie
    Modify:     
*************************************************/
#include "dlt645_private.h"
#include "dlt645_1997.h"
#include "dlt645_2007.h"
#include "string.h"

/**
 * Name:    dlt645_receive_msg
 * Brief:   645协议调用底层接收数据
 * Input:
 *  @ctx:       645环境句柄
 *  @msg:       数据包存储地址
 *  @addr:      从站地址
 *  @code:      数据标识   
 *  @protocal:  645协议类型  
 * Output:  接收成功：0，接收失败：-1
 */
int dlt645_receive_msg(dlt645_t *ctx, uint8_t *msg, uint32_t addr, uint32_t code, dlt645_protocal protocal)
{
    int msg_len = ctx->read(ctx, msg);

    if (protocal == DLT645_1997)
    {
        return dlt645_1997_recv_check(msg, msg_len, addr, code);
    }
    else if (protocal == DLT645_2007)
    {
        return dlt645_2007_recv_check(msg, msg_len, addr, code);
    }
    else
    {
        return -1;
    }
}

/**
 * Name:    dlt645_send_msg
 * Brief:   645协议调用底层发送数据
 * Input:
 *  @ctx:       645环境句柄
 *  @msg:       发送的数据首地址
 *  @len:       发送长度 
 * Output:  实际发送的字节数，错误返回-1
 */
int dlt645_send_msg(dlt645_t *ctx, uint8_t *msg, int len)
{
    msg[0] = DL645_START_CODE;
    msg[DL645_ADDR_LEN + 1] = DL645_START_CODE;
    msg[len - 1] = DL645_STOP_CODE;
    msg[len - 2] = _crc(msg, len - 2);

    return ctx->write(ctx, msg, len);
}

/**
 * Name:    dlt645_set_addr
 * Brief:   设置从站地址
 * Input:
 *  @ctx:       645环境句柄
 *  @addr:      从站地址
 * Output:  None
 */
void dlt645_set_addr(dlt645_t *ctx, uint8_t *addr)
{
    memcpy(ctx->addr, addr, DL645_ADDR_LEN);
}


/**
 * Name:    dlt645_set_debug
 * Brief:   设置调试模式
 * Input:
 *  @ctx:       645环境句柄
 *  @flag:      调试标志
 * Output:  None
 */
int dlt645_set_debug(dlt645_t *ctx, int flag)
{
    ctx->debug = flag;
    return 0;
}


/**
 * Name:    dlt645_read_data（用户调用）
 * Brief:   根据645协议类型读取数据
 * Input:
 *  @ctx:           645环境句柄
 *  @addr:          从站地址
 *  @code:          标识符
 *  @read_data:     读取数据存储地址
 *  @protocal:      协议类型
 * Output:  成功返回数据长度，失败返回-1
 */
int dlt645_read_data(dlt645_t *ctx,
                     uint32_t addr,
                     uint32_t code,
                     uint8_t *read_data,
                     dlt645_protocal protocal)
{
    int rs = -1;
    switch (protocal)
    {
    case DLT645_1997:
        rs = dlt645_1997_read_data(ctx, addr, code, read_data);
        break;
    case DLT645_2007:
        rs = dlt645_2007_read_data(ctx, addr, code, read_data);
        break;
    default:
        DLT645_LOG("unrecognized protocal!\r\n");
        break;
    }
    return rs;
}

