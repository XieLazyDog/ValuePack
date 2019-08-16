#include "stm32f10x.h"
#include "valuepack.h"


RxPack rxpack;

void initLED()
{
   GPIO_InitTypeDef GPIO_InitStructure;
	
   /* GPIOA and GPIOB clock enable */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	 
}


void setLED(unsigned char on)
{
	  GPIO_WriteBit(GPIOA,GPIO_Pin_6,on);
}

int main(void)
{

	initLED();
	initValuePack(115200);
	
	while(1)
	{
    
		// 延时
		for(int i=0;i<100000;i++)
		{}
			
		// 接收数据
			if(readValuePack(&rxpack))
				setLED(rxpack.bools[0]);
	
	}	
}



