#include "stm32f10x.h"

#define FAKE_I2C_SCL_PIN GPIO_Pin_6
#define FAKE_I2C_SDA_PIN GPIO_Pin_7
#define FAKE_I2C_PORT GPIOB
#define FAKE_I2C_PORT_CLK RCC_APB2Periph_GPIOB

// 这里需要注意 改变IO口也需要在这修改//////////////////////////////////////////////////////////////////////////////////////
#define FAKE_SDA_IN  FAKE_I2C_PORT->CRL &=0x0fffffff;\
                                       FAKE_I2C_PORT->CRL |=0x80000000;\
																			 FAKE_SDA_H
											
#define FAKE_SDA_OUT   FAKE_I2C_PORT->CRL &=0x0fffffff ;  FAKE_I2C_PORT->CRL |=0x30000000
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////// 下面的宏定义不需要修改
#define FAKE_SDA_H FAKE_I2C_PORT->BSRR=FAKE_I2C_SDA_PIN
#define FAKE_SDA_L FAKE_I2C_PORT->BRR=FAKE_I2C_SDA_PIN
#define FAKE_SDA  	(((FAKE_I2C_PORT->IDR&FAKE_I2C_SDA_PIN)!=0)?1:0)
#define FAKE_SCL_H  FAKE_I2C_PORT->BSRR=FAKE_I2C_SCL_PIN
#define FAKE_SCL_L  FAKE_I2C_PORT->BRR=FAKE_I2C_SCL_PIN

//// 初始化函数
////  delayloop : 用于调整通信频率
////  time_out :  用于等待Ack的时间
////  device_8bit_add : 目标设备的地址（8位，最低位补0）
////  add_byte_size : 寄存器地址占有的字节数， 1:8位地址 2:16位地址
void init_fake_i2c(unsigned short delay_loop,unsigned char time_out,unsigned char device_8bit_add,unsigned char add_byte_size);

void fake_i2c_sel_device(unsigned char add_8bit);

void fake_i2c_delay(void);
void fake_i2c_start(void);
unsigned char fake_i2c_send(unsigned char dat);
void fake_i2c_stop(void);

unsigned char fake_i2c_read(unsigned char ack);
unsigned char fake_i2c_write_reg(u16 reg,unsigned char dat);
unsigned char fake_i2c_read_reg(u16 reg);
unsigned char fake_i2c_read_buff(u16 reg,u16 length,unsigned char *buffer);






