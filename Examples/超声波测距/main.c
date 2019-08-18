#include "stm32f10x.h"
#include "valuepack.h"


#define TRIG_PIN GPIO_Pin_6
#define ECHO_PIN GPIO_Pin_7
#define USONIC_PORT GPIOA
#define USONIC_PORT_CLOCK RCC_APB2Periph_GPIOA


u16 c = 0;

TxPack txpack;

void initUltraSonic()
{
   GPIO_InitTypeDef GPIO_InitStructure;
   EXTI_InitTypeDef   EXTI_InitStructure;
   NVIC_InitTypeDef   NVIC_InitStructure;
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

   RCC_APB2PeriphClockCmd(USONIC_PORT_CLOCK, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
   GPIO_InitStructure.GPIO_Pin = TRIG_PIN ;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(USONIC_PORT, &GPIO_InitStructure);
   GPIO_InitStructure.GPIO_Pin = ECHO_PIN;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(USONIC_PORT, &GPIO_InitStructure);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7);
   EXTI_InitStructure.EXTI_Line = EXTI_Line7;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);
   NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   TIM_Cmd(TIM2,ENABLE);
	 // 虽然TIM2挂载在36MHz的APB1下，但是其实际最高时钟频率可达72MHz。
   TIM_TimeBaseStructure.TIM_Period = 65535;
   TIM_TimeBaseStructure.TIM_Prescaler = 719;  ///   719 = (72000000/100000)-1 ,每10us 计数器加一
   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
   GPIO_ResetBits(USONIC_PORT,TRIG_PIN);
}


void EXTI9_5_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line7))
	{
		//上升沿
		if(GPIO_ReadInputDataBit(USONIC_PORT,ECHO_PIN))
		{
     // 清零定时器值
		 TIM2->CNT = 0;
		 // 开始计时
		 TIM_Cmd(TIM2,ENABLE);
		 
		}else // 下降沿
		{
			// 读取定时器值
			c =	 TIM2->CNT ;
			TIM_Cmd(TIM2,DISABLE);
		}
     // 清除中断标志位
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	
}

void delay10us(void)
{
	
	for(short s=0;s<70;s++)
	{}
 
}



short procUltraSonic(void)
{

  short distance_mm;

	if(c!=0)
  distance_mm	= (17*c/10);
  GPIO_SetBits(USONIC_PORT,TRIG_PIN);
  delay10us();  
	GPIO_ResetBits(USONIC_PORT,TRIG_PIN);
	
  c=0;	
	return distance_mm;
	
}


int main(void)
{

	initUltraSonic();
	initValuePack(115200);

	while(1)
	{
    
		// 延时
		for(int i=0;i<1000000;i++)
		{}
		
	   
		txpack.shorts[0] = 	procUltraSonic();;
			
		sendValuePack(&txpack);
			
			
	}	
}




