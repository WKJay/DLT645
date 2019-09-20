#ifndef __DLT645_1997_PRIVATE_H
#define __DLT645_1997_PRIVATE_H

#define DL645_1997_RD_LEN 14 //读取数据命令的长度
#define DL645_1997_C_RD 0x01    //读数据操作码
#define DL645_1997_C_WR 0x04    //写数据操作码

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
