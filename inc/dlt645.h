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
typedef struct
{
    uint8_t addr[6];    //从机地址
    uint8_t debug;      //调试标志
    uint16_t timeout;   //接收超时
    uint16_t nfds;      //串口文件句柄
    
    /*以下为底层操作接口，需用户根据自己的硬件平台实现*/
    int (*write)(int nfds, uint8_t *buf, uint16_t len);     //底层写函数
    int (*read)(int nfds, uint8_t *msg, uint16_t timeout);  //底层读函数
    int (*flush)(void);                                     //底层清缓存函数
} dlt645_t;

typedef enum
{
    DLT645_2007 = 1,
    DLT645_1997
} dlt645_protocal;

extern void dlt645_set_addr(dlt645_t *ctx, uint8_t *addr);
extern void dlt645_set_nfds(dlt645_t *ctx, int nfds);
extern int dlt645_connect(dlt645_t *ctx);
extern void dlt645_free(dlt645_t *ctx);
extern int dlt645_set_debug(dlt645_t *ctx, int flag);
extern int dlt645_set_byte_timeout(dlt645_t *ctx, uint32_t to_sec, uint32_t to_usec);
extern int dlt645_set_response_timeout(dlt645_t *ctx, uint16_t timeout);
extern int dlt645_read_data(dlt645_t *ctx, uint32_t addr, uint32_t code, uint8_t *read_data, dlt645_protocal protocal);
extern int dlt645_write_data(dlt645_t *ctx, uint32_t addr, uint32_t code, uint8_t *write_data, uint8_t write_len);
#endif
