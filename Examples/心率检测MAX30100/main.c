#include "stm32f10x.h"
#include "valuepack.h"
#include "fake_i2c.h"

#define  MAX30100_ADD 0xAe
#define  MAX30100_MODE_LED_IR 3
#define  MAX30100_MODE_IR  2

#define  MAX30100_FILTERED_VALUE_RANGE  300 

TxPack txpack;

//------------------------------------------------------------------------------------------------------
//
//  初始化心率检测模块 MAX30100 
//  
//  设置模式 : 可设置仅 心率 或 SPO2模式
//
//  设置两种光源的亮度brightness : 配置亮度，范围是0~f 
//  
void initMAX30100(unsigned char mode,unsigned char ir_led_brightness,unsigned char red_led_brightness)
{
	init_fake_i2c(1,2,MAX30100_ADD,1);
	
  fake_i2c_write_reg(0x06,0x08|mode); //设置模式
	fake_i2c_write_reg(0x01,0xF0);      //开启中断
	fake_i2c_write_reg(0x09,ir_led_brightness|(red_led_brightness<<4));        //LED设置 
	fake_i2c_write_reg(0x07,0x43);      // 配置为16位
	fake_i2c_write_reg(0x02,0x00);      
	fake_i2c_write_reg(0x03,0x00);      
	fake_i2c_write_reg(0x04,0x00); 
}


//-------------------------------------------------------------------------------------------------------
//  读取采样值
//
//  这些采样值都是经过一定的滤波处理的
//  将变量地址传入以读出数据  

void MAX30100_ReadValue( float *ir_v, float *red_v)
{
	static unsigned short last_ir,last_red;
  static float rate_ir,rate_red;
	
	unsigned char buffer[4];
	unsigned char state;
	
	unsigned short raw_value_ir;
	unsigned short raw_value_red;
	
	state = fake_i2c_read_reg(0x02);  // 读取FIFO新数据的数目，如果非0，说明有新的数据
	
	if(state)
	{
	fake_i2c_read_buff(0x05,4,buffer);
	fake_i2c_write_reg(0x02,0x00);
  fake_i2c_write_reg(0x04,0x00); 
	
	raw_value_ir = (buffer[0]<<8)+buffer[1];
	raw_value_red = (buffer[2]<<8)|buffer[3];
	
	if(raw_value_ir!=last_ir)
		rate_ir += ((raw_value_ir-last_ir)-rate_ir)*0.18f;
			
	if(raw_value_red!=last_red)
		rate_red += ((raw_value_red-last_red)-rate_red)*0.18f;
	
	last_ir = raw_value_ir;
	last_red = raw_value_red;
	}
	
			(*red_v)+=rate_red;
			(*ir_v)+=rate_ir;
			(*ir_v)*=0.9f;
			(*red_v)*=0.9f;
	
			if(*ir_v>MAX30100_FILTERED_VALUE_RANGE)
				*ir_v = MAX30100_FILTERED_VALUE_RANGE;
			if(*ir_v<-MAX30100_FILTERED_VALUE_RANGE)
				*ir_v = -MAX30100_FILTERED_VALUE_RANGE;
			if(*red_v>MAX30100_FILTERED_VALUE_RANGE)
				*red_v = MAX30100_FILTERED_VALUE_RANGE;
			if(*red_v<-MAX30100_FILTERED_VALUE_RANGE)
				*red_v = -MAX30100_FILTERED_VALUE_RANGE;	
}


///  ir:红外   red:红光   检测两种光源的反射程度
float ir_value,red_value;

int main(void)
{

	initMAX30100(MAX30100_MODE_LED_IR,5,5);
	initValuePack(115200);
	
	while(1)
	{ 
		
		// 延时
		for(int i=0;i<20000;i++)
		{}
	
			MAX30100_ReadValue(&ir_value,&red_value);	
			
			txpack.floats[0] = ir_value;
			txpack.floats[1] = red_value;
						
			sendValuePack(&txpack);
			
	}	
}




