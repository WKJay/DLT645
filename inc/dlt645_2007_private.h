#ifndef __DLT645_2007_PRIVATE_H
#define __DLT645_2007_PRIVATE_H


#define DL645_2007_RD_CMD_LEN 16 //读取数据命令的长度

//功能码
#define C_2007_CODE_BRC 0x08    //广播校时
#define C_2007_CODE_RD 0X11     //读数据
#define C_2007_CODE_RDM 0x12    //读后续数据
#define C_2007_CODE_RDA 0x13    //读通信地址
#define C_2007_CODE_WR 0x14     //写数据
#define C_2007_CODE_WRA 0x15    //写通信地址
#define C_2007_CODE_DJ 0x16     //冻结
#define C_2007_CODE_BR 0x17     //更改通信速率
#define C_2007_CODE_PD 0x18     //修改密码
#define C_2007_CODE_XL 0x19     //最大需量清零
#define C_2007_CODE_DB 0x1a     //电表清零
#define C_2007_CODE_MSG 0x1b    //事件清零


#define ERR_2007_RATE 0x40  //费率数超
#define ERR_2007_DAY 0x20   //日时段数超
#define ERR_2007_YEAR 0x10  //年时区数超
#define ERR_2007_BR 0x08    //通信速率不能更改
#define ERR_2007_PD 0x04    //密码错误/未授权
#define ERR_2007_DATA 0x02  //无请求数据
#define ERR_2007_OTHER 0x01 //其他错误

/*DLT 645 2007数据标识*/
#define DIC_0 0x0         //组合有功总电能
#define DIC_100 0x100     //组合有功费率1电能
#define DIC_200 0x200     //组合有功费率2电能
#define DIC_300 0x300     //组合有功费率3电能
#define DIC_400 0x400     //组合有功费率4电能
#define DIC_10000 0x10000 //正向有功总电能
#define DIC_10100 0x10100 //正向有功费率1电能
#define DIC_10200 0x10200 //正向有功费率2电能
#define DIC_10300 0x10300 //正向有功费率3电能
#define DIC_10400 0x10400 //正向有功费率4电能
#define DIC_20000 0x20000 //反向有功总电能
#define DIC_20100 0x20100 //反向有功费率1电能
#define DIC_20200 0x20200 //反向有功费率2电能
#define DIC_20300 0x20300 //反向有功费率3电能
#define DIC_20400 0x20400 //反向有功费率4电能
#define DIC_30000 0x30000 //组合无功1总电能
#define DIC_40000 0x40000 //组合无功2总电能
#define DIC_50000 0x50000 //第一象限无功电能
#define DIC_60000 0x60000 //第二象限无功电能
#define DIC_70000 0x70000 //第三象限无功电能
#define DIC_80000 0x80000 //第四象限无功电能
#define DIC_90000 0x90000 //正向视在总电能

#define DIC_2010100 0x2010100 //A相电压
#define DIC_2010200 0x2010200 //B相电压
#define DIC_2010300 0x2010300 //C相电压
#define DIC_2020100 0x2020100 //A相电流
#define DIC_2020200 0x2020200 //B相电流
#define DIC_2020300 0x2020300 //C相电流
#define DIC_2030000 0x2030000 //总有功功率
#define DIC_2030100 0x2030100 //A相有功功率
#define DIC_2030200 0x2030200 //B相有功功率
#define DIC_2030300 0x2030300 //C相有功功率
#define DIC_2040000 0x2040000 //总无功功率
#define DIC_2040100 0x2040100 //A相无功功率
#define DIC_2040200 0x2040200 //B相无功功率
#define DIC_2040300 0x2040300 //C相无功功率
#define DIC_2050000 0x2050000 //总视在功率
#define DIC_2050100 0x2050100 //A相视在功率
#define DIC_2050200 0x2050200 //B相视在功率
#define DIC_2050300 0x2050300 //C相视在功率
#define DIC_2060000 0x2060000 //总功率因素
#define DIC_2060100 0x2060100 //A相功率因素
#define DIC_2060200 0x2060200 //B相功率因素
#define DIC_2060300 0x2060300 //C相功率因素
#define DIC_20C0100 0x20C0100 //AB线电压
#define DIC_20C0200 0x20C0200 //BC线电压
#define DIC_20C0300 0x20C0300 //CA线电压
#define DIC_2800002 0x2800002 //频率
#define DIC_4000101 0x4000101 //年月日星期
#define DIC_4000102 0x4000102 //时分秒
#define DIC_5060001 0x5060001 //上一次日冻结时间
#define DIC_5060101 0x5060101 //上一次日冻结正向有功电能

#define DIC_30C0000 0x30C0000 //过流总次数，总时间
#define DIC_30C0101 0x30C0101 //上一次A相过流记录

#define DIC_3300100 0x3300100 //电表清零总次数
#define DIC_3300101 0x3300101 //电表清零记录

#define DIC_4000501 0x4000501
#define DIC_4000502 0x4000502
#define DIC_4000503 0x4000503
#define DIC_4000504 0x4000504
#define DIC_4000505 0x4000505
#define DIC_4000506 0x4000506
#define DIC_4000507 0x4000507

#define DIC_4000403 0x4000403 //资产管理编码

#define DIC_4000701 0x4000701 //信号强度

#define DIC_4000702 0x4000702 //版本号

#define DIC_7000001 0x7000001 //master_api_key
#define DIC_7000002 0x7000002 //device_id

#endif /* __DLT645_2007_PRIVATE_H */
