#ifndef __DLT645_1997_PRIVATE_H
#define __DLT645_1997_PRIVATE_H

#define DL645_1997_RD_CMD_LEN 14 //读取数据命令的长度

//功能码
#define C_1997_CODE_BRC 0x08    //广播校时
#define C_1997_CODE_RD 0x01     //读数据
#define C_1997_CODE_RDM 0x02    //读后续数据
#define C_1997_CODE_WR 0x04     //写数据
#define C_1997_CODE_WRA 0x0A    //写设备地址
#define C_1997_CODE_BR 0x0C     //更改通信速率
#define C_1997_CODE_PD 0x0F     //修改密码
#define C_1997_CODE_XL 0x10     //最大需量清零
#define C_1997_CEODE_RR 0x03    //重读数据


#define ERR_1997_RATE 0x40  //费率数超
#define ERR_1997_DAY 0x20   //日时段数超
#define ERR_1997_YEAR 0x10  //年时区数超
#define ERR_1997_BR 0x08    //通信速率不能更改
#define ERR_1997_PD 0x04    //密码错误/未授权
#define ERR_1997_DATA 0x02  //无请求数据
#define ERR_1997_OTHER 0x01 //其他错误

// DLT645 1997 数据标识
#define DIC_B611 0xB611 //A相电压
#define DIC_B612 0xB612 //B相电压
#define DIC_B613 0xB613 //C相电压
#define DIC_B691 0xB691 //AB线电压
#define DIC_B692 0xB692 //BC线电压
#define DIC_B693 0xB693 //CA线电压

#define DIC_B621 0xB621 //A相电流
#define DIC_B622 0xB622 //B相电流
#define DIC_B623 0xB623 //C相电流

#define DIC_B630 0xB630 //总有功功率
#define DIC_B631 0xB631 //A相有功功率
#define DIC_B632 0xB632 //B相有功功率
#define DIC_B633 0xB633 //C相有功功率

#define DIC_B640 0xB640 //总无功功率
#define DIC_B641 0xB641 //A相无功功率
#define DIC_B642 0xB642 //B相无功功率
#define DIC_B643 0xB643 //C相无功功率

#define DIC_B660 0xB660 //总视在功率
#define DIC_B661 0xB661 //A相视在功率
#define DIC_B662 0xB662 //B相视在功率
#define DIC_B663 0xB663 //C相视在功率

#endif /* __DLT645_1997_PRIVATE_H */
