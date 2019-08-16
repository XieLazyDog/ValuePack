#include "stm32f10x.h"
#include "valuepack.h"

// 首先需要有 发送数据包 和 接收数据包  数据包中有不同类型变量的数组，可以在valuepack.h中定义数据包的结构

TxPack txpack;
RxPack rxpack;

int main(void)
{

	// 初始化串口 设置波特率
  initValuePack(115200);	
	
	while(1)
	{

		// 延时一段时间
		for(int i=0;i<100000;i++)
		{}
			
///////////////////////////////////////////////////////////////////////////////////////////////////////
/// 数据收发部分
			
			if(readValuePack(&rxpack))
			{
			
				// 在此读取手机传来的数据
				
				// 这里是将接收的数据原样回传
				txpack.bools[0] = rxpack.bools[0];
				txpack.bytes[0] = rxpack.bytes[0];
				txpack.shorts[0] = rxpack.shorts[0];
				txpack.integers[0] = rxpack.integers[0];
				txpack.floats[0] = rxpack.floats[0];
			
				// 你也可以把 sendValuePack放在这，这样就只有当接收到手机传来的数据包后才回传数据
				
				
			}
			
			// 在此对数据包赋值并将数据发送到手机
		
      
			sendValuePack(&txpack);

/// 数据收发结束
///////////////////////////////////////////////////////////////////////////////////////////////////////
			
	}
}



