#include "stm32f10x.h"
#include "valuepack.h"


RxPack rxpack;


void initColorLED(uint16_t period)
{
   GPIO_InitTypeDef GPIO_InitStructure;
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	 TIM_OCInitTypeDef TIM_OCInitStructure;
   /* TIM3 clock enable */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
   /* GPIOA and GPIOB clock enable */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_AFIO, ENABLE);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
   GPIO_Init(GPIOB, &GPIO_InitStructure);

	
	 uint16_t PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
   /* Time base configuration */
   TIM_TimeBaseStructure.TIM_Period = period;
   TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
 
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
 
   /* PWM1 Mode configuration: Channel1 */
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = 0;
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
 
   TIM_OC1Init(TIM3, &TIM_OCInitStructure);
 
   TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
 
   /* PWM1 Mode configuration: Channel2 */
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = 0;
 
   TIM_OC2Init(TIM3, &TIM_OCInitStructure);
 
   TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
 
   /* PWM1 Mode configuration: Channel3 */
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = 0;
 
   TIM_OC3Init(TIM3, &TIM_OCInitStructure);
 
   TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
 
   /* PWM1 Mode configuration: Channel4 
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
   TIM_OCInitStructure.TIM_Pulse = 0;
 
   TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	 */ 
 
   TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
 
   TIM_ARRPreloadConfig(TIM3, ENABLE);
 
   /* TIM3 enable counter */
   TIM_Cmd(TIM3, ENABLE);
}


void setRGB(unsigned char r,unsigned char g,unsigned char b)
{
	  TIM3->CCR1 = r;
    TIM3->CCR2 = g;
    TIM3->CCR3 = b;		
}

int main(void)
{
 
	initColorLED(128);
	initValuePack(115200);
	
	while(1)
	{
    
		// 延时
		for(int i=0;i<100000;i++)
		{}
			
		// 接收数据
			if(readValuePack(&rxpack))
				setRGB(rxpack.bytes[0],rxpack.bytes[1],rxpack.bytes[2]);
		
	}	
}



