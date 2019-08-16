#include "stm32f10x.h"

// 本程序通过DMA和USART 进行数据包的接收和发送
// 接收的数据自动写入到buffer中，通过定时调用readValuePack()函数来解析，定时间隔建议在10ms以内。
// 数据发送也采用DMA

/// 1.指定接收缓冲区的大小 ----------------------------------------------------------------------------------
//    一般需要512字节以上，需要根据实际接收数据的速度和proc函数的频率考虑。
#define VALUEPACK_BUFFER_SIZE 1024


/// 2.指定发送到手机的数据包的结构--------------------------在发送时会自动额外在前后加上包头，包尾和校验和数据，因此会多出3个字节
///   
//    根据实际需要的变量，定义数据包中 bool byte short int float 五种类型的数目

#define TX_BOOL_NUM  0  
#define TX_BYTE_NUM  0 
#define TX_SHORT_NUM 0  
#define TX_INT_NUM   0  
#define TX_FLOAT_NUM 1  

/// 3.指定接收数据包的结构-----------------------------------------------------------------------------------
//    根据实际需要的变量，定义数据包中 bool byte short int float 五种类型的数目

#define RX_BOOL_NUM  0
#define RX_BYTE_NUM  0
#define RX_SHORT_NUM 0
#define RX_INT_NUM   0
#define RX_FLOAT_NUM 0



typedef struct   
{	
	#if TX_BOOL_NUM > 0
	unsigned char bools[TX_BOOL_NUM];
	#endif
	
	#if TX_BYTE_NUM > 0
  char bytes[TX_BYTE_NUM];
  #endif
	
	#if TX_SHORT_NUM > 0
	short shorts[TX_SHORT_NUM];	
	#endif
	
	#if TX_INT_NUM > 0
	int  integers[TX_INT_NUM];
	#endif
	
	#if TX_FLOAT_NUM > 0
	float floats[TX_FLOAT_NUM];
	#endif
	
	char space; // 无意义，只为了不让结构体为空，结构体为空会报错。
}
TxPack;
typedef struct 
{	
#if RX_BOOL_NUM > 0
	unsigned char bools[RX_BOOL_NUM];
	#endif
	
	#if RX_BYTE_NUM > 0
  char bytes[RX_BYTE_NUM];
  #endif
	
	#if RX_SHORT_NUM > 0
	short shorts[RX_SHORT_NUM];	
	#endif
	
	#if RX_INT_NUM > 0
	int  integers[RX_INT_NUM];
	#endif
	
	#if RX_FLOAT_NUM > 0
	float floats[RX_FLOAT_NUM];
	#endif
	char space; // 无意义，只为了不让结构体为空，结构体为空会报错。
}RxPack;
// 初始化 valuepack 包括一些必要的硬件外设配置

void initValuePack(int baudrate);

// 需要保证至少每秒执行10次该函数
// 该函数的主要过程是先解析接收的缓冲区，如果接收到完整的RX数据包，则解析RX数据包中的数据，然后开始串口发送TX数据包 。
// 接收到数据包时 返回 1 ，否则返回 0
unsigned char readValuePack(RxPack *rx_pack_ptr);


// 发送数据包
void sendValuePack(TxPack *tx_pack_ptr);


#define PACK_HEAD 0xa5   
#define PACK_TAIL 0x5a
