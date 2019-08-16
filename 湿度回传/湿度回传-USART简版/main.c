#include "stm32f10x.h"
#include "valuepack.h"

void initHumiditySensor()
{
	
    ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;	
	ADC_Init(ADC1,&ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,
    ADC_SampleTime_55Cycles5);
	 ADC_Cmd(ADC1, ENABLE); 
	 ADC_ResetCalibration(ADC1); 
	 while(ADC_GetResetCalibrationStatus(ADC1))
	 {}
     ADC_StartCalibration(ADC1);
	 while(ADC_GetCalibrationStatus(ADC1))
	 {}
	 ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

short readHumidity()
{
   return  ADC1->DR;
}

unsigned char buffer[10];
float f = 100.0f/(4096*3/3.3f);

int main(void)
{

initHumiditySensor();
initValuePack(115200);
	
while(1)
{

	// 延时
	for(int t=0;t<7200000;t++)
	{
	}
	
	startValuePack(buffer);
	putFloat(((int)(readHumidity()*f*10))*0.1f);
	sendBuffer(buffer,endValuePack());
	
		
}
}







