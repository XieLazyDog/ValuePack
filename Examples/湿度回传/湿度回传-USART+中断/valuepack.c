#include "valuepack.h"

unsigned char bits[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};


const unsigned int VALUEPACK_INDEX_RANGE=VALUEPACK_BUFFER_SIZE<<3; 
const unsigned short  TXPACK_BYTE_SIZE = ((TX_BOOL_NUM+7)>>3)+TX_BYTE_NUM+(TX_SHORT_NUM<<1)+(TX_INT_NUM<<2)+(TX_FLOAT_NUM<<2);
const unsigned short  RXPACK_BYTE_SIZE = ((RX_BOOL_NUM+7)>>3)+RX_BYTE_NUM+(RX_SHORT_NUM<<1)+(RX_INT_NUM<<2)+(RX_FLOAT_NUM<<2);
unsigned short rx_pack_length = RXPACK_BYTE_SIZE+3;

long rxIndex=0;
long rdIndex=0;

unsigned char vp_rxbuff[VALUEPACK_BUFFER_SIZE];
unsigned char vp_txbuff[TXPACK_BYTE_SIZE+3];

void initValuePack(int baudrate)
{
	
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
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
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(USART1, &USART_InitStructure);
	
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;				                                        
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	
	USART_Cmd(USART1, ENABLE);
}



unsigned short rdi,rdii,idl,idi;
uint32_t  idc;
unsigned int err=0;
unsigned char sum=0;
unsigned char isok;

unsigned short vp_circle_rx_index;
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE)) 
	{
		vp_rxbuff[vp_circle_rx_index] = USART1->DR;
		vp_circle_rx_index++;
		if(vp_circle_rx_index>=VALUEPACK_BUFFER_SIZE)
			vp_circle_rx_index=0;
		rxIndex++;
	}
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//procValuePack 用来处理手机传来的数据，并发送数据包
//

unsigned char readValuePack(RxPack *rx_pack_ptr)
{
 
	isok = 0;

	
	while(rdIndex<(rxIndex-((rx_pack_length))))
   rdIndex+=rx_pack_length;	
	
	while(rdIndex<=(rxIndex-rx_pack_length))
	{
		
		rdi = rdIndex % VALUEPACK_BUFFER_SIZE;
		rdii=rdi+1;
		if( vp_rxbuff[rdi]==PACK_HEAD)
		{
			if(vp_rxbuff[(rdi+RXPACK_BYTE_SIZE+2)%VALUEPACK_BUFFER_SIZE]==PACK_TAIL)
			{
				//  计算校验和
				sum=0;
			  for(short s=0;s<RXPACK_BYTE_SIZE;s++)
				{
					rdi++;
					if(rdi>=VALUEPACK_BUFFER_SIZE)
					  rdi -= VALUEPACK_BUFFER_SIZE;
					sum += vp_rxbuff[rdi];
				}	
						rdi++;
					if(rdi>=VALUEPACK_BUFFER_SIZE)
					  rdi -= VALUEPACK_BUFFER_SIZE;
					
        if(sum==vp_rxbuff[rdi]) 
				{
					//  提取数据包数据 一共有五步， bool byte short int float
					
					// 1. bool
					#if  RX_BOOL_NUM>0
					
						idc = (uint32_t)rx_pack_ptr->bools;
					  idl = (RX_BOOL_NUM+7)>>3;
					  for(idi=0;idi<idl;idi++)
					  {
					    if(rdii>=VALUEPACK_BUFFER_SIZE)
					      rdii -= VALUEPACK_BUFFER_SIZE;
					   (*((unsigned char *)idc))= vp_rxbuff[rdii];
							rdii++;
							idc++;
					  }
				  #endif
					// 2.byte
					#if RX_BYTE_NUM>0
						idc = (uint32_t)(rx_pack_ptr->bytes);
					  idl = RX_BYTE_NUM;
					  for(idi=0;idi<idl;idi++)
					  {
					    if(rdii>=VALUEPACK_BUFFER_SIZE)
					      rdii -= VALUEPACK_BUFFER_SIZE;
					    (*((unsigned char *)idc))= vp_rxbuff[rdii];
							rdii++;
							idc++;
					  }
				  #endif
					// 3.short
					#if RX_SHORT_NUM>0
						idc = (uint32_t)(rx_pack_ptr->shorts);
					  idl = RX_SHORT_NUM<<1;
					  for(idi=0;idi<idl;idi++)
					  {
					    if(rdii>=VALUEPACK_BUFFER_SIZE)
					      rdii -= VALUEPACK_BUFFER_SIZE;
					    (*((unsigned char *)idc))= vp_rxbuff[rdii];
							rdii++;
							idc++;
					  }
				  #endif
					// 4.int
					#if RX_INT_NUM>0
						idc = (uint32_t)(&(rx_pack_ptr->integers[0]));
					  idl = RX_INT_NUM<<2;
					  for(idi=0;idi<idl;idi++)
					  {
					    if(rdii>=VALUEPACK_BUFFER_SIZE)
					      rdii -= VALUEPACK_BUFFER_SIZE;
					    (*((unsigned char *)idc))= vp_rxbuff[rdii];
							rdii++;
							idc++;
					  }
				  #endif
					// 5.float
					#if RX_FLOAT_NUM>0
						idc = (uint32_t)(&(rx_pack_ptr->floats[0]));
					  idl = RX_FLOAT_NUM<<2;
					  for(idi=0;idi<idl;idi++)
					  {
					    if(rdii>=VALUEPACK_BUFFER_SIZE)
					      rdii -= VALUEPACK_BUFFER_SIZE;
					    (*((unsigned char *)idc))= vp_rxbuff[rdii];
							rdii++;
							idc++;
					  }
				  #endif
				  err = rdii;
					rdIndex+=rx_pack_length;
					isok = 1;
				}else
				{
				  rdIndex++;
			    err++;
				}
			}else
			{ 
		  rdIndex++;
			err++;
		}		
		}else
		{ 
		  rdIndex++;
			err++;
		}		
	}
	return isok;
}

void sendBuffer(unsigned char *p,unsigned short length)
{
	  for(int i=0;i<length;i++)
   { 
      USART_SendData(USART1, *p++); 
      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) 
      {} 
    }
}

unsigned short loop;
unsigned char valuepack_tx_bit_index;
unsigned char valuepack_tx_index;

void sendValuePack(TxPack *tx_pack_ptr)
{
	int i;
		vp_txbuff[0]=0xa5;
		sum=0;
	//  由于结构体中不同类型的变量在内存空间的排布不是严格对齐的，中间嵌有无效字节，因此需要特殊处理
	 
  valuepack_tx_bit_index = 0;
  valuepack_tx_index = 1;
	
  #if TX_BOOL_NUM>0 
	  
	  for(loop=0;loop<TX_BOOL_NUM;loop++)
	  {
			if(tx_pack_ptr->bools[loop])
      vp_txbuff[valuepack_tx_index] |= 0x01<<valuepack_tx_bit_index;
	  else
			vp_txbuff[valuepack_tx_index] &= ~(0x01<<valuepack_tx_bit_index);

    valuepack_tx_bit_index++;
	  if(valuepack_tx_bit_index>=8)
	  {
		  valuepack_tx_bit_index = 0;
		  valuepack_tx_index++;
	  }	
	  }
	  if(valuepack_tx_bit_index!=0)
      valuepack_tx_index++;			
	#endif
		
	 #if TX_BYTE_NUM>0 
	  
	  for(loop=0;loop<TX_BYTE_NUM;loop++)
	  {
		  vp_txbuff[valuepack_tx_index] = tx_pack_ptr->bytes[loop];
      valuepack_tx_index++;			
		}
		
	#endif
	
	#if TX_SHORT_NUM>0 
	  for(loop=0;loop<TX_SHORT_NUM;loop++)
	  {
		  vp_txbuff[valuepack_tx_index] = tx_pack_ptr->shorts[loop]&0xff;
			vp_txbuff[valuepack_tx_index+1] = tx_pack_ptr->shorts[loop]>>8;
      valuepack_tx_index+=2;			
		}
	#endif
		
	#if TX_INT_NUM>0   
	  for(loop=0;loop<TX_INT_NUM;loop++)
	  {
			i = tx_pack_ptr->integers[loop];
			
		  vp_txbuff[valuepack_tx_index] = i&0xff;
			vp_txbuff[valuepack_tx_index+1] = (i>>8)&0xff;
      vp_txbuff[valuepack_tx_index+2] =(i>>16)&0xff;
			vp_txbuff[valuepack_tx_index+3] = (i>>24)&0xff;
			
			valuepack_tx_index+=4;			
		}
	#endif
	
	#if TX_FLOAT_NUM>0   
	  for(loop=0;loop<TX_FLOAT_NUM;loop++)
	  {
			i = *(int *)(&(tx_pack_ptr->floats[loop]));
			
		  vp_txbuff[valuepack_tx_index] = i&0xff;
			vp_txbuff[valuepack_tx_index+1] = (i>>8)&0xff;
      vp_txbuff[valuepack_tx_index+2] =(i>>16)&0xff;
			vp_txbuff[valuepack_tx_index+3] = (i>>24)&0xff;
			
			valuepack_tx_index+=4;			
		}
	#endif	
	
		for(unsigned short d=1;d<=TXPACK_BYTE_SIZE;d++)
		   sum+=vp_txbuff[d];
		vp_txbuff[TXPACK_BYTE_SIZE+1] = sum;
		vp_txbuff[TXPACK_BYTE_SIZE+2] = 0x5a;
		sendBuffer(vp_txbuff,TXPACK_BYTE_SIZE+3);
}



