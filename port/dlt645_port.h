#ifndef __DLT645_PORT_H
#define __DLT645_PORT_H
#include "dlt645.h"

//645采集硬件层初始化
int dlt645_port_init(dlt645_t*ctx, const char *device, int baud, char parity, uint8_t data_bit, uint8_t stop_bit, uint8_t is_rs485);

void dlt645_port_close(void *port_data);

#endif
