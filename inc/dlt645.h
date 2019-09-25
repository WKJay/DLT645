#ifndef _DLT645_H
#define _DLT645_H

#include <stdint.h>

#define DL645_MAX_READ_LEN 200 //读数据的最大数据长度
#define DL645_MAX_WRITE_LEN 50 //写数据的最大数据长度

// port setting
#define DLT645_DEFAULT_RESPONSE_TIMEOUT 500 //500ms
#define MAX_DEVICE_NAME_LEN 10              //最大设备名长度

#define dlt_malloc rt_malloc
#define dlt_free rt_free

//DLT645 环境结构体
typedef struct dlt645
{
    uint8_t addr[6];    //从机地址
    uint8_t debug;      //调试标志
    int (*write)(struct dlt645 *ctx, uint8_t *buf, uint16_t len);     //底层写函数
    int (*read) (struct dlt645 *ctx, uint8_t *msg);                   //底层读函数
    void *port_data;                                            //移植层拓展接口
} dlt645_t;

typedef enum
{
    DLT645_2007 = 1,
    DLT645_1997
} dlt645_protocal;

//设置从机地址
extern void dlt645_set_addr(dlt645_t *ctx, uint8_t *addr);
//设置调试开关
extern int dlt645_set_debug(dlt645_t *ctx, int flag);
//数据采集
extern int dlt645_read_data(dlt645_t *ctx, uint32_t code, uint8_t *read_data, dlt645_protocal protocal);
//十进制转BCD码(32位)
extern uint32_t dec_to_bcd(uint32_t val);
//字符串转BCD形式
extern int str_to_bcd(char *str, uint8_t *bcd_store_address, uint16_t bcd_len);

#endif
