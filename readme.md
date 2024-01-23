# DL/T 645 采集软件包使用说明

本软件包用于 DL/T 645 协议的采集与数据处理。在硬件层的移植（主要针对于串口收发数据）完成之后， **用户仅需调用一个API即可完成针对于特定协议（DL/T 1997 或 DL/T 2007）的标识符数据读取、处理与存储。** 使用户无需关注请求数据的封包与接收数据的解包等复杂的协议内部操作，真正做到 **一键采集** 。

当然，由于本人精力有限，无法第一时间考虑并编写所有可能的情况与功能，所以在软件包的初期其功能只是根据个人所用到的功能进行编写，无法涵盖所有的需求。也可能会有一些小问题。不过随着时间的推移，本软件包会逐步趋向于完善，也希望使用本软件包的开发人员们能够加入到软件包的完善中来，为该软件包的成长提供一份宝贵的力量！

### 目前支持的功能：

1. DL/T 645 1997 版数据采集与部分标识符数据解析
2. DL/T 645 2007 版数据采集与部分标识符数据解析
- 标识符的解析提供了便捷的接口，用户可以调用该接口实现自己所需标识符的解析功能
- 目前还不支持数据写入功能


## 一、软件包使用说明

在使用软件包之前， 需要把 **src** 目录下的所有源文件加入工程，将 **inc** 添加到头文件目录，并且参照 **port** 路径下的 **dlt645_port.c** 源文件实现基于用户使用的硬件平台的底层硬件操作的移植。scons现在默认不会将port文件加入到工程中，使用时需在工程的其他目录下手动加入。（详细的移植方法会在下文进行描述）

当完成硬件接口移植后，可以参照以下步骤进行对软件包的使用：

1. 调用用户实现的硬件接口初始化用于DLT645采集的硬件，并注册645环境结构体。
2. 调用 `dlt645_set_addr()` 函数设置需要采集的从机设备地址。（函数详细说明请阅读 **API详解** ）
3. 调用 `dlt645_read_data()` 函数进行具体标识符数据的读取并存储到用户指定的地址下。（函数详细说明请阅读 **API详解** ）


## 二、软件包移植指南

软件包提供了完备的基于DL/T 645协议的数据包封包与解包操作，而用户则需要根据自己的平台给软件包提供底层数据的发送与接收接口，让软件包能够接收到数据并且成功发送出去。下面就来详细介绍一下软件包的移植操作。如文字描述有概念模糊或者不理解的地方可以参考本节下方的移植案例。

### port文件说明
1. **dlt645_port.c** 基于UART串口通信，文件里有485控制脚的宏开关用于简易操作485的收发功能；
2. **dlt645_rs485_port.c** 基于RS485通信，使用RT-Thread的RS485软件包的底层驱动。

### 移植步骤

1. 初始化用于dlt645协议通信的硬件。（如串口、RS485）
2. 定义一个dlt645结构体作为dlt645通信的环境结构体。
3. 实现数据发送与接收函数。（发送和接收单位为一个数据包）
4. 将数据发送与接收函数注册到dlt645环境结构体中。

#### 环境结构体：

环境结构体是本软件包运行时的内核对象，内核在工作中会调用其中的数据及接口，因此，用户提供的接口也需要注册到该结构体中。其结构如下：

```C

typedef struct dlt645
{
    uint8_t addr[6];    //从机地址
    uint8_t debug;      //调试标志
    int (*write)(struct dlt645 *ctx, uint8_t *buf, uint16_t len);     //底层写函数
    int (*read) (struct dlt645 *ctx, uint8_t *msg, uint16_t len);     //底层读函数
    void *port_data;                                                  //移植层拓展接口
} dlt645_t;

```

|成员|说明|
|--|-----|
|addr|从机六位地址数组|
|debug|内核调试开关标志|
|write|硬件层写数据接口（用户提供）|
|read|硬件层读数据接口（用户提供）|
|port_data|用户拓展接口|


#### 数据发送接口：

**接口格式：** 

```C
int (*write)(struct dlt645 *ctx, uint8_t *buf, uint16_t len);
```

**详细介绍：**

在用户想要 读/写 从机设备时软件包内核需要调用该接口发送相应的指令帧，用户需要针对于自己的硬件平台来实现该函数。该函数会传入三个参数：

1. **ctx：** 为dlt645环境结构体；
2. **buf：** 为待发送的数据首地址；
3. **len：** 为要发送的长度。

#### 数据接收接口：

**接口格式：** 

```C
int (*read) (struct dlt645 *ctx, uint8_t *msg, uint16_t len);
```

**详细介绍：**

当内核成功发送一个命令后，会立刻调用该接口进行从机设备回应数据的读取，调用时会传入三个参数：

1. **ctx：** 为dlt645环境结构体；
2. **msg：** 为接收到数据的存储地址；
3. **len：** 为允许接收数据的最大长度。

其中 **len** 参数标志着本次接收的最大长度，用户要注意对接收数据长度进行判断，若实际接收长度大于传入的允许接收数据最大长度，用户应该 **视本次数据接收无效，返回0**。

### 拓展接口

在 dlt645 结构体中提供了一个 **port_data** 成员字段，其类型为 `void *` ,用户可以利用此字段创建针对于自身应用的结构体，从而拓展更多的接口和数据字段，实现更为复杂的硬件操作。

### 移植案例

```C

/*************************************************
 Copyright (c) 2019
 All rights reserved.
 File name:     dlt645_port.c
 Description:   DLT645 移植&使用例程文件
 History:
 1. Version:    
    Date:       2019-09-19
    Author:     wangjunjie
    Modify:     
*************************************************/
#include "dlt645.h"
#include "rtthread.h"
#include "drv_gpio.h"

//DLT645采集使用的串口名
#define DLT645_SERIAL_NAME "uart4"
//RS485的收发控制引脚，如果驱动层已有485驱动，不使用填-1
#define DLT645_RS485_DE_PIN -1  //如果用PA15，改为15

//DL/T 645硬件拓展结构体
typedef struct
{
    rt_sem_t dlt645_sem;   //用于串口接收的信号量
    uint32_t byte_timeout; //字节间的超时时间
} dlt645_port_t;

static dlt645_port_t dlt645_port = {
    .dlt645_sem = RT_NULL,
    .byte_timeout = 10, //接收字节间超时时间
};
//dlt645 采集设备句柄
static rt_device_t dlt645_device = RT_NULL;
//dlt645 采集接收信号量
static struct rt_semaphore dlt645_receive_sem;
//串口配置参数
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
//dlt645 环境结构体
dlt645_t dlt645;

//串口接收数据回调函数
rt_err_t uart_handler(rt_device_t dev, rt_size_t size)
{
    //接收到一个数据释放信号量
    rt_sem_release(&dlt645_receive_sem);
    return RT_EOK;
}

/**
 * Name:    dlt645_hw_read
 * Brief:   dlt645 硬件层接收数据
 * Input:
 *  @ctx:   645运行环境
 *  @msg:   接收数据存放地址
 *  @len:   数据最大接收长度 
 * Output:  读取数据的长度
 */
static int dlt645_hw_read(dlt645_t *ctx, uint8_t *msg ,uint16_t len)
{
    //实际接收长度
    int read_len = 0;
    //清缓存变量
    uint8_t buf = 0;
    
    //清空缓存
    while(rt_device_read(dlt645_device,0,&buf,1));
    //等待串口接收到数据
    if(rt_sem_take(&dlt645_receive_sem, 1000) == -RT_ETIMEOUT)
    {
        return 0;
    }
    //每次读取一个字节的数据
    while (rt_device_read(dlt645_device, 0, msg + read_len, 1) == 1)
    {
        if(read_len > len)
        {
            return 0;
        }
        else
        {
            read_len ++;
        }
        //读取超时标志一帧数据读取完成
        if (rt_sem_take(&dlt645_receive_sem, ((dlt645_port_t *)(ctx->port_data))->byte_timeout) == -RT_ETIMEOUT)
        {
            break;
        }
    }
    return read_len;
}

/**
 * Name:    dlt645_hw_write
 * Brief:   dlt645 硬件层发送数据
 * Input:
 *  @ctx:   645运行环境
 *  @buf:   待发送数据
 *  @len:   发送长度
 * Output:  实际发送的字节数，错误返回-1
 */
static int dlt645_hw_write(dlt645_t *ctx, uint8_t *buf, uint16_t len)
{
    if(DLT645_RS485_DE_PIN != -1)
    {
        rt_pin_write(DLT645_RS485_DE_PIN, PIN_HIGH);
    }

    //串口发送数据
    int ret = rt_device_write(dlt645_device,0,buf,len);

    if(DLT645_RS485_DE_PIN != -1)
    {
        rt_pin_write(DLT645_RS485_DE_PIN, PIN_LOW);
    }
    return ret;
}


/**
 * Name:    dlt645_port_init
 * Brief:   645采集硬件层初始化
 * Input:   None
 * Output:  None
 */
int dlt645_port_init(void)
{
    //串口初始化
    dlt645_device = rt_device_find(DLT645_SERIAL_NAME);
    if (dlt645_device == RT_NULL)
    {
        rt_kprintf("cannot find device %s \r\n", DLT645_SERIAL_NAME);
        return -RT_ERROR;
    }
    if (rt_device_open(dlt645_device, RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("cannot open device %s \r\n", DLT645_SERIAL_NAME);
        return -RT_ERROR;
    }
    else
    {
        config.baud_rate = BAUD_RATE_9600;
        config.data_bits = DATA_BITS_8;
        config.stop_bits = STOP_BITS_1;
        config.parity = PARITY_NONE;
        /* 打开设备后才可修改串口配置参数 */
        rt_device_control(dlt645_device, RT_DEVICE_CTRL_CONFIG, &config);
        rt_kprintf("device %s open success \r\n", DLT645_SERIAL_NAME);
    }

    //信号量初始化
    if (rt_sem_init(&dlt645_receive_sem, "receive_sem", 0, RT_IPC_FLAG_FIFO) == RT_EOK)
    {
        dlt645_port.dlt645_sem = &dlt645_receive_sem;
    }
    else
    {
        return -RT_ERROR;
    }

    //设置串口接收回调函数
    rt_device_set_rx_indicate(dlt645_device, uart_handler);

    //485控制引脚初始化
    if(DLT645_RS485_DE_PIN != -1)
    {
        rt_pin_mode(DLT645_RS485_DE_PIN, PIN_MODE_OUTPUT);
    }
    return  RT_EOK;
}

//645结构体注册
dlt645_t dlt645 = {
    {0},
    0,
    dlt645_hw_write,
    dlt645_hw_read,
    (void *)&dlt645_port};


```

## 三、API详解

### 1、设置从机地址

**简介：** 用户在通过645协议读取某设备的数据时，首先要通过该接口设置想要读取的从机的地址。

**API格式：** 

```C
 void dlt645_set_addr(dlt645_t *ctx, uint8_t *addr);
```

**参数：**

|参数名|介绍|
|-|--------|
|ctx|645结构句柄|
|addr|6位地址数组|

**使用示例**

```C

//dlt645 环境结构体
extern dlt645_t dlt645;
//从机地址为111
uint8_t slave_addr[6] = {0x00,0x00,0x00,0x00,0x01,0x11};

dlt645_set_addr(&dlt645,slave_addr);

```

### 2、读取数据

**简介：** 用户通过调用该接口读取从机指定标识符的数据

**API格式：** 

```C
int dlt645_read_data(dlt645_t *ctx, uint32_t code, uint8_t *read_data, dlt645_protocal protocal);
```

**参数：**

|参数名|描述|
|-|--------|
|ctx|645结构句柄|
|code|标识符|
|read_data|读取数据的存储地址（注意地址长度为4字节）|
|protocal|指定协议类型（可选 **DLT645_2007** 或 **DLT645_1997** ）|

|返回值|描述|
|-|--------|
|整数|读取数据长度|
|-1|读取失败|

- **read_data** 参数为读取数据的存储地址，目前的版本中将所有的数据都转换为浮点数保存，（尚不支持超过4字节大小的数据，若有需要可以修改内核）因此 **read_data** 大小必须为4字节。后续版本会进行修改，从而支持任意大小。

- **protocal** 参数指定了读取的协议类型，可选值为：`DLT645_1997` 和 `DLT645_2007` ，分别对应1997版与2007版。

**使用示例**

```C

//dlt645 环境结构体
extern dlt645_t dlt645;
//dlt645 采集测试标识符 （A相电压）
#define DLT645_2007_READ_TEST_CODE 0x02010100
//用于存放读取数据的数组
uint8_t read_buf[4];

//读取数据
if (dlt645_read_data(&dlt645,DLT645_2007_READ_TEST_CODE,read_buf,DLT645_2007) > 0)  
{
    printf("读取成功,A相电压值为: %.2f \r\n",*(float *)read_buf);
}
else
{
    rt_kprintf("读取失败\r\n");
}

```

## 四、使用案例

```C

/*************************************************
 Copyright (c) 2019
 All rights reserved.
 File name:     sample.c
 Description:   DLT645 软件包使用样例
 History:
 1. Version:
    Date:       2019-09-23
    Author:     wangjunjie
    Modify:
*************************************************/
#include "dlt645.h"
#include "rtthread.h"
#include "stdio.h"
#include "dlt645_port.h"

// dlt645 采集测试标识符 （A相电压）
#define DLT645_2007_READ_TEST_CODE 0x02010100
#define DLT645_1997_READ_TEST_CODE 0xB611
uint8_t test_addr[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

/**
 * Name:    dlt645_read_test
 * Brief:   dlt645协议采集测试程序
 * Input:   None
 * Output:  None
 */
static void dlt645_read_test(void) {
    uint8_t read_buf[4];
    rt_memset(read_buf, 0, 4);

    // 设置从机地址
    dlt645_set_addr(&dlt645, test_addr);

    // 设置debug模式
    dlt645_set_debug(&dlt645, 0);

    // if(dlt645_read_data(&dlt645,DLT645_1997_READ_TEST_CODE,read_buf,DLT645_1997) > 0) //1997采集测试
    if (dlt645_read_data(&dlt645, DLT645_2007_READ_TEST_CODE, read_buf, DLT645_2007) > 0)  // 2007采集测试
    {
        printf("读取成功,A相电压值为: %.2f \r\n", *(float *)read_buf);
    } else {
        rt_kprintf("读取失败\r\n");
    }
}

/**
 * Name:    dlt645_entry
 * Brief:   dlt645协议测试线程
 * Input:   None
 * Output:  None
 */
void dlt645_entry(void *param) {
    // dlt645 硬件层初始化
    dlt645_port_init();
    while (1) {
        // 采集测试
        dlt645_read_test();
        rt_thread_mdelay(1000);
    }
}

int dlt645_test(void) {
    rt_thread_t tid;
    tid = rt_thread_create("dlt645", dlt645_entry, RT_NULL, 4096, 8, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
    return 0;
}
MSH_CMD_EXPORT(dlt645_test, dlt645 test);


```
终端输入 `dlt645_test` 命令打开采集功能.

## 五、注意事项

1. 目前插件不支持自动识别前导码，不会自动拼包或者过滤处理；
2. 如果串口有数据输出和输入但是打印读取失败，考虑是前导码的问题，不同厂商的仪表会有长度不一的前导码。
 - 可以用PC工具发送68开头的数据：68 AA AA AA AA AA AA 68  11 04 33 34 34 35 B1 16
 - 返回的报文68开头前的FE个数就是前导码的长度，然后配置DLT645_PREMBLE_LEN即可。

## 支持

![支持](./docs/_assets/wechat_support.png)

如果这个软件包解决了你的问题，不妨扫描上面二维码请我喝杯咖啡吧 