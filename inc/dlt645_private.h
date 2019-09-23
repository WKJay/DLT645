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



#define C_CODE_MASK 0x1f
#define C_CODE_NULL 0x00
#define C_CODE_BRC 0x08 //广播校时
#define C_CODE_RD 0x11  //读数据
#define C_CODE_RDM 0x12 //读后续数据
#define C_CODE_RDA 0x13 //读通信地址
#define C_CODE_WR 0x14  //写数据
#define C_CODE_WRA 0x15 //写通信地址
#define C_CODE_DJ 0x16  //冻结
#define C_CODE_BR 0x17  //更改通信速率
#define C_CODE_PD 0x18  //修改密码
#define C_CODE_XL 0x19  //最大需量清零
#define C_CODE_DB 0x1a  //电表清零
#define C_CODE_MSG 0x1b //事件清零
#define C_TD_MASK 0x80
#define C_TD_POS 7    //传送方向位置
#define C_TD_MASTER 0 //主站发出的命令帧
#define C_TD_SLAVE 1  //从站发出的应答帧
#define C_ACK_MASK 0x40
#define C_ACK_POS 6 //从站应答位置
#define C_ACK_OK 0  //从站应答正确
#define C_ACK_ERR 1 //从站应答错误
#define C_FU_MASK 0x20
#define C_FU_POS 5
#define C_FU_NONE 0 //无后续帧 follow-up frame
#define C_FU_EXT 1  //有后续帧

#define DL645_ERR_RATE 0x40  //费率数超
#define DL645_ERR_DAY 0x20   //日时段数超
#define DL645_ERR_YEAR 0x10  //年时区数超
#define DL645_ERR_BR 0x08    //通信速率不能更改
#define DL645_ERR_PD 0x04    //密码错误/未授权
#define DL645_ERR_DATA 0x02  //无请求数据
#define DL645_ERR_OTHER 0x01 //其他错误


//645 公共校验
extern int dlt645_common_check(uint8_t *msg, int len, uint32_t addr);
//645 和校验
extern int _crc(uint8_t *msg, int len);
//645 调用底层接口接收数据
extern int dlt645_receive_msg(dlt645_t *ctx, uint8_t *msg, uint32_t addr, uint32_t code, dlt645_protocal protocal);
//645 调用底层接口发送
extern int dlt645_send_msg(dlt645_t *ctx, uint8_t *msg, int len);
//十进制转BCD码
extern uint32_t dec2bcd(uint32_t val);
//将接收到的dlt645数据包中的数据转化为整数
extern int data_package_translate_to_int(uint8_t *read_data, uint16_t len);
//根据数据格式将645协议读取的数据转换为真实数据并存储
extern int dlt645_data_parse_by_format_to_float(uint8_t *read_data, uint16_t read_len, const char *data_format, uint8_t *store_address);
#endif
