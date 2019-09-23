#ifndef __DLT645_PORT_H
#define __DLT645_PORT_H
#include "dlt645.h"

//对外提供环境声明
extern dlt645_t dlt645;
//645采集硬件层初始化
int dlt645_port_init(void);

#endif
