#include "valuepack.h"

unsigned char *valuepack_tx_buffer;
unsigned short valuepack_tx_index;
unsigned char valuepack_tx_bit_index;
unsigned char valuepack_stage;

void initValuePack(int baudrate)
{
	
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 时钟初始化
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	// 引脚初始化

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	/* PA10 USART1_Rx  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 串口初始化
	
  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode =  USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
}


// 1. 开始将数据打包，需传入定义好的数组（需保证数组长度足以存放要发送的数据）

void startValuePack(unsigned char *buffer)
{
	valuepack_tx_buffer = buffer;
	valuepack_tx_index = 1;
	valuepack_tx_bit_index = 0;
	valuepack_tx_buffer[0] = PACK_HEAD;
	valuepack_stage = 0;
}

// 2. 向数据包中放入各类数据，由于数据包的顺序结构是固定的，因此注意严格以如下的顺序进行存放，否则会出现错误

void putBool(unsigned char b)
{
if(valuepack_stage<=1)
{
   if(b)
      valuepack_tx_buffer[valuepack_tx_index] |= 0x01<<valuepack_tx_bit_index;
	 else
			valuepack_tx_buffer[valuepack_tx_index] &= ~(0x01<<valuepack_tx_bit_index);

  valuepack_tx_bit_index++;
	if(valuepack_tx_bit_index>=8)
	{
		valuepack_tx_bit_index = 0;
		valuepack_tx_index++;
	}
	valuepack_stage = 1;
}
}


void putByte(char b)
{
if(valuepack_stage<=2)
{
	if(valuepack_tx_bit_index!=0)
	{	
		valuepack_tx_index++;
	  valuepack_tx_bit_index = 0;
	}
	valuepack_tx_buffer[valuepack_tx_index] = b;
	valuepack_tx_index++;
	
	valuepack_stage = 2;
}
}
void putShort(short s)
{
if(valuepack_stage<=3)
{
		if(valuepack_tx_bit_index!=0)
	{	
		valuepack_tx_index++;
	  valuepack_tx_bit_index = 0;
	}
	valuepack_tx_buffer[valuepack_tx_index] = s&0xff;
	valuepack_tx_buffer[valuepack_tx_index+1] = s>>8;
	
	valuepack_tx_index +=2;
	valuepack_stage = 3;
}
}
void putInt(int i)
{
if(valuepack_stage<=4)
{
		if(valuepack_tx_bit_index!=0)
	{	
		valuepack_tx_index++;
	  valuepack_tx_bit_index = 0;
	}
	
	valuepack_tx_buffer[valuepack_tx_index] = i&0xff;
	valuepack_tx_buffer[valuepack_tx_index+1] = (i>>8)&0xff;
	valuepack_tx_buffer[valuepack_tx_index+2] = (i>>16)&0xff;
	valuepack_tx_buffer[valuepack_tx_index+3] = (i>>24)&0xff;
	
	valuepack_tx_index +=4;
	
	valuepack_stage = 4;
}
}
int fi;
void putFloat(float f)
{
if(valuepack_stage<=5)
{
		if(valuepack_tx_bit_index!=0)
	{	
		valuepack_tx_index++;
	  valuepack_tx_bit_index = 0;
	}
	
	fi = *(int*)(&f);
	valuepack_tx_buffer[valuepack_tx_index] = fi&0xff;
	valuepack_tx_buffer[valuepack_tx_index+1] = (fi>>8)&0xff;
	valuepack_tx_buffer[valuepack_tx_index+2] = (fi>>16)&0xff;
	valuepack_tx_buffer[valuepack_tx_index+3] = (fi>>24)&0xff;
	valuepack_tx_index +=4;
	valuepack_stage = 5;
}
}




// 3. 结束打包,函数将返回 数据包的总长度
unsigned short endValuePack()
{
  unsigned char sum=0;
	for(int i=1;i<valuepack_tx_index;i++)
	{
		sum+=valuepack_tx_buffer[i];
	}
	valuepack_tx_buffer[valuepack_tx_index] = sum;
	valuepack_tx_buffer[valuepack_tx_index+1] = PACK_TAIL;
	return valuepack_tx_index+2;
}

// 4. 发送数据

void sendBuffer(unsigned char *p,unsigned short length)
{
	  for(int i=0;i<length;i++)
   { 
      USART_SendData(USART1, *p++); 
      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) 
      {} 
    }
}





