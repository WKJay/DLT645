#ifndef __DLT645_1997_H
#define __DLT645_1997_H

#include "dlt645.h"

//基于 DLT645 2007 协议读取数据
int dlt645_1997_read_data(dlt645_t *ctx, uint32_t code, uint8_t *read_data);
//基于 DLT645 2007 协议校验数据
int dlt645_1997_recv_check(uint8_t *msg, int len, uint8_t *addr, uint32_t code);

#endif /* __DLT645_1997_H */
