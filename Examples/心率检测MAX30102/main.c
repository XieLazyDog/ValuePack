#include "stm32f10x.h"
#include "valuepack.h"
#include "fake_i2c.h"

#define  MAX30102_ADD 0xAe
#define  MAX30102_MODE_LED_IR 3
#define  MAX30102_MODE_IR  2
#define  MAX30102_FILTERED_VALUE_RANGE  500 


TxPack txpack;

//------------------------------------------------------------------------------------------------------
//
//  初始化心率检测模块 MAX30102 
//  
//  设置模式 : 可设置仅 心率 或 SPO2模式
//
//  设置两种光源的亮度brightness : 配置亮度，范围是00~ff 
//  
void initMAX30102(unsigned char mode,unsigned char ir_led_brightness,unsigned char red_led_brightness)
{
	init_fake_i2c(1,2,MAX30102_ADD,1);

  fake_i2c_write_reg(0x02,0xf0);
  fake_i2c_write_reg(0x03, 0x02); 
	
	fake_i2c_write_reg(0x04, 0x00);  
  fake_i2c_write_reg(0x05, 0x00); 
	fake_i2c_write_reg(0x06, 0x00); 
	
  fake_i2c_write_reg(0x08, 0x1f);                                                                                       
  fake_i2c_write_reg(0x09, mode);    
	fake_i2c_write_reg(0x0a, 0x61);               
  fake_i2c_write_reg(0x0c, ir_led_brightness);       
  fake_i2c_write_reg(0x0d, red_led_brightness);       
}


//-------------------------------------------------------------------------------------------------------
//  读取采样值
//
//  这些采样值都是经过一定的滤波处理的
//  将变量地址传入以读出数据  

void MAX30102_ReadValue( float *ir_v, float *red_v)
{
	static unsigned short last_ir,last_red;
  static float rate_ir,rate_red;
	
	unsigned char buffer[6];
	unsigned char state;
	
	unsigned short raw_value_ir;
	unsigned short raw_value_red;
	
	state = fake_i2c_read_reg(0x04);  // 读取FIFO新数据的数目，如果非0，说明有新的数据
	
	if(state)
	{
	fake_i2c_read_buff(0x07,6,buffer);
	fake_i2c_write_reg(0x04,0x00);
  fake_i2c_write_reg(0x06,0x00); 
	
	raw_value_ir = (buffer[1]<<8)+buffer[2];
	raw_value_red = (buffer[4]<<8)|buffer[5];
	
	if(raw_value_ir!=last_ir)
		rate_ir += ((raw_value_ir-last_ir)-rate_ir)*0.18f;
			
	if(raw_value_red!=last_red)
		rate_red += ((raw_value_red-last_red)-rate_red)*0.18f;
	
	last_ir = raw_value_ir;
	last_red = raw_value_red;
	}
	
			(*red_v)+=rate_red;
			(*ir_v)+=rate_ir;
			(*ir_v)*=0.8f;
			(*red_v)*=0.8f;
	
			if(*ir_v>MAX30102_FILTERED_VALUE_RANGE)
				*ir_v = MAX30102_FILTERED_VALUE_RANGE;
			if(*ir_v<-MAX30102_FILTERED_VALUE_RANGE)
				*ir_v = -MAX30102_FILTERED_VALUE_RANGE;
			if(*red_v>MAX30102_FILTERED_VALUE_RANGE)
				*red_v = MAX30102_FILTERED_VALUE_RANGE;
			if(*red_v<-MAX30102_FILTERED_VALUE_RANGE)
				*red_v = -MAX30102_FILTERED_VALUE_RANGE;	
}


///  ir:红外   red:红光   检测两种光源的反射程度
float ir_value,red_value;

int main(void)
{

	initMAX30102(MAX30102_MODE_IR,0x66,0x65);
	initValuePack(115200);
	
	while(1)
	{ 
		
		// 延时
		for(int i=0;i<20000;i++)
		{}
	
			MAX30102_ReadValue(&ir_value,&red_value);	
			
			txpack.floats[0] = ir_value;
			txpack.floats[1] = red_value;
						
			sendValuePack(&txpack);
			
	}	
}




