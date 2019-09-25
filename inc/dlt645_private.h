#ifndef _DLT645_PRIVATE_H
#define _DLT645_PRIVATE_H

#include "dlt645.h"

#ifdef DLT645_DEBUG
#define DLT645_LOG kprintf
#else
#define DLT645_LOG(...)
#endif

#define DL645_START_CODE 0x68
#define DL645_STOP_CODE 0x16

#define DL645_PREMBLE_ENABLE 0 //前导码使能标记
#define DL645_PREMBLE 0xFE     //前导码

#define DL645_ADDR_LEN 6    //设备地址长度
#define DL645_CONTROL_POS 8 //控制码位置
#define DL645_LEN_POS 9     //长度位置
#define DL645_DATA_POS 10   //数据位置
#define DL645_WR_LEN 50     //写入数据命令的长度
#define DL645_RESP_LEN 60   //读取数据命令的长度

#define C_TD_MASK 0x80      //主从标志位
#define C_TD_POS 7          //主从标志位比特位
#define C_TD_MASTER 0       //主站发出的命令帧
#define C_TD_SLAVE 1        //从站发出的应答帧

#define C_ACK_MASK 0x40     //从站是否正确应答标志位
#define C_ACK_POS 6         //从站应答标志位比特位
#define C_ACK_OK 0          //从站应答正确
#define C_ACK_ERR 1         //从站应答错误

#define C_FU_MASK 0x20      //是否有后续帧标志位
#define C_FU_POS 5          //后续帧标志位比特位
#define C_FU_NONE 0         //无后续帧
#define C_FU_EXT 1          //有后续帧

#define C_CODE_MASK 0x1F    //功能码标志位



//645 公共校验
extern int dlt645_common_check(uint8_t *msg, int len, uint8_t *addr);
//645 和校验
extern int _crc(uint8_t *msg, int len);
//645 调用底层接口接收数据
extern int dlt645_receive_msg(dlt645_t *ctx, uint8_t *msg, uint16_t len, uint32_t code, dlt645_protocal protocal);
//645 调用底层接口发送
extern int dlt645_send_msg(dlt645_t *ctx, uint8_t *msg, int len);
//将接收到的dlt645数据包中的数据转化为整数
extern int data_package_translate_to_int(uint8_t *read_data, uint16_t len);
//根据数据格式将645协议读取的数据转换为真实数据并存储
extern int dlt645_data_parse_by_format_to_float(uint8_t *read_data, uint16_t read_len, const char *data_format, uint8_t *store_address);
#endif
