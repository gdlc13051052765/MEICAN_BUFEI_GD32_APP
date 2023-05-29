
#include <stdio.h>
#include <string.h>
#include "utility.h"
#include "lm75_app.h"
#include "command.h"
#include "myiic.h"
#include "stdint.h"

uint8_t addr=0x90;
uint8_t addr1=0x91;
/*==================================================================================
* 函 数 名： I2C_LM75read
* 参    数： None
* 功能描述:  lm
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
static uint16_t I2C_LM75read(void)
{
	uint16_t readDATA=0x0000;
	uint8_t tempH=0x00;
	uint8_t tempL=0x00;
	
	IIC_Start();
	IIC_Send_Byte(addr);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x00);
	IIC_Wait_Ack();
	
	IIC_Start();
	IIC_Send_Byte(addr1);
	IIC_Wait_Ack();
	tempH=IIC_Read_Byte(1);
	tempL=IIC_Read_Byte(0);
	IIC_Stop();
	readDATA=(((uint16_t)tempH<<8 )| tempL)>>5;
	return readDATA;
}

/*==================================================================================
* 函 数 名： get_temperature_task
* 参    数： None
* 功能描述:  定时获取温度任务
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-09-29 170658
==================================================================================*/
void get_temperature_task(void* argv)
{
	float data;
	
	//debug_print("%d \r\n",I2C_LM75read());
	data=0.125*(I2C_LM75read());
	debug_print("当前温度 = %d;  %.3f ℃\r\n", I2C_LM75read(), data);
}
