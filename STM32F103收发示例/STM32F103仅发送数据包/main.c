#include "stm32f10x.h"
#include "valuepack.h"

// 定义一个足够大的数组用于存放数据包
unsigned char buffer[50];


int main(void)
{

	// 初始化串口 设置波特率
  initValuePack(115200);
    
    
	while(1)
	{

		// 延时一段时间
		for(int i=0;i<1000000;i++)
		{}

		// 开始发送
			startValuePack(buffer);
		
		// 将要回传的值放入，注意顺序为 Bool->Byte->Short->Int->Float			
			putBool(0);
			putBool(1);
			putBool(0);
			putBool(0);
			putShort(3123);
			putInt(3124);
			putFloat(3125);
			
		// 通过串口发送，endValuePack返回了数据包的总长度
			sendBuffer(buffer,endValuePack());
			
	}
}



