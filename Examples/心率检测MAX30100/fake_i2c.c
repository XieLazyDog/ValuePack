#include "fake_i2c.h"

unsigned char address_byte_size;
unsigned  char device_8bit_add;
unsigned short delayloop;
unsigned char  timeout;

void fake_i2c_delay(void)
{
for(int i=0;i<delayloop*36;i++)
	{}
}

void init_fake_i2c(unsigned short delay_loop,unsigned char time_out,unsigned char device8bit_add,unsigned char add_byte_size)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	timeout = time_out;
	delayloop = delay_loop;
	device_8bit_add = device8bit_add;
	address_byte_size = add_byte_size;
	
   RCC_APB2PeriphClockCmd(FAKE_I2C_PORT_CLK, ENABLE);
	
   GPIO_InitStructure.GPIO_Pin = FAKE_I2C_SDA_PIN|FAKE_I2C_SCL_PIN;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(FAKE_I2C_PORT, &GPIO_InitStructure);
   FAKE_I2C_PORT->BSRR = FAKE_I2C_SCL_PIN|FAKE_I2C_SDA_PIN;
	
}

void fake_i2c_sel_device(unsigned char add_8bit)
{
	device_8bit_add = add_8bit;
}

void fake_i2c_start(void)
{
	FAKE_SDA_OUT;
	FAKE_SDA_H;
	FAKE_SCL_H;
	fake_i2c_delay();
	FAKE_SDA_L;
	fake_i2c_delay();
}

unsigned char fake_i2c_send(unsigned char dat)
{
	for(u8 i=0;i<8;i++)
	{
		FAKE_SCL_L;
		fake_i2c_delay();
		FAKE_SDA_OUT;
		if(dat&0x80)
			FAKE_SDA_H;
		else
			FAKE_SDA_L;
		dat = dat<<1;
		fake_i2c_delay();
		FAKE_SCL_H;
		fake_i2c_delay();
		fake_i2c_delay();
	}

  FAKE_SCL_L;
	fake_i2c_delay();
	  FAKE_SDA_IN;	
	fake_i2c_delay();
	FAKE_SCL_H;
	fake_i2c_delay();
	
  dat = 1;	
	for(u8 i=0;i<timeout;i++)
	{
		fake_i2c_delay();
	 if(FAKE_SDA==0)
	 { 
	   dat= 0;
		 break;
	 }
	}
	
	FAKE_SDA_L;
	FAKE_SDA_OUT;
		FAKE_SCL_L;
	fake_i2c_delay();
	return dat;
}

void fake_i2c_stop(void)
{
	FAKE_SCL_L;
		fake_i2c_delay();
	FAKE_SDA_OUT;
	FAKE_SDA_L;
	
	fake_i2c_delay();
	FAKE_SCL_H;
	fake_i2c_delay();
	FAKE_SDA_H;
	fake_i2c_delay();

}

unsigned char fake_i2c_read(unsigned char ack)
{
	u8 dat=0;

	
		for(u8 i=0;i<8;i++)
	{
		dat = dat<<1;
		FAKE_SCL_L;
		fake_i2c_delay();
		FAKE_SDA_IN;
		fake_i2c_delay();
		FAKE_SCL_H;
		fake_i2c_delay();

		dat = (dat|FAKE_SDA);
		fake_i2c_delay();

	}
	  FAKE_SCL_L;
		fake_i2c_delay();
		FAKE_SDA_OUT;
	if(ack)
		FAKE_SDA_L;
	else
		FAKE_SDA_H;
	
	fake_i2c_delay();
	FAKE_SCL_H;
	fake_i2c_delay();

	return dat;
}

unsigned char fake_i2c_write_reg(unsigned short  reg, unsigned char dat)
{
	
	fake_i2c_start();
	fake_i2c_send(device_8bit_add);
	fake_i2c_send(reg);
	fake_i2c_send(dat);
	fake_i2c_stop();

	return 0;
}


unsigned char fake_i2c_read_reg(unsigned short  reg)
{
  u8 err=0;
  u8 d;
	fake_i2c_start();
	d=fake_i2c_send(device_8bit_add);
	
	if(d)
	{
	err = 1;
	}
	
	 d=fake_i2c_send(reg);
	
	if(d)
	{
	 err = 2;
	}
	
	fake_i2c_delay();

	
	
	
	fake_i2c_start();
	
	
	d=fake_i2c_send(device_8bit_add+1);
	
	if(d)
	{
	  err = 3;
	}
	
	d = fake_i2c_read(0);
	
	
	
	fake_i2c_stop();
	
if(err)
  return err;
else
	return d;

}
unsigned char fake_i2c_read_buff(unsigned short  reg,unsigned short length,unsigned char *buffer)
{

  u8 err=0;
  u8 d;
	fake_i2c_start();
	d=fake_i2c_send(device_8bit_add);
	
	if(d)
	{
	err = 1;
	}
  fake_i2c_send(reg);
	if(d)
	{
	 err = 2;
	}
	fake_i2c_delay();
	fake_i2c_start();
	d=fake_i2c_send(device_8bit_add+1);
	if(d)
	{
	  err = 3;
	}
	for(int i=0;i<length-1;i++)
	buffer[i] = fake_i2c_read(1);
  buffer[length-1] = fake_i2c_read(0);
	fake_i2c_stop();
  return err;
}



