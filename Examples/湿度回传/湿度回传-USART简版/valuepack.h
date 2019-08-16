#include "stm32f10x.h"

// 本程序通过 USART 将数据包发送到手机上，不支持接收的功能


//  初始化 valuepack 一些必要的硬件外设配置
void initValuePack(int baudrate);


////////////////////////////////////////////////////////////////////////////////////////////
// 以下函数是按顺序调用的，先开始打包，然后依次存放数据，最后结束打包。结束打包后，使用串口将数组发送出去。


// 1. 开始将数据打包，需传入定义好的数组（需保证数组长度足以存放要发送的数据）

void startValuePack(unsigned char *buffer);

// 2. 向数据包中放入各类数据，由于数据包的结构是固定的，因此注意严格以如下的顺序进行存放，否则会出现错误

void putBool(unsigned char b);
void putByte(char b);
void putShort(short s);
void putInt(int i);
void putFloat(float f);

// 3. 结束打包,函数将返回 数据包的总长度
unsigned short endValuePack(void);

// 4. 发送数据

void sendBuffer(unsigned char *buffer,unsigned short length);


#define PACK_HEAD 0xa5   
#define PACK_TAIL 0x5a


